#include "D2D1Manager.h"
#include <d3d11.h>
#include <d2d1_2.h>
#include <dxgi1_2.h>
#include "../grid.h"
#include "plog/Log.h"


class GridDrawer
{
	Microsoft::WRL::ComPtr<struct ID2D1SolidColorBrush> m_cellBursh;
	Microsoft::WRL::ComPtr<struct ID2D1SolidColorBrush> m_cursorBrush;
	int m_cellWidth = 8;
	int m_cellHeight = 16;
	int m_lineMergin = 2;

	class DrawGuard
	{
		Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_context;

	public:
		DrawGuard(const Microsoft::WRL::ComPtr<ID2D1DeviceContext> &context
			, const Microsoft::WRL::ComPtr<ID2D1Bitmap1> &bitmap)
			: m_context(context)
		{
			m_context->SetTarget(bitmap.Get());
			m_context->BeginDraw();
		}

		~DrawGuard()
		{
			m_context->EndDraw();
		}
	};
public:
	void Draw(
		const Microsoft::WRL::ComPtr<ID2D1DeviceContext> &context,
		const Microsoft::WRL::ComPtr<ID2D1Bitmap1> &bitmap,
		const Cell*cell, int cellCount, int cols, int cursorY, int cursorX)
	{
		if (!m_cellBursh) {
			auto hr = context->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF(0x9ACD32, 1.0f))
				, &m_cellBursh);
			if (FAILED(hr)) {
				return;
			}
		}
		if (!m_cursorBrush) {
			auto hr = context->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF(0x9A32CD, 1.0f))
				, &m_cursorBrush);
			if (FAILED(hr)) {
				return;
			}
		}

		{
			DrawGuard guard(context, bitmap);

			context->SetTransform(D2D1::Matrix3x2F::Identity());
			context->Clear(D2D1::ColorF(D2D1::ColorF::White));

			auto rows = cellCount / cols;
			int y = 0;
			for (auto row = 0; row < rows; ++row, y+=(m_cellHeight + m_lineMergin)) {
				int x = 0;
				for (auto col = 0; col < cols; ++col, x+=(m_cellWidth)) {

					D2D1_RECT_F rect = D2D1::RectF(
						x,
						y,
						x + m_cellWidth,
						y + m_cellHeight
					);

					if (row == cursorY && col == cursorX) {
						context->FillRectangle(&rect, m_cursorBrush.Get());
					}
					else {
						context->FillRectangle(&rect, m_cellBursh.Get());
					}
				}
			}

			/*
			D2D1_SIZE_F rtSize = m_d2dDeviceContext->GetSize();
			D2D1_RECT_F rect = D2D1::RectF(
				rtSize.width / 2 - 50.0f,
				rtSize.height / 2 - 50.0f,
				rtSize.width / 2 + 50.0f,
				rtSize.height / 2 + 50.0f
			);
			*/
		}
	}
};


static void HrToStr(HRESULT hr) {
	LPVOID string;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&string,
		0,
		NULL);
	if (string != NULL)
		OutputDebugString((LPCWSTR)string);
	LocalFree(string);
}


D2D1Manager::D2D1Manager()
	: m_drawer(new GridDrawer())
{
}

D2D1Manager::~D2D1Manager()
{
	delete m_drawer;
}

bool D2D1Manager::Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &device)
{
	auto hr = S_OK;

	D2D1_FACTORY_OPTIONS options = {};
#ifdef _DEBUG
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory1),
		&options,
		&m_d2d1Factory))) {
		return false;
	}

	Microsoft::WRL::ComPtr<IDXGIDevice2> dxgiDevice;
	if (FAILED(device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)))) {
		return false;
	}

	hr = m_d2d1Factory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice);
	if (FAILED(hr)) {
		return false;
	}

	if (FAILED(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dDeviceContext))) {
		return false;
	}

	return true;
}


void D2D1Manager::SetTargetTexture(const Microsoft::WRL::ComPtr<ID3D11Texture2D> &texture)
{
	if (m_target == texture) {
		return;
	}
	m_target = texture;

	// clear
	if (m_d2dDeviceContext) {
		m_d2dDeviceContext->SetTarget(nullptr);
	}
	m_bitmap.Reset();
}


void D2D1Manager::Render(const Cell*cell, int cellCount, int cols, int cursorY, int cursorX)
{
	if (!m_bitmap) {
		if (!m_target) {
			return;
		}

		Microsoft::WRL::ComPtr<IDXGISurface2> dxgiSurface;
		if (FAILED(m_target.As(&dxgiSurface))) {
			return;
		}
		const auto bp = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
		);

		auto hr = m_d2dDeviceContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bp, &m_bitmap);
		if (FAILED(hr)) {
			return;
		}
		LOGI << "d2d rendertarget set";
	}

	m_drawer->Draw(m_d2dDeviceContext, m_bitmap,
		cell, cellCount, cols, cursorY, cursorX);
}
