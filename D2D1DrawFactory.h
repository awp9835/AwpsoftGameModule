#pragma once
#include <d2d1.h>
#include <dshow.h>
#include<shlwapi.h>
#include<wincodec.h>
#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")
#pragma comment  (lib,"strmiids.lib")
#pragma comment  (lib,"Dxguid.lib")
#pragma comment  (lib, "Windowscodecs.lib" )
#pragma comment  (lib, "shlwapi.lib" )
#pragma comment  (lib, "d2d1.lib" )
struct DrawParametersD2D1
{
	FLOAT PosCenterX, PosCenterY, PicCenterX, PicCenterY, RotationDEG,SecondaryAlpha,WScale,HScale;
	BOOL Visible;
	ID2D1Bitmap *Image;//必须用对应的RenderTarget绘制
	DrawParametersD2D1();
};
struct TextParametersD2D1
{
	BOOL Visible;
	FLOAT SecondaryAlpha,XLeft,XRight,YTop;
	WCHAR *StrBuffer;
	UINT32 StrLength;
	D2D_COLOR_F ColorF;
	IDWriteTextFormat *TextFormat;
	TextParametersD2D1();
};
struct ImagesClipD2D1
{
	ID2D1Bitmap* *Images;
	UINT Frames;
	ImagesClipD2D1();
};
class D2D1Created
{
protected:
	ID2D1Factory *pFactory;
	D2D1Created(HWND hwnd);
	~D2D1Created();
}; 
class D2D1DrawFactory :private D2D1Created
{

protected:
	ID2D1HwndRenderTarget *pRenderTarget;
	ID2D1Bitmap* CreateImageFromMemory(IWICBitmapDecoder * pDecoder, LPBYTE Buffer, UINT32 FSize);
	IWICImagingFactory *pIFactory;
public:
	D2D1DrawFactory(HWND hwnd, UINT32 Width = 1280, UINT32 Height = 720);
	~D2D1DrawFactory();
	void DrawStep(DrawParametersD2D1 drawPara);
	void DrawTextStep(TextParametersD2D1 TextPara, BOOL ResetTransForm);
	void BeginDraw();
	BOOL EndDraw();
	ID2D1Bitmap* CreateImageFromMemoryBMP(LPBYTE Buffer,UINT32 FSize);	
	ID2D1Bitmap* CreateImageFromMemoryJPG(LPBYTE Buffer, UINT32 FSize);
	ID2D1Bitmap* CreateImageFromMemoryPNG(LPBYTE Buffer, UINT32 FSize);
	ID2D1Bitmap** CreateMultipleImagesFromMemoryGIF(UINT *_return_Frames,LPBYTE Buffer, UINT32 FSize);
	ImagesClipD2D1 CreateMultipleImagesFromMemoryGIF(LPBYTE Buffer, UINT32 FSize);
};