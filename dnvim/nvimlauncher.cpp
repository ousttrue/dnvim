#include "nvimlauncher.h"
#include "dispatcher.h"
#include <process.hpp>
#include <string>
#include <chrono>
#include <thread>
#include <msgpackpp.h>
#include <vector>
#include <functional>
#include <plog/Log.h>



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


class NVimImpl
{
	Dispatcher m_dispatcher;
	Grid m_grid;
	std::shared_ptr<TinyProcessLib::Process> m_process;

public:
	NVimImpl()
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

	~NVimImpl()
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


NVim::NVim()
	: m_impl(new NVimImpl)
{
}

NVim::~NVim()
{
	delete m_impl;
	m_impl = nullptr;
}

void NVim::Launch(const wchar_t *cmd)
{
	m_impl->Launch(cmd);
}

bool NVim::GetExitStatus(int *_exit_status)
{
	return m_impl->GetExitStatus(_exit_status);
}
