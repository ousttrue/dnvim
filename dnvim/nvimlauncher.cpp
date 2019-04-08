#include "nvimlauncher.h"
#include "dispatcher.h"
#include "grid.h"
#include <process.hpp>
#include <plog/Log.h>
#include "windowsutils.h"
#include "d3d/D3D11Manager.h"
#include "d3d/resource.h"
#include "d3d/D2D1Manager.h"


struct Option
{
	bool rgb = true;
};
MPPP_MAP_SERIALIZER(Option, 1, rgb);


class NVimImpl
{
	Dispatcher m_dispatcher;
	Grid m_grid;
	std::shared_ptr<TinyProcessLib::Process> m_process;
	std::shared_ptr<D3D11Manager> m_d3d;
	std::shared_ptr<D2D1Manager> m_d2d;

public:
	NVimImpl()
		: m_d3d(new D3D11Manager)
		, m_d2d(new D2D1Manager)	
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
		DISPATCHER_ADD_METHOD(flush);
		DISPATCHER_ADD_METHOD(eol_clear);
#undef DISPACHER_ADD_METHOD
	}

	~NVimImpl()
	{
		auto process = m_process;
		if (process) {
			process->kill();
			process->get_exit_status();
			m_process = nullptr;
		}
	}

	int m_requestID = 1;

	template<typename ... AS>
	void Call(const char *method, AS... args)
	{
		auto p = msgpackpp::make_rpc_request(m_requestID++, method, args...);
		m_process->write((const char*)p.data(), p.size());
	}

	void Launch(const wchar_t *cmd)
	{
		auto d3d = m_d3d;
		auto d2d = m_d2d;
		auto grid = &m_grid;
		auto callback = [this, d3d, d2d, grid]( const char *bytes, size_t n) {

			this->m_dispatcher.push_bytes(bytes, n);

			if (grid->use_flush()) 
			{
				d2d->SetTargetTexture(d3d->GetBackBuffer());
				auto& cells = grid->get_cells();
				auto& cursor = grid->get_cursor();
				d2d->Render(cells.data(), (int)cells.size(), grid->get_cols(), cursor.row, cursor.col);
				d3d->EndRender();
			}

		};
		m_process = std::shared_ptr<TinyProcessLib::Process>(new TinyProcessLib::Process(cmd, L"", callback, nullptr, true));

		Option option;
		Call("nvim_ui_attach", 80, 20, option);
	}

	bool GetExitStatus(int *_exit_status)
	{
		auto process = m_process;
		if (!process) {
			return 0;
		}

		//std::this_thread::sleep_for(std::chrono::seconds(2));
		int exit_status;
		if (process->try_get_exit_status(exit_status)) {
			if (_exit_status)*_exit_status = exit_status;
			m_process = nullptr;
			return true;
		}

		return false;
	}

	void Input(const std::string &keys) {
		LOGD << "Input: '" << keys << "'";
		Call("nvim_input", keys);
	}

	bool Initialize(HINSTANCE hInstance, HWND hWnd, const WCHAR *shaderResource)
	{
		Resource res(hInstance, ID_SHADERSOURCE, shaderResource);
		auto shaderSource = res.GetString();
		if (shaderSource.empty()) {
			LOGE << "fail to get shader resource";
			return false;
		}

		LOGI << "d3d initialize";
		if (!m_d3d->Initialize(hWnd, shaderSource, L"")) {
			LOGE << "fail to initilaize d3d";
			return false;
		}

		LOGI << "d2d initialize";
		if (!m_d2d->Initialize(m_d3d->GetDevice()))
		{
			LOGE << "fail to initilaize d2d";
			return false;
		}

		return true;
	}

	void Resize(int w, int h)
	{
		m_d2d->SetTargetTexture(nullptr);
		m_d3d->Resize(w, h);
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

bool NVim::Initialize(HINSTANCE hInstance, HWND hWnd, const WCHAR *shaderResource)
{
	return m_impl->Initialize(hInstance, hWnd, shaderResource);
}

void NVim::Resize(int w, int h)
{
	m_impl->Resize(w, h);
}
