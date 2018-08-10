#include "D3D11Manager.h"
#include "resource.h"
#include "nvimlauncher.h"
#include <plog/Log.h>
#include <plog/Appenders/DebugOutputAppender.h>
//#include <plog/Appenders/ColorConsoleAppender.h>

#include <Windows.h>


#define LAUNCH_COMMAND L"nvim --embed"
#define WINDOW_CLASS L"dnvim_ui"
#define WINDOW_TITLE L"dnvim"
auto textureFile = L"../MinTriangle/texture.png";




static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto d3d = (D3D11Manager*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (message)
	{
	case WM_CREATE:
	{
		auto d3d = (D3D11Manager*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)d3d);
		break;
	}

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE:
		d3d->Resize(LOWORD(wParam), HIWORD(wParam));
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


static HWND CreateUIWindow(HINSTANCE hInstance, int nCmdShow, D3D11Manager &d3d11)
{
	{
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;//MAKEINTRESOURCE(IDC_D3D11SAMPLE);
		wcex.lpszClassName = WINDOW_CLASS;
		wcex.hIconSm = NULL;
		RegisterClassEx(&wcex);
	}
	HWND hWnd = CreateWindow(WINDOW_CLASS, WINDOW_TITLE, WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT, CW_USEDEFAULT
		, 320, 320
		, NULL, NULL, hInstance, &d3d11);
	if (!hWnd)
	{
		return nullptr;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}


static std::string GetShader(HINSTANCE hInst)
{
	HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(ID_SHADERSOURCE), L"SHADERSOURCE");
	if (!hRes) {
		return "";
	}
	HGLOBAL hMem = LoadResource(hInst, hRes);
	DWORD size = SizeofResource(hInst, hRes);
	char* resText = (char*)LockResource(hMem);
	char* text = (char*)malloc(size + 1);
	memcpy(text, resText, size);
	text[size] = 0;
	std::string s(text);
	free(text);
	FreeResource(hMem);
	return s;
}


int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if 1
	static plog::DebugOutputAppender<plog::TxtFormatter> debugOutputAppender;
	plog::init(plog::verbose, &debugOutputAppender);
#else
	static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(plog::verbose, &consoleAppender);
#endif

	NVim nvim;

	LOGI << "launch nvim";
	nvim.Launch(LAUNCH_COMMAND);

	LOGI << "create window";
	D3D11Manager d3d11;
	auto hWnd = CreateUIWindow(hInstance, nCmdShow, d3d11);

	auto shaderSource = GetShader(hInstance);
	if (shaderSource.empty()) {
		return 5;
	}

	// d3d
	if (!d3d11.Initialize(hWnd, shaderSource, textureFile)) {
		return 2;
	}

	// main loop
	MSG msg;
	int exit_status;
	while (true)
	{
		if (nvim.GetExitStatus(&exit_status)) {
			break;
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0)) {
				exit_status = (int)msg.wParam;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			d3d11.Render();
		}
	}

	return exit_status;
}
