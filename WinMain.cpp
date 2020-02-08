#include "Config.h"
#include "GameBoy.h"
#include "GameSettings.h"
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <sstream>
#include <stdexcept> // For std::runtime_error
#include <string>    // For std::wstring
#include <Windows.h> // For Win32 API
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */

#define MAX_PATH 260

//// Represents an error in a call to a Win32 API.
//class win32_error : public std::runtime_error
//{
//public:
//    win32_error(const char* msg, DWORD error)
//        : std::runtime_error(msg)
//        , _error(error)
//    { }
//
//    DWORD error() const
//    {
//        return _error;
//    }
//
//private:
//    DWORD _error;
//};
//
//char* do_open_rom_dialog(void)
//{
//    OPENFILENAMEW dialog;
//    wchar_t filename[MAX_PATH] = { 0 };
//
//    memset(&dialog, 0, sizeof(dialog));
//    dialog.lStructSize = sizeof(dialog);
//    dialog.lpstrFile = filename;
//    dialog.nMaxFile = sizeof(filename);
//    dialog.lpstrFilter = L"Game Boy ROMs\0*.gb;*.sgb\0All files\0*.*\0\0";
//    dialog.nFilterIndex = 1;
//    dialog.lpstrFileTitle = NULL;
//    dialog.nMaxFileTitle = 0;
//    dialog.lpstrInitialDir = NULL;
//    dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//
//    BOOL GetOpenFileNameA(
//        LPOPENFILENAMEA Arg1
//    );
//
//    return NULL;
//}

int main(int argc, char *argv[])
{
	printf(std::to_string(argc).c_str());
	if(argc < 2) {
		printf("Pass rom as argument.");
		return 1;
	}

	LogMessage* log = LogMessage::CreateInstance() ;
	GameSettings* settings = GameSettings::CreateInstance();

	GameBoy* gb = GameBoy::CreateInstance( argv[1] ) ;

	gb->StartEmulation( ) ;

	delete gb ;
	delete settings ;
	delete log ;


	return 0;
}
