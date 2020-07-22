#include "TextFormatFactory.h"
namespace AwpSoftGameModule
{
	SingleFontCollectionLoader::~SingleFontCollectionLoader()
	{
		if (FontFile) FontFile -> Release();
	}

	SingleFontCollectionLoader::SingleFontCollectionLoader(IDWriteFontFile * para)
	{
		FontFile = para;
		if (para) para->AddRef();
		FirstMove = TRUE;
		Reference = 1;
	}

	ULONG __stdcall SingleFontCollectionLoader::Release(void)
	{
		Reference--;
		if (!Reference) {
			delete this;
			return 0;
		}
		return Reference;
	}

	ULONG __stdcall SingleFontCollectionLoader::AddRef(void)
	{
		Reference++;
		return Reference;
	}

	HRESULT __stdcall SingleFontCollectionLoader::QueryInterface(REFIID riid, void ** ppvObject)
	{
		if (riid == __uuidof(IDWriteFontCollectionLoader))
		{
			*ppvObject = (IDWriteFontCollectionLoader*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == __uuidof(IDWriteFontFileEnumerator))
		{
			*ppvObject = (IDWriteFontFileEnumerator*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == __uuidof(IUnknown))
		{
			*ppvObject = (IUnknown*)(IDWriteFontCollectionLoader*)this;
			AddRef();
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT SingleFontCollectionLoader::CreateEnumeratorFromKey(IDWriteFactory * factory, void const * collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator ** fontFileEnumerator)
	{
		FirstMove = TRUE;
		*fontFileEnumerator = this;
		return S_OK;
	}

	HRESULT SingleFontCollectionLoader::GetCurrentFontFile(IDWriteFontFile ** fFile)
	{
		*fFile = FontFile;
		if (!FontFile) return E_FAIL;
		FontFile->AddRef();
		return S_OK;
	}

	HRESULT SingleFontCollectionLoader::MoveNext(BOOL * hasCurrentFile)
	{
		if (FirstMove)
		{
			*hasCurrentFile = TRUE;
			FirstMove = FALSE;
		}
		else
		{
			*hasCurrentFile = FALSE;
		}
		return S_OK;
	}


	SingleFontCollection* TextFormatFactory::CreateSingleFontCollection(const WCHAR * Path)
	{
		SingleFontCollectionLoader* Loader = new SingleFontCollectionLoader(pDWriteFactory, Path);
		return new SingleFontCollection(Loader, pDWriteFactory);
	}

	SingleFontCollection * TextFormatFactory::CreateSingleFontCollection(IDWriteFontFile * FontFile)
	{
		SingleFontCollectionLoader* Loader = new SingleFontCollectionLoader(FontFile);
		return new SingleFontCollection(Loader, pDWriteFactory);
	}

	IDWriteTextFormat * TextFormatFactory::CreateTextFormat(const WCHAR * FontName, FLOAT FontSize, DWRITE_FONT_WEIGHT FontWeight, DWRITE_FONT_STYLE FontStyle, DWRITE_FONT_STRETCH FontStretch)
	{
		IDWriteTextFormat *pTextFormat = NULL;
		HRESULT hr;
		hr = pDWriteFactory->CreateTextFormat(FontName, NULL, FontWeight, FontStyle, FontStretch, FontSize, L"zh-cn", &pTextFormat);
		if (SUCCEEDED(hr))return pTextFormat;
		hr = pDWriteFactory->CreateTextFormat(FontName, NULL, FontWeight, FontStyle, FontStretch, FontSize, L"en-us", &pTextFormat);
		if (SUCCEEDED(hr))return pTextFormat;
		hr = pDWriteFactory->CreateTextFormat(FontName, NULL, FontWeight, FontStyle, FontStretch, FontSize, LOCALE_NAME_INVARIANT, &pTextFormat);
		if (SUCCEEDED(hr))return pTextFormat;
		hr = pDWriteFactory->CreateTextFormat(FontName, NULL, FontWeight, FontStyle, FontStretch, FontSize, LOCALE_NAME_USER_DEFAULT, &pTextFormat);
		if (SUCCEEDED(hr))return pTextFormat;
		hr = pDWriteFactory->CreateTextFormat(FontName, NULL, FontWeight, FontStyle, FontStretch, FontSize, LOCALE_NAME_SYSTEM_DEFAULT, &pTextFormat);
		return pTextFormat;
	}

	IDWriteTextFormat * TextFormatFactory::CreateTextFormat(SingleFontCollection * SingleFont, FLOAT FontSize, DWRITE_FONT_WEIGHT FontWeight, DWRITE_FONT_STYLE FontStyle, DWRITE_FONT_STRETCH FontStretch)
	{
		IDWriteTextFormat *pTextFormat = NULL;
		HRESULT hr;
		hr = pDWriteFactory->CreateTextFormat(SingleFont->GetFontName(), SingleFont->GetFontCollection(), FontWeight, FontStyle, FontStretch, FontSize, L"zh-cn", &pTextFormat);
		if (SUCCEEDED(hr))return pTextFormat;
		hr = pDWriteFactory->CreateTextFormat(SingleFont->GetFontName(), SingleFont->GetFontCollection(), FontWeight, FontStyle, FontStretch, FontSize, L"en-us", &pTextFormat);
		if (SUCCEEDED(hr))return pTextFormat;
		hr = pDWriteFactory->CreateTextFormat(SingleFont->GetFontName(), SingleFont->GetFontCollection(), FontWeight, FontStyle, FontStretch, FontSize, LOCALE_NAME_INVARIANT, &pTextFormat);
		if (SUCCEEDED(hr))return pTextFormat;
		hr = pDWriteFactory->CreateTextFormat(SingleFont->GetFontName(), SingleFont->GetFontCollection(), FontWeight, FontStyle, FontStretch, FontSize, LOCALE_NAME_USER_DEFAULT, &pTextFormat);
		if (SUCCEEDED(hr))return pTextFormat;
		hr = pDWriteFactory->CreateTextFormat(SingleFont->GetFontName(), SingleFont->GetFontCollection(), FontWeight, FontStyle, FontStretch, FontSize, LOCALE_NAME_SYSTEM_DEFAULT, &pTextFormat);
		return pTextFormat;
	}

	IDWriteFactory * TextFormatFactory::GetWriteFactory()
	{
		return pDWriteFactory;
	}

	TextFormatFactory::TextFormatFactory()
	{
		pDWriteFactory = NULL;
		HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)(&pDWriteFactory));
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Create DWrite Factor Failed!", L"Fatal Error!", MB_ICONERROR);
			exit(0);
		}
	}

	TextFormatFactory::~TextFormatFactory()
	{
		if (pDWriteFactory) pDWriteFactory->Release();
		pDWriteFactory = NULL;
	}

	SingleFontCollectionLoader::SingleFontCollectionLoader(IDWriteFactory* pDWriteFactory, const WCHAR * Path)
	{
		HRESULT hr;
		hr = pDWriteFactory->CreateFontFileReference(Path, NULL, &FontFile);
		if (FAILED(hr)) FontFile = NULL;
		FirstMove = TRUE;
		Reference = 1;
	}

	SingleFontCollection::SingleFontCollection(SingleFontCollectionLoader *Loader, IDWriteFactory *pDWriteFactory)
	{
		HRESULT hr;
		FontCollection = NULL;
		FontName[0] = L'\0';
		hr = pDWriteFactory->RegisterFontCollectionLoader(Loader);
		if (FAILED(hr))return;
		hr = pDWriteFactory->CreateCustomFontCollection(Loader, NULL, 0, &FontCollection);
		pDWriteFactory->UnregisterFontCollectionLoader(Loader);
		if (FAILED(hr))return;
		IDWriteFontFamily *pFontFamily = NULL;
		IDWriteLocalizedStrings* pFamilyNames = NULL;
		hr = FontCollection->GetFontFamily(0, &pFontFamily);
		if (FAILED(hr)) return;
		hr = pFontFamily->GetFamilyNames(&pFamilyNames);
		if (FAILED(hr))
		{
			pFontFamily->Release();
			return;
		}
		pFamilyNames->GetString(0, FontName, 256);
		pFamilyNames->Release();
		pFontFamily->Release();
	}

	SingleFontCollection ::~SingleFontCollection()
	{
		if (FontCollection) FontCollection->Release();
		FontCollection = NULL;
	}

	IDWriteFontCollection * SingleFontCollection::GetFontCollection()
	{
		return FontCollection;
	}

	WCHAR * SingleFontCollection::GetFontName()
	{
		return FontName;
	}
};