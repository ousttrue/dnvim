#pragma once
#include <wrl/client.h>


class D2D1Manager
{
	Microsoft::WRL::ComPtr<struct ID2D1Factory1> m_d2d1Factory;
	Microsoft::WRL::ComPtr<struct ID2D1Device> m_d2dDevice;
	Microsoft::WRL::ComPtr<struct ID2D1DeviceContext> m_d2dDeviceContext;

	Microsoft::WRL::ComPtr<struct ID3D11Texture2D> m_target;
	Microsoft::WRL::ComPtr<struct ID2D1Bitmap1> m_bitmap;

	class GridDrawer *m_drawer = nullptr;

public:
	D2D1Manager();
	~D2D1Manager();
	bool Initialize(const Microsoft::WRL::ComPtr<struct ID3D11Device> &device);
	void SetTargetTexture(const Microsoft::WRL::ComPtr<struct ID3D11Texture2D> &texture);
	void Render(const struct Cell *cell, int cellCount, int cols, int cursorY, int cursorX);
};
