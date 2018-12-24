#pragma once
#include <string>


static std::wstring GetString(const Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> &str)
{
	UINT32 length;
	auto hr = str->GetStringLength(0, &length);
	if (FAILED(hr)) {
		return L"";
	}

	std::vector<WCHAR> buffer(length + 1);
	hr = str->GetString(0, buffer.data(), buffer.size());
	if (FAILED(hr)) {
		return L"";
	}
	buffer.pop_back();

	return std::wstring(buffer.begin(), buffer.end());
}

static Microsoft::WRL::ComPtr<IDWriteFontFace> GetFontFace(
	const std::wstring &targetFamily,
	const std::wstring &targetFace)
{
	Microsoft::WRL::ComPtr<IDWriteFactory> dwf;
	auto hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(dwf.GetAddressOf())
	);
	if (FAILED(hr)) {
		return nullptr;
	}

	Microsoft::WRL::ComPtr<IDWriteFontCollection> collection;
	hr = dwf->GetSystemFontCollection(&collection);
	if (FAILED(hr)) {
		return nullptr;
	}

	auto familyCount = collection->GetFontFamilyCount();
	for (int i = 0; i < familyCount; ++i) {
		Microsoft::WRL::ComPtr<IDWriteFontFamily> family;
		hr = collection->GetFontFamily(i, &family);
		if (FAILED(hr)) {
			return nullptr;
		}

		Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> pFamilyName;
		hr = family->GetFamilyNames(&pFamilyName);
		if (FAILED(hr)) {
			continue;
		}

		auto familyName = GetString(pFamilyName);
		LOGD << "familyName: " << familyName;

		if (familyName == targetFamily) {

			auto fontCount = family->GetFontCount();
			for (int j = 0; j < fontCount; ++j) {
				Microsoft::WRL::ComPtr<IDWriteFont> font;
				hr = family->GetFont(j, &font);
				if (FAILED(hr)) {
					return nullptr;
				}

				Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> pFaceName;
				hr = font->GetFaceNames(&pFaceName);
				if (FAILED(hr)) {
					return nullptr;
				}

				auto faceName = GetString(pFaceName);
				LOGD << "faceName: " << faceName;

				if (faceName == targetFace)
				{
					Microsoft::WRL::ComPtr<IDWriteFontFace> face;
					hr = font->CreateFontFace(&face);
					if (FAILED(hr)) {
						return nullptr;
					}
					return face;
				}
			}
		}
	}

	return nullptr;
}
