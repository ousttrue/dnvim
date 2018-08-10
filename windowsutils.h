#include <string>
#include <Windows.h>


HWND CreateUIWindow(HINSTANCE hInstance, int nCmdShow,
	const wchar_t *window_class, const wchar_t *window_title,
	void *userdata);


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
