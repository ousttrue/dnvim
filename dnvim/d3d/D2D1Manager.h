#pragma once
#include <wrl/client.h>


class D2D1Manager
{
	Microsoft::WRL::ComPtr<struct ID2D1Factory1> m_d2d1Factory;
	Microsoft::WRL::ComPtr<struct ID2D1Device> m_d2dDevice;
	Microsoft::WRL::ComPtr<struct ID2D1DeviceContext> m_d2dDeviceContext;

	Microsoft::WRL::ComPtr<struct ID3D11Texture2D> m_target;
	Microsoft::WRL::ComPtr<struct ID2D1Bitmap1> m_renderTarget;

	// scene
	//Microsoft::WRL::ComPtr<struct ID2D1Bitmap> m_src;
	Microsoft::WRL::ComPtr<struct ID2D1SolidColorBrush> m_brush;

public:
	D2D1Manager();
	~D2D1Manager();
	bool Initialize(const Microsoft::WRL::ComPtr<struct ID3D11Device> &device);
	void SetTargetTexture(const Microsoft::WRL::ComPtr<struct ID3D11Texture2D> &texture);
	void Render(const class Grid *grid);
	//void ReleaseDevice();
};
