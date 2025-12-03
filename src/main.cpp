
// ########################################################
//                          Platform Globels
// ########################################################
static bool running = true;

// ########################################################
//                          Platform Functions
// ########################################################
bool platfrom_create_window(int width, int height, char* title);
void platform_update_window();

// ########################################################
//                          Windows Platform
// ########################################################

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>


// ########################################################
//                          Windows globals
// ########################################################
static HWND window;


// ########################################################
//                          Windows Platform implemataion
// ########################################################
LRESULT CALLBACK Windows_windows_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (msg)
	{
		case WM_CLOSE:
		{
			running = false;
			break;
		}
			
		default:
		{
			// let windows handle the default input for now
			result = DefWindowProc(window,msg,wParam,lParam);
			break;
		}
	}

	return result;
}


bool platfrom_create_window(int width, int height, char* title)
{

	HINSTANCE instance = GetModuleHandleA(0);


	WNDCLASSA wc = {};
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(instance,IDI_APPLICATION);
	wc.hCursor = LoadCursor(instance, IDC_ARROW);
	wc.lpszClassName = title;
	wc.lpfnWndProc = Windows_windows_callback;   // callback for Input into the Window

	if(!RegisterClassA(&wc))
	{
		return false;
	}

// WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXMIZEBOX
	int dwStyle = WS_OVERLAPPEDWINDOW;

	window = CreateWindowExA(
		0,
		title, // ti is not for title, it is just a Unique Identifier(ID) for owr windows
		title, // this is window title 
		dwStyle,
		100,
		100,
		width,
		height,
		NULL,
		NULL,
		instance,
		NULL
	);

	if(window == NULL)
	{
		return false;
	}

	ShowWindow(window,SW_SHOW);

	return true;
}

void platform_update_window()
{
	MSG msg;

	while (PeekMessageA(&msg,window,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);  // calls the callback specified when creating the window
	}
	
}

#endif



int main()
{
	platfrom_create_window(1200,720,"My Clester");
	while (running)
	{
		// Update
		platform_update_window();
	}

	return 0;
}
