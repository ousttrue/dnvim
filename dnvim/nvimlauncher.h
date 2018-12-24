#include <string>
#include <Windows.h>


class NVimImpl;
class NVim
{
    NVimImpl *m_impl;

public:
	NVim();
	~NVim();
    void Launch(const wchar_t *cmd);
    bool GetExitStatus(int *_exit_status);
	void Input(const std::string &keys);
	bool Initialize(HINSTANCE hInstance, HWND hWnd, const WCHAR *shaderResource);
	void Resize(int w, int h);
};
