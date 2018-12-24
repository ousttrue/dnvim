#include <string>
#include <memory>
#include <wrl/client.h>
#include <d3d11.h>


class D3D11Manager
{
	Microsoft::WRL::ComPtr<struct IDXGISwapChain> m_pSwapChain;
	Microsoft::WRL::ComPtr<struct ID3D11Texture2D> m_backbuffer;
	Microsoft::WRL::ComPtr<struct ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<struct ID3D11DeviceContext> m_pDeviceContext;

	std::shared_ptr<class RenderTarget> m_renderTarget;
	std::shared_ptr<class Shader> m_shader;

public:
	D3D11Manager();
	~D3D11Manager();
	bool Initialize(HWND hWnd
		, const std::string &shaderSource, const std::wstring &textureFile);
	Microsoft::WRL::ComPtr<struct ID3D11Device> GetDevice()const { return m_pDevice; }
	Microsoft::WRL::ComPtr<struct ID3D11Texture2D> GetBackBuffer();
	void Resize(int w, int h);
	void BeginRender();
	void Render();
	void EndRender();
};
