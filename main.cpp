#include "D3D11Manager.h"
#include "resource.h"

#include <process.hpp>
#include <string>
#include <chrono>
#include <thread>
#include <msgpackpp.h>
#include <vector>
#include <functional>

#include <plog/Log.h>
#include <plog/Appenders/DebugOutputAppender.h>
//#include <plog/Appenders/ColorConsoleAppender.h>

#include <Windows.h>

class Dispatcher
{
	std::vector<uint8_t> m_buffer;
	std::unordered_map<std::string, msgpackpp::procedurecall> m_method_map;

public:
	Dispatcher()
	{
	}

	void push_bytes(const char *bytes, size_t n)
	{
		auto pos = m_buffer.size();
		m_buffer.resize(pos + n);
		for (int i = 0; i < n; ++i, ++pos)
		{
			m_buffer[pos] = bytes[i];
		}

		while (!m_buffer.empty()) {
			if (!consume()) {
				break;
			}
		}
	}

	void add_method(const std::string &method_name, const msgpackpp::procedurecall &proc)
	{
		m_method_map.insert(std::make_pair(method_name, proc));
	}

private:
	void redraw(const msgpackpp::parser &args)
	{
		if (args.is_array()) {
			LOGD << "[redraw] " << args.count();
			auto child = args[0];
			for (int i = 0; i < args.count(); ++i) {
				
				auto cmd = child[0].get_string();

				auto found = m_method_map.find(cmd);
				if (found != m_method_map.end()) {
					// found
					auto proc = found->second;

					try {
						proc(child[1]);
					}
					catch (const std::exception &ex)
					{
						LOGE << ex.what() << " " << child[1];
					}
				}
				else {
					LOGE << ", unknown " << cmd;
				}

				child = child.next();
			}
		}
		else {

			LOGW << "redraw";

		}
	}

	bool consume()
	{
		auto msg = msgpackpp::parser(m_buffer.data(), static_cast<int>(m_buffer.size()));
		auto d = msg.consumed_size();
		m_buffer.erase(m_buffer.begin(), m_buffer.begin() + d);

		if (!msg.is_array()) {
			throw std::exception("is not error");
		}

		auto msgType = msg[0].get_number<int>();
		if (msgType == 0) {
			// request
			throw std::exception("not implemented");
		}
		else if (msgType == 1) {
			// response
			auto msgId = msg[1].get_number<int>();
			auto error = msg[2];
			auto payload = msg[3];

			LOGW
				<< "response: " << msgId
				//<< " => " << payload 
				;
		}
		else if (msgType == 2) {
			// notify
			auto method = msg[1].get_string();

			if (method == "redraw") {

				redraw(msg[2]);

			}
			else {

				LOGW
					<< "notify: " << method
					//<<  " => " << payload 
					;
			}

		}
		else {
			throw std::exception("invali msgType");
		}
	}
};


struct Highlight
{
	bool bold = false;
	int foreground = 255;
};
MPPP_MAP_SERIALIZER(Highlight, 2, bold, foreground);


///
/// https://github.com/neovim/neovim/blob/master/runtime/doc/ui.txt
///
class Grid
{
public:
	void option_set(std::string key, bool enable) {

	}

	void default_colors_set(int i0, int i1, int i2, int i3, int i4) {

	}

	void update_fg(int g) {
	}

	void update_bg(int g) {
	}

	void update_sp(int g) {
	}

	void resize(int cols, int rows) {

	}

	void clear() {

	}

	void cursor_goto(int col, int row) {

	}

	void highlight_set(Highlight hl) {

	}

	void put(std::string str)
	{

	}

	void mode_info_set(bool cursor_style_enabled, msgpackpp::parser map)
	{
		//std::cout << "mode_info_set: " << map;
	}

	void mode_change(std::string mode, int value)
	{

	}
};


#define LAUNCH_COMMAND L"nvim --embed"
#define WINDOW_CLASS L"dnvim_ui"
#define WINDOW_TITLE L"dnvim"
auto textureFile = L"../MinTriangle/texture.png";


class NVim
{
	Dispatcher m_dispatcher;
	Grid m_grid;
	std::shared_ptr<TinyProcessLib::Process> m_process;

public:
	NVim()
	{
#define DISPATCHER_ADD_METHOD(method) m_dispatcher.add_method(#method, msgpackpp::make_methodcall(&m_grid, &Grid::method))
		DISPATCHER_ADD_METHOD(option_set);
		DISPATCHER_ADD_METHOD(default_colors_set);
		DISPATCHER_ADD_METHOD(update_fg);
		DISPATCHER_ADD_METHOD(update_bg);
		DISPATCHER_ADD_METHOD(update_sp);
		DISPATCHER_ADD_METHOD(resize);
		DISPATCHER_ADD_METHOD(clear);
		DISPATCHER_ADD_METHOD(cursor_goto);
		DISPATCHER_ADD_METHOD(highlight_set);
		DISPATCHER_ADD_METHOD(put);
		DISPATCHER_ADD_METHOD(mode_info_set);
		DISPATCHER_ADD_METHOD(mode_change);
#undef DISPACHER_ADD_METHOD
	}

	~NVim()
	{
		if (m_process) {
			m_process->kill();
			m_process->get_exit_status();
			m_process = nullptr;
		}
	}

	void Launch(const wchar_t *cmd)
	{
		auto callback = [this](const char *bytes, size_t n) {

			this->m_dispatcher.push_bytes(bytes, n);

		};
		m_process = std::shared_ptr<TinyProcessLib::Process>(new TinyProcessLib::Process(cmd, L"", callback, nullptr, true));

		msgpackpp::packer packer;
		packer.pack_array(4);
		packer.pack_integer(0);
		packer.pack_integer(1);
		packer.pack_str("nvim_ui_attach");
		packer.pack_array(3);
		packer.pack_integer(80);
		packer.pack_integer(56);
		packer.pack_map(0);

		// std::vector<std::uint8_t>
		auto p = packer.get_payload();
		m_process->write((const char*)p.data(), p.size());
	}

	bool GetExitStatus(int *_exit_status)
	{
		int exit_status;
		//std::this_thread::sleep_for(std::chrono::seconds(2));
		if (m_process->try_get_exit_status(exit_status)) {
			if (_exit_status)*_exit_status = exit_status;
			return true;
		}

		return false;
	}
};


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
