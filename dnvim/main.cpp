#include "nvimlauncher.h"
#include <plog/Log.h>
#include <plog/Appenders/DebugOutputAppender.h>
#include "windowsutils.h"


#define LAUNCH_COMMAND L"nvim --embed"
#define WINDOW_CLASS L"dnvim_ui"
#define WINDOW_TITLE L"dnvim"
auto shaderResource = L"SHADERSOURCE";


int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	static plog::DebugOutputAppender<plog::TxtFormatter> debugOutputAppender;
	plog::init(plog::verbose, &debugOutputAppender);

	LOGI << "launch nvim";
	NVim nvim;

	LOGI << "create window";
	auto wnd = UIWindow::Create(hInstance, nCmdShow, 
		WINDOW_CLASS, WINDOW_TITLE,
		640, 480);
	if (!wnd) {
		return 1;
	}

	if (!nvim.Initialize(hInstance, wnd->Get(), shaderResource))
	{
		return 2;
	}
	nvim.Launch(LAUNCH_COMMAND);

	wnd->AddOnSize([&nvim](int w, int h) {
		nvim.Resize(w, h);
	});

	wnd->AddOnKeyDown([&nvim](char code) {
		char keys[] = { code, 0 };
		nvim.Input(keys);
	});

	LOGI << "start main loop";
	MSG msg;
	int exit_status;

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		if (nvim.GetExitStatus(&exit_status)) {
			PostQuitMessage(0);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return exit_status;
}
