#include "windowsutils.h"
#include "d3d/D3D11Manager.h"
#include <vector>


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


HWND CreateUIWindow(HINSTANCE hInstance, int nCmdShow, 
	const wchar_t *window_class, const wchar_t *window_title,
	void *userdata)
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
        wcex.lpszClassName = window_class;
        wcex.hIconSm = NULL;
        RegisterClassEx(&wcex);
    }
    HWND hWnd = CreateWindow(window_class, window_title, WS_OVERLAPPEDWINDOW
            , CW_USEDEFAULT, CW_USEDEFAULT
            , 320, 320
            , NULL, NULL, hInstance, userdata);
    if (!hWnd)
    {
        return nullptr;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return hWnd;
}


Resource::Resource(HINSTANCE hInst, int id, const wchar_t *resource)
        : m_hInst(hInst)
{
    m_hRes = FindResource(hInst, MAKEINTRESOURCE(id), resource);
    if (m_hRes) {
        m_hMem = LoadResource(hInst, m_hRes);
    }
}

Resource::~Resource()
{
    if (m_hMem) {
        FreeResource(m_hMem);
    }
}

std::string Resource::GetString()const
{
	if (!m_hMem) {
		return "";
	}
	auto resText = (char*)LockResource(m_hMem);
	auto size = SizeofResource(m_hInst, m_hRes);
	std::vector<char> buf(size);
	memcpy(buf.data(), resText, size);
	return std::string(buf.begin(), buf.end());
}
