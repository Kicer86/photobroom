
#include <windows.h>
#include <tchar.h>

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	SetDllDirectory("./lib");
	CreateProcess(NULL, "./bin/photo_broom.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
