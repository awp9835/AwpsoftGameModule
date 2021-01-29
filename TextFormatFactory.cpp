#include "TextFormatFactory.h"
namespace AwpSoftGameModule
{

	SingleFontCollection::SingleFontFileEnumerator::~SingleFontFileEnumerator()
	{
		if (FontFile) FontFile->Release();
	}

	SingleFontCollection::SingleFontFileEnumerator::SingleFontFileEnumerator(IDWriteFontFile* fontFile)
	{
		FontFile = fontFile;
		if (fontFile) fontFile->AddRef();
		Reference = 1;
		CurrentPos = -1;
	}
	ULONG STDMETHODCALLTYPE SingleFontCollection::SingleFontFileEnumerator::Release(void)
	{
		Reference--;
		if (Reference == 0)
		{
			delete this;
			return 0;
		}
		return Reference;
	}
	ULONG STDMETHODCALLTYPE SingleFontCollection::SingleFontFileEnumerator::AddRef(void)
	{
		Reference++;
		return Reference;
	}
	HRESULT STDMETHODCALLTYPE SingleFontCollection::SingleFontFileEnumerator::QueryInterface(REFIID riid, void** ppvObject)
	{
		if (!ppvObject) return E_INVALIDARG;
		if (riid == __uuidof(IDWriteFontFileEnumerator))
		{
			*ppvObject = (IDWriteFontFileEnumerator*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == __uuidof(IUnknown))
		{
			*ppvObject = (IUnknown*)(IDWriteFontFileEnumerator*)this;
			AddRef();
			return S_OK;
		}
		else return E_NOTIMPL;
	}

	HRESULT SingleFontCollection::SingleFontFileEnumerator::GetCurrentFontFile(IDWriteFontFile** fontFile)
	{
		if (!fontFile) return E_INVALIDARG;
		if (!FontFile) return E_NOT_VALID_STATE;
		*fontFile = FontFile;
		FontFile->AddRef();
		return S_OK;
	}

	HRESULT SingleFontCollection::SingleFontFileEnumerator::MoveNext(BOOL* hasCurrentFile)
	{
		if (!hasCurrentFile) return E_INVALIDARG;
		CurrentPos++;
		if (CurrentPos == 0)
		{
			*hasCurrentFile = TRUE;
		}
		else
		{
			CurrentPos = 1;
			*hasCurrentFile = FALSE;
		}
		return S_OK;
	}

	SingleFontCollection::SingleFontCollectionLoader::~SingleFontCollectionLoader()
	{
		if (FontFile) FontFile -> Release();
	}

	SingleFontCollection::SingleFontCollectionLoader::SingleFontCollectionLoader(IDWriteFontFile * fontFile)
	{
		FontFile = fontFile;
		if (fontFile) fontFile->AddRef();
		Reference = 1;
	}

	ULONG STDMETHODCALLTYPE SingleFontCollection::SingleFontCollectionLoader::Release(void)
	{
		Reference--;
		if (Reference == 0) 
		{
			delete this;
			return 0;
		}
		return Reference;
	}

	ULONG STDMETHODCALLTYPE SingleFontCollection::SingleFontCollectionLoader::AddRef(void)
	{
		Reference++;
		return Reference;
	}

	HRESULT STDMETHODCALLTYPE SingleFontCollection::SingleFontCollectionLoader::QueryInterface(REFIID riid, void ** ppvObject)
	{
		if (!ppvObject) return E_INVALIDARG;
		if (riid == __uuidof(IDWriteFontCollectionLoader))
		{
			*ppvObject = (IDWriteFontCollectionLoader*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == __uuidof(IUnknown))
		{
			*ppvObject = (IUnknown*)(IDWriteFontCollectionLoader*)this;
			AddRef();
			return S_OK;
		}
		else return E_NOTIMPL;
	}

	HRESULT SingleFontCollection::SingleFontCollectionLoader::CreateEnumeratorFromKey(IDWriteFactory *factory, void const *collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator ** fontFileEnumerator)
	{
		if (!factory || !fontFileEnumerator || !collectionKey) return E_INVALIDARG;
		if (collectionKeySize != 17) return E_FAIL;
		if (memcmp(collectionKey, "AWPSOFTGAMEMODULE", 17)) return E_FAIL;
		*fontFileEnumerator = new SingleFontFileEnumerator(FontFile);
		return S_OK;
	}


	SingleFontCollection* TextFormatFactory::createSingleFontCollection(const wchar_t* path)
	{
		SingleFontCollection::SingleFontCollectionLoader* loader = new SingleFontCollection::SingleFontCollectionLoader(DWriteFactoryPtr, path);
		SingleFontCollection* result = new SingleFontCollection(DWriteFactoryPtr, loader);
		loader->Release();
		return result;
	}

	SingleFontCollection* TextFormatFactory::createSingleFontCollection(IDWriteFontFile* fontFile)
	{
		SingleFontCollection::SingleFontCollectionLoader* loader = new SingleFontCollection::SingleFontCollectionLoader(fontFile);
		SingleFontCollection* result = new SingleFontCollection(DWriteFactoryPtr, loader);
		loader->Release();
		return result;
	}
	const wchar_t* MainLocalNames[15] =
	{
		LOCALE_NAME_USER_DEFAULT,LOCALE_NAME_SYSTEM_DEFAULT,LOCALE_NAME_INVARIANT,
		L"zh-cn",L"en-us", L"en-gb",L"zh-hk",L"zh-tw",
		L"fr-fr",L"es-es", L"de-de",L"ja-jp",L"ko-kr",
		L"ru-ru",L"ar-sa"
	};
	IDWriteTextFormat * TextFormatFactory::createTextFormat(const wchar_t * fontName, float fontSize, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch)
	{
		IDWriteTextFormat *textFormatPtr = nullptr;
		HRESULT hr;
		for(int i = 0; i < 15; i++)
		{
			hr = DWriteFactoryPtr->CreateTextFormat(fontName, nullptr, fontWeight, fontStyle, fontStretch, fontSize, MainLocalNames[i], &textFormatPtr);
			if (SUCCEEDED(hr))return textFormatPtr;
		}
		return textFormatPtr;
	}

	IDWriteTextFormat* TextFormatFactory::createTextFormat(SingleFontCollection * singleFontCollection, float fontSize, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch)
	{
		IDWriteTextFormat* textFormatPtr = nullptr;
		HRESULT hr;
		for (int i = 0; i < 15; i++)
		{
			hr = DWriteFactoryPtr->CreateTextFormat(singleFontCollection->getFontName(), singleFontCollection->getFontCollection(), 
				fontWeight, fontStyle, fontStretch, fontSize, MainLocalNames[i], &textFormatPtr);
			if (SUCCEEDED(hr))return textFormatPtr;
		}
		return textFormatPtr;
	}

	IDWriteFactory * TextFormatFactory::getInnerDWriteFactory()
	{
		return DWriteFactoryPtr;
	}

	TextFormatFactory::TextFormatFactory()
	{
		DWriteFactoryPtr = nullptr;
		HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)(&DWriteFactoryPtr));
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"Create DWrite Factor Failed!", L"Fatal Error!", MB_ICONERROR);
			exit(0);
		}
	}

	TextFormatFactory::~TextFormatFactory()
	{
		if (DWriteFactoryPtr) DWriteFactoryPtr->Release();
		DWriteFactoryPtr = nullptr;
	}

	SingleFontCollection::SingleFontCollectionLoader::SingleFontCollectionLoader(IDWriteFactory* dWriteFactoryPtr, const wchar_t * path)
	{
		HRESULT hr = dWriteFactoryPtr->CreateFontFileReference(path, nullptr, &FontFile);
		if (FAILED(hr)) FontFile = nullptr;
		Reference = 1;
	}

	SingleFontCollection::SingleFontCollection(IDWriteFactory *dWriteFactoryPtr, SingleFontCollectionLoader* loader)
	{
		HRESULT hr;
		FontCollection = nullptr;
		memset(&FontName[0], 0, sizeof(wchar_t) * 256);
		hr = dWriteFactoryPtr->RegisterFontCollectionLoader(loader);
		if (FAILED(hr))return;
		hr = dWriteFactoryPtr->CreateCustomFontCollection(loader, "AWPSOFTGAMEMODULE", 17, &FontCollection);
		dWriteFactoryPtr->UnregisterFontCollectionLoader(loader);
		if (FAILED(hr))return;
		IDWriteFontFamily *fontFamilyPtr = nullptr;
		IDWriteLocalizedStrings* familyNamesPtr = nullptr;
		hr = FontCollection->GetFontFamily(0, &fontFamilyPtr);
		if (FAILED(hr)) return;
		hr = fontFamilyPtr->GetFamilyNames(&familyNamesPtr);
		if (FAILED(hr))
		{
			fontFamilyPtr->Release();
			return;
		}
		familyNamesPtr->GetString(0, FontName, 512);
		familyNamesPtr->Release();
		fontFamilyPtr->Release();
	}

	SingleFontCollection ::~SingleFontCollection()
	{
		if (FontCollection) FontCollection->Release();
		FontCollection = nullptr;
	}

	IDWriteFontCollection * SingleFontCollection::getFontCollection()
	{
		return FontCollection;
	}

	wchar_t * SingleFontCollection::getFontName()
	{
		return FontName;
	}
};