#include <windows.h>

int Running = 1;

int WindowWidth = 640;
int WindowHeight = 480;

void* BackBuffer;

BITMAPINFO BitMapInfo = { 0 };


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;

	switch (uMsg)
	{
		case WM_DESTROY:
			Running = 0;
			break;

		case WM_KEYUP:
			Running = 0;
			break;
		
		default:
			Result = DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
	}

	return Result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	//define window class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "Module 3";

	RegisterClassEx(&wc);

	DWORD dwExStyle = 0;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;

	//set up full screen
	BOOL FullScreen = TRUE;

	if (FullScreen)
	{
		DEVMODE dmScreen = { 0 };
		dmScreen.dmSize = sizeof(dmScreen);
		
		dmScreen.dmPelsWidth = WindowWidth;
		dmScreen.dmPelsHeight = WindowHeight;
		dmScreen.dmBitsPerPel = 32;
		dmScreen.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreen, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP;
		}
		else
		{
			FullScreen = FALSE;
		}
	}

	RECT r = { 0 };
	r.left = 0;
	r.right = WindowWidth;
	r.top = 0;
	r.bottom = WindowHeight;

	AdjustWindowRectEx(&r, dwStyle, 0, dwExStyle);

	//create window
	HWND MainWindow = CreateWindowEx(dwExStyle, "Module 3", "Lesson 3", dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, NULL, NULL, hInstance, 0);

	if (FullScreen)
		SetWindowLong(MainWindow, GWL_STYLE, 0);

	ShowWindow(MainWindow, nShowCmd);

	//define the bitmap info
	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biWidth = WindowWidth;
	BitMapInfo.bmiHeader.biHeight = WindowHeight;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 32;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;

	BackBuffer = malloc(WindowWidth * WindowHeight * 4);

	MSG msg;
	while (Running)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		int *MemoryWalker = (int*)BackBuffer;

		for (int Height = 0; Height < WindowHeight; Height++)
		{
			for (int Width = 0; Width < WindowWidth; Width++)
			{
				char Red = rand() % 256;
				char Green = rand() % 100;
				char Blue = rand() % 200;

				*MemoryWalker++ = ((Red << 16) | (Green << 8) | Blue);
			}
		}

		HDC dc = GetDC(MainWindow);
		StretchDIBits(dc, 0, 0, WindowWidth, WindowHeight, 0, 0, WindowWidth, WindowHeight, BackBuffer, &BitMapInfo, DIB_RGB_COLORS, SRCCOPY);
		DeleteDC(dc);
	}

	return EXIT_SUCCESS;
}