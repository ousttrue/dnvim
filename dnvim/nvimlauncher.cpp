#include "nvimlauncher.h"
#include "dispatcher.h"
#include "grid.h"
#include <process.hpp>
#include <plog/Log.h>


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

	int m_requestID = 1;

	template<typename ... AS>
	void Call(const char *method, AS... args)
	{
		msgpackpp::packer packer;

		auto tuple = std::make_tuple(args...);
	
		packer.pack_array(4)
			<< 0
			<< m_requestID++
			<< method
			<< tuple
			;

		auto p = packer.get_payload();
		m_process->write((const char*)p.data(), p.size());
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
		packer.pack_integer(m_requestID++);
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

	void Input(const std::string &keys) {
		Call("nvim_ui_attach", keys);
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

void NVim::Input(const std::string &keys)
{
	m_impl->Input(keys);
}
