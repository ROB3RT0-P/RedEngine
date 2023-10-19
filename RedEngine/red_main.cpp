//--------------------------------
// red_main.cpp - RJP - 19/10/2023
//--------------------------------

#include "red_engine.h"
#include "core.h"
#include "list.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

LPCWSTR g_szAppName = L"Red Engine";
static const float MinFrameTime = 1.0f / 75.0f - 0.0001f;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	memory::Heap::Create();

	if (!XMVerifyCPUSupport())
		return 1;

	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return 1;

	DEBUG_MESSAGE("Creating core object.\n");
	Core* const core = new Core();

	// Register class and create window
	{
		// Register class
		WNDCLASSEXW wcEx;
		wcEx.cbSize = sizeof(WNDCLASSEXW);
		wcEx.style = CS_HREDRAW | CS_VREDRAW;
		wcEx.lpfnWndProc = WndProc;
		wcEx.cbClsExtra = 0;
		wcEx.cbWndExtra = 0;
		wcEx.hInstance = hInstance;
		wcEx.hIcon = LoadIconW(hInstance, L"IDI_ICON");
		wcEx.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcEx.lpszMenuName = nullptr;
		wcEx.lpszClassName = L"DirectXTKSimpleSampleWindowClass";
		wcEx.hIconSm = LoadIconW(wcEx.hInstance, L"IDI_ICON");
		if (!RegisterClassExW(&wcEx))
			return 1;

		// Create window
		int width = 1280, height = 720;

		RECT rc = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		HWND hWnd = CreateWindowExW(0, L"DirectXTKSimpleSampleWindowClass", g_szAppName, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
			nullptr);

		if (!hWnd)
			return 1;

		ShowWindow(hWnd, nCmdShow);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(core));

		GetClientRect(hWnd, &rc);

		core->Initialise(hWnd, rc.right - rc.left, rc.bottom - rc.top);
	}

	utils::Timers::InitialiseTimers();

	// Main message loop
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			utils::Timers::UpdateFrameTimer();
			const float frameTime = utils::Timers::GetFrameTime();
			if (frameTime > MinFrameTime)
				core->Update();
			core->Render();
		}
	}

	core->Shutdown();

	delete core;

	CoUninitialize();

	memory::Heap::Destroy();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//PAINTSTRUCT ps;
	//HDC hdc;

	//Core* const core = reinterpret_cast<Core*>( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );

	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void WINAPI wWinMainCRTStartup()
{
	STARTUPINFO				startupInfo = { sizeof(STARTUPINFO),0 };

	GetStartupInfo(&startupInfo);

	WCHAR* commandLine = GetCommandLine();

	// Skip past program name (first token in command line).
	if (*commandLine == '"') // check for and handle quoted program name
	{
		// Scan, and skip over, subsequent characters until another
		// double-quote or a null is encountered
		while (*commandLine && (*commandLine != '"'))
			commandLine++;

		// If we stopped on a double-quote (usual case), skip over it.
		if (*commandLine == '"')
			commandLine++;
	}
	else
	{
		// First token wasn't a quote
		while (*commandLine > ' ')
			commandLine++;
	}

	// Skip past any white space preceeding the second token.
	while (*commandLine && (*commandLine <= ' '))
		commandLine++;

	int result = wWinMain(GetModuleHandle(NULL), 0, commandLine, startupInfo.dwFlags & STARTF_USESHOWWINDOW
		? startupInfo.wShowWindow : SW_SHOWDEFAULT);

	ExitProcess(result);
}