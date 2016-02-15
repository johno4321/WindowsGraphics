#include "quakeDef.h"

#include "host.h"

int Running = 1;

void* BackBuffer;

HWND MainWindow; 

BITMAPINFO BitMapInfo = { 0 };

//////////////////////////////
//      TIMER CODE
//////////////////////////////

static double GTimePassed = 0;
static double SecondsPerTick = 0;
static __int64 GTimeCount = 0;

float Sys_InitFloatTime(void)
{
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);

	SecondsPerTick = 1.0 / (double)Frequency.QuadPart;

	LARGE_INTEGER Counter;
	QueryPerformanceCounter(&Counter);

	GTimeCount = Counter.QuadPart;

	return 0;
}

float Sys_FloatTime(void)
{
	LARGE_INTEGER Counter;
	QueryPerformanceCounter(&Counter);

	__int64 Interval = Counter.QuadPart - GTimeCount;
	GTimeCount = Counter.QuadPart;

	double SecondsGoneBy = (double)Interval * SecondsPerTick;
	GTimePassed += SecondsGoneBy;

	return (float)GTimePassed;
}

//////////////////////////////
//      END TIMER CODE
//////////////////////////////


void UpdateGraphcis(void)
{
	int *MemoryWalker = (int*)BackBuffer;

	for (int Height = 0; Height < BufferHeight; Height++)
	{
		for (int Width = 0; Width < BufferWidth; Width++)
		{
			char Red = 255;
			char Green = 255;
			char Blue = 255;

			if (Height % 2 == 0)
				if (Width % 2 == 0)
				{
					Red = 0;
					Blue = rand() % 256;
				}


			*MemoryWalker++ = ((Red << 16) | (Green << 8) | Blue);
		}
	}

	HDC dc = GetDC(MainWindow);
	StretchDIBits(dc, 0, 0, BufferWidth, BufferHeight, 0, 0, BufferWidth, BufferHeight, BackBuffer, &BitMapInfo, DIB_RGB_COLORS, SRCCOPY);
	DeleteDC(dc);
}

void Sys_Shutdown(void)
{
	Running = FALSE;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;

	switch (uMsg)
	{
		case WM_ACTIVATE:
			break;
		
		case WM_CREATE:
			break;

		case WM_DESTROY:
			Sys_Shutdown();
			break;

		case WM_KEYUP:
			Sys_Shutdown();
			break;
		
		default:
			Result = DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
	}

	return Result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	//define window class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "Module 3";

	if (!RegisterClass((const WNDCLASS *)&wc))
		exit(EXIT_FAILURE);

	DWORD dwExStyle = 0;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;

	//set up full screen
	BOOL FullScreen = TRUE;

	if (FullScreen)
	{
		DEVMODE dmScreen = { 0 };
		dmScreen.dmSize = sizeof(dmScreen);
		
		dmScreen.dmPelsWidth = BufferWidth;
		dmScreen.dmPelsHeight = BufferHeight;
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
	r.right = BufferWidth;
	r.top = 0;
	r.bottom = BufferHeight;

	AdjustWindowRectEx(&r, dwStyle, 0, dwExStyle);

	//create window
	MainWindow = CreateWindowEx(dwExStyle, "Module 3", "Lesson 3", dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, NULL, NULL, hInstance, 0);

	if (FullScreen)
		SetWindowLong(MainWindow, GWL_STYLE, 0);

	ShowWindow(MainWindow, nShowCmd);

	//define the bitmap info
	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biWidth = BufferWidth;
	BitMapInfo.bmiHeader.biHeight = BufferHeight;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 32;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;

	BackBuffer = malloc(BufferWidth * BufferHeight * 4);

	Host_Init();

	float oldtime = Sys_InitFloatTime();
	float TargetTime = 1.0f / 60.0f;
	
	MSG msg;
	while (Running)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UpdateGraphcis();

		float newtime = Sys_FloatTime();
		if (newtime - oldtime > TargetTime)
		{
			Host_Frame(TargetTime);
			oldtime = newtime;
		}
	}

	Host_Shutdown();

	return EXIT_SUCCESS;
}

