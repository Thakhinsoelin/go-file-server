#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <new>

#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, 
					HINSTANCE hPrevInstance, 
					PWSTR pCmdLine, 
					int nCmdShow)
{
	MainWindow win;
	if (!win.Create(L"စိုးသီဟဖိုင်ဆာဗာ", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//MessageBoxA(NULL, "Hello, World!", "NativeFront", MB_OK);
	return 0;
}

