#include <Windows.h>
#include <string>
#include <memory>
#include <functional>
#include <list>


class UIWindow
{
	HWND m_hWnd = nullptr;
	typedef std::function<void(int, int)> OnSizeFunc;
	std::list<OnSizeFunc> m_onSizeFunc;

public:
	static std::shared_ptr<UIWindow> Create(HINSTANCE hInstance, int nCmdShow,
		const wchar_t *window_class, const wchar_t *window_title);

	void Attach(HWND hWnd) { m_hWnd = hWnd; }
	HWND Get()const { return m_hWnd; }

	void AddOnSize(const OnSizeFunc &func)
	{
		m_onSizeFunc.push_back(func);
	}
	void OnSize(int w, int h)
	{
		for (auto &func : m_onSizeFunc)func(w, h);
	}
};


class Resource
{
    HINSTANCE m_hInst;
    HRSRC m_hRes;
    HGLOBAL m_hMem;
public:

    Resource(HINSTANCE hInst, int id, const wchar_t *resource);
    ~Resource();
    std::string GetString()const;
};

