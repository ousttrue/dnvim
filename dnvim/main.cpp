#include "d3d/D3D11Manager.h"
#include "d3d/resource.h"
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
	nvim.Launch(LAUNCH_COMMAND);

	LOGI << "create window";
	auto wnd = UIWindow::Create(hInstance, nCmdShow, 
		WINDOW_CLASS, WINDOW_TITLE);
	if (!wnd) {
		return 1;
	}

	Resource res(hInstance, ID_SHADERSOURCE, shaderResource);
	auto shaderSource = res.GetString();
	if (shaderSource.empty()) {
		return 2;
	}

	LOGI << "d3d initialize";
	D3D11Manager d3d11;
	if (!d3d11.Initialize(wnd->Get(), shaderSource, L"")) {
		return 3;
	}

	wnd->AddOnSize(std::bind(&D3D11Manager::Resize, &d3d11, std::placeholders::_1, std::placeholders::_2));
	wnd->AddOnKeyDown([&nvim](char code) {
		char keys[] = { code, 0 };
		nvim.Input(keys);
	});

	LOGI << "start main loop";
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
