#include "D2D1Manager.h"
#include <d3d11.h>
#include <d2d1_2.h>
#include <dxgi1_2.h>
#include "../grid.h"
#include "plog/Log.h"


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
{
}

D2D1Manager::~D2D1Manager()
{
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

	//ƒuƒ‰ƒV‚Ìì¬
	hr = m_d2dDeviceContext->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF(0x9ACD32, 1.0f))
		, &m_brush);
	if (FAILED(hr)) {
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
	m_renderTarget.Reset();
}


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


void D2D1Manager::Render(const Grid *grid)
{
	if (!m_renderTarget) {
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

		auto hr = m_d2dDeviceContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bp, &m_renderTarget);
		if (FAILED(hr)) {
			return;
		}
		LOGI << "d2d rendertarget set";
	}

	{
		DrawGuard guard(m_d2dDeviceContext, m_renderTarget);

		m_d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
		m_d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::White));

		/*
		if (m_src) {
			m_d2dDeviceContext->DrawBitmap(m_src.Get());
		}
		*/

		D2D1_SIZE_F rtSize = m_d2dDeviceContext->GetSize();
		D2D1_RECT_F rect = D2D1::RectF(
			rtSize.width / 2 - 50.0f,
			rtSize.height / 2 - 50.0f,
			rtSize.width / 2 + 50.0f,
			rtSize.height / 2 + 50.0f
		);
		m_d2dDeviceContext->FillRectangle(&rect, m_brush.Get());
	}
}
