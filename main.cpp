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
		auto msg = msgpackpp::parser(m_buffer.data(), m_buffer.size());
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


#define LAUNCH_COMMAND "nvim --embed"


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

	LOGI << "dnvim";

	Dispatcher dispatcher;
	Grid grid;

#define DISPATCHER_ADD_METHOD(method) dispatcher.add_method(#method, msgpackpp::make_methodcall(&grid, &Grid::method))
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

	TinyProcessLib::Process process1(LAUNCH_COMMAND, "", [&dispatcher](const char *bytes, size_t n) {

		dispatcher.push_bytes(bytes, n);

	}, nullptr, true);

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
	process1.write((const char*)p.data(), p.size());

	std::this_thread::sleep_for(std::chrono::seconds(2));
	auto exit_status = process1.get_exit_status();

	return exit_status;
}
