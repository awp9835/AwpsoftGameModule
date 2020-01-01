#pragma once
#include <windows.h>
#include <dwrite.h>
#pragma comment  (lib,"Dwrite.lib")
class SingleFontCollectionLoader:public IDWriteFontCollectionLoader, public IDWriteFontFileEnumerator
{
	
private:
	IDWriteFontFile *FontFile;
	volatile ULONG Reference;
	BOOL FirstMove;
	~SingleFontCollectionLoader(); 
public:
	SingleFontCollectionLoader(IDWriteFontFile *FontFile); //Disposable Loader
	SingleFontCollectionLoader(IDWriteFactory* pDWriteFactory, const WCHAR * Path); //Disposable Loader
	virtual ULONG STDMETHODCALLTYPE Release(void); //Auto Destory in Unregister.
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);
	virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(IDWriteFactory *factory, void const *collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator **fontFileEnumerator);
	virtual HRESULT STDMETHODCALLTYPE GetCurrentFontFile(IDWriteFontFile **fFile);
	virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL *hasCurrentFile);
};
class SingleFontCollection
{
private :
	IDWriteFontCollection* FontCollection;
	WCHAR FontName[256];
public: 
	SingleFontCollection(SingleFontCollectionLoader* Loader, IDWriteFactory  *pDWriteFactory);
	~SingleFontCollection ();
	IDWriteFontCollection* GetFontCollection();
	WCHAR* GetFontName();
};

class TextFormatFactory
{
protected:
	IDWriteFactory* pDWriteFactory;
public:
	SingleFontCollection* CreateSingleFontCollection(const WCHAR* Path);
	SingleFontCollection* CreateSingleFontCollection(IDWriteFontFile *FontFile);
	IDWriteTextFormat* CreateTextFormat(const WCHAR* SystemFontName, FLOAT FontSize, DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE FontStyle = DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH  FontStretch = DWRITE_FONT_STRETCH_NORMAL);
	IDWriteTextFormat* CreateTextFormat(SingleFontCollection* SingleFont, FLOAT FontSize, DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE FontStyle = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH  FontStretch = DWRITE_FONT_STRETCH_NORMAL);
	IDWriteFactory* GetWriteFactory();
	TextFormatFactory();
	virtual ~TextFormatFactory();
};

