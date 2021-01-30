#pragma once
#include <d2d1.h>
#include <dshow.h>
#include <shlwapi.h>
#include <wincodec.h>
#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")
#pragma comment  (lib,"strmiids.lib")
#pragma comment  (lib,"Dxguid.lib")
#pragma comment  (lib, "Windowscodecs.lib" )
#pragma comment  (lib, "shlwapi.lib" )
#pragma comment  (lib, "d2d1.lib" )
#include <vector>
#include <atomic>
namespace AwpSoftGameModule
{
	struct DrawParametersD2D1
	{
		bool Visible;
		float PosCenterX, PosCenterY, PicCenterX, PicCenterY, RotationDEG, SecondaryAlpha, WScale, HScale;
		ID2D1Bitmap* Image;//必须用对应的RenderTarget绘制
		DrawParametersD2D1();
	};
	struct TextParametersD2D1
	{
		bool Visible;
		int StrLength;
		float SecondaryAlpha, XLeft, XRight, YTop;
		wchar_t* StrBuffer;
		IDWriteTextFormat* TextFormat;
		D2D_COLOR_F ColorF;
		TextParametersD2D1();
	};
	class D2D1Created
	{
	protected:
		ID2D1Factory* FactoryPtr;
		D2D1Created();
		~D2D1Created();
	};
	class D2D1DrawFactory :protected D2D1Created
	{
	protected:
		int FrequencyDivide;
		ID2D1HwndRenderTarget* RenderTargetPtr;
		IWICImagingFactory* ImageFactoryPtr;
		ID2D1Bitmap* createImageFromMemory(IWICBitmapDecoder* decorderPtr, unsigned char* buffer, unsigned int size);
	public:
		D2D1DrawFactory(HWND hwnd, unsigned int width = 1280 , unsigned int height = 720);
		virtual ~D2D1DrawFactory();
		void drawStep(DrawParametersD2D1 drawPara);
		void drawTextStep(TextParametersD2D1 textPara, bool resetTransform);
		void beginDraw();
		bool endDraw();
		void setDpi(float dpi = 96.0f);
		void setFrequencyDivide(int divide = 1);
		ID2D1HwndRenderTarget* getInnerHwndRenderTarget();
		ID2D1Bitmap* createImageFromMemoryBMP(unsigned char* buffer, unsigned int size);
		ID2D1Bitmap* createImageFromMemoryJPG(unsigned char* buffer, unsigned int size);
		ID2D1Bitmap* createImageFromMemoryPNG(unsigned char* buffer, unsigned int size);
		std::vector<ID2D1Bitmap*> createMultipleImagesFromMemoryGIF(unsigned char* buffer, unsigned int size);
	};
};