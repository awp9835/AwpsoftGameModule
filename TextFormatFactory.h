#pragma once
#include <windows.h>
#include <dwrite.h>
#include <atomic>
#pragma comment  (lib,"Dwrite.lib")
namespace AwpSoftGameModule
{
	
	class SingleFontCollection
	{
	private:
		friend class TextFormatFactory;
		class SingleFontFileEnumerator : public IDWriteFontFileEnumerator
		{
		private:
			unsigned int Reference;
			int CurrentPos;
			IDWriteFontFile* FontFile;
			~SingleFontFileEnumerator();
		public :
			SingleFontFileEnumerator(IDWriteFontFile* fontFile);
			virtual ULONG STDMETHODCALLTYPE Release(void);
			virtual ULONG STDMETHODCALLTYPE AddRef(void);
			virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject);
			virtual HRESULT STDMETHODCALLTYPE GetCurrentFontFile(IDWriteFontFile** fontFile);
			virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL* hasCurrentFile);
		};

		class SingleFontCollectionLoader : public IDWriteFontCollectionLoader
		{
		private:
			unsigned int Reference;
			IDWriteFontFile* FontFile;
			~SingleFontCollectionLoader();
		public:
			SingleFontCollectionLoader(IDWriteFontFile* fontFile);
			SingleFontCollectionLoader(IDWriteFactory* dWriteFactoryPtr, const wchar_t* path);
			virtual ULONG STDMETHODCALLTYPE Release(void);
			virtual ULONG STDMETHODCALLTYPE AddRef(void);
			virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject);
			virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(IDWriteFactory* factory, void const* collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator** fontFileEnumerator);
		};
		IDWriteFontCollection* FontCollection;
		wchar_t FontName[512];
		SingleFontCollection(IDWriteFactory* dWriteFactoryPtr, SingleFontCollectionLoader* loader);
		SingleFontCollection(IDWriteFactory* dWriteFactoryPtr, IDWriteFontFile* fontFile);
		SingleFontCollection(IDWriteFactory* dWriteFactoryPtr, const wchar_t* path);
		IDWriteFontCollection* getFontCollection();
		wchar_t* getFontName();
	public:
		~SingleFontCollection();
	};

	class TextFormatFactory
	{
	protected:
		IDWriteFactory* DWriteFactoryPtr;
	public:
		SingleFontCollection* createSingleFontCollection(const wchar_t* path);
		SingleFontCollection* createSingleFontCollection(IDWriteFontFile* fontFile);
		IDWriteTextFormat* createTextFormat(const wchar_t* systemFontName, float fontSize, DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH  fontStretch = DWRITE_FONT_STRETCH_NORMAL);
		IDWriteTextFormat* createTextFormat(SingleFontCollection* singleFont, float fontSize, DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH  fontStretch = DWRITE_FONT_STRETCH_NORMAL);
		IDWriteFactory* getInnerDWriteFactory();
		TextFormatFactory();
		virtual ~TextFormatFactory();
	};
};