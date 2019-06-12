#include "D2D1DrawFactory.h"

D2D1Created::D2D1Created(HWND hwnd)
{
	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pFactory);
	if (FAILED(hr))
	{
		MessageBox(NULL,L"Adapter or driver not support D2D1/DirectX11 !",L"Fatal Error!", MB_ICONERROR);
		exit(0);
	}
}

D2D1Created::~D2D1Created()
{
	if (pFactory) pFactory->Release();
	pFactory = NULL;
}

D2D1DrawFactory::D2D1DrawFactory(HWND hwnd, UINT32 Width, UINT32 Height) :D2D1Created(hwnd)
{
	HRESULT hr;
	BOOL CdcdFailed = FALSE;
	hr = pFactory->CreateHwndRenderTarget
	(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(Width, Height)),
		&pRenderTarget
	);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create Render Target Failed!", L"Fatal Error!", MB_ICONERROR);
		exit(0);
	}
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = CoCreateInstance(CLSID_WICImagingFactory,NULL,CLSCTX_INPROC_SERVER,IID_IWICImagingFactory,(LPVOID*)&pIFactory);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create Image Factory Failed!", L"Fatal Error!", MB_ICONERROR);
		exit(0);
	}	
}

D2D1DrawFactory::~D2D1DrawFactory()
{
	if (pRenderTarget)	pRenderTarget->Release();
	pRenderTarget = NULL;
	if (pIFactory) pIFactory->Release();
	pIFactory = NULL;
}

void D2D1DrawFactory::DrawStep(DrawParametersD2D1 DrawPara)
{
	if (!DrawPara.Image) return;
	D2D1::Matrix3x2F Transform = D2D1::Matrix3x2F::Identity();
	FLOAT LeftTopX = DrawPara.PosCenterX - DrawPara.PicCenterX;
	FLOAT LeftTopY = DrawPara.PosCenterY - DrawPara.PicCenterY;
	D2D1_SIZE_F ImageSize = DrawPara.Image->GetSize();
	if (!DrawPara.Visible) return;
	
	if (DrawPara.HScale != 1.0f || DrawPara.WScale != 1.0f)
	{
		Transform = Transform * D2D1::Matrix3x2F::Scale(DrawPara.WScale, DrawPara.HScale, D2D1::Point2F(DrawPara.PosCenterX, DrawPara.PosCenterY));
	}
	if (DrawPara.RotationDEG != 0.0f)
	{
		Transform = Transform * D2D1::Matrix3x2F::Rotation(DrawPara.RotationDEG, D2D1::Point2F(DrawPara.PosCenterX, DrawPara.PosCenterY));
	}
	pRenderTarget->SetTransform(Transform);
	pRenderTarget->DrawBitmap(DrawPara.Image, D2D1::RectF(LeftTopX, LeftTopY, LeftTopX + ImageSize.width, LeftTopY + ImageSize.height),DrawPara.SecondaryAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,NULL);
}
void D2D1DrawFactory::DrawTextStep(TextParametersD2D1 TextPara, BOOL ResetTransform = TRUE)
{

	if (!TextPara.Visible) return;
	if (!TextPara.StrBuffer || !TextPara.StrLength) return;
	if (!TextPara.TextFormat)return;
	TextPara.ColorF.a *= TextPara.SecondaryAlpha;	
	ID2D1SolidColorBrush *ColorBrush = NULL;
	HRESULT hr = pRenderTarget->CreateSolidColorBrush(TextPara.ColorF, (ID2D1SolidColorBrush **)&ColorBrush);
	if (FAILED(hr)) return;
	D2D1_RECT_F LayoutRect = D2D1::RectF(TextPara.XLeft, TextPara.YTop, TextPara.XRight, 4096.0f);
	if (ResetTransform)
	{
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	pRenderTarget->DrawText(TextPara.StrBuffer, TextPara.StrLength, TextPara.TextFormat,LayoutRect, ColorBrush);
	ColorBrush->Release();
}
void D2D1DrawFactory::BeginDraw()
{
	pRenderTarget->BeginDraw();
}

BOOL D2D1DrawFactory::EndDraw()
{
	HRESULT hr = pRenderTarget->EndDraw();
	if (SUCCEEDED(hr)) return TRUE;
	return FALSE;
}





ID2D1Bitmap * D2D1DrawFactory::CreateImageFromMemory(IWICBitmapDecoder * pDecorder,LPBYTE Buffer, UINT32 FSize)
{
	IStream *s = NULL;
	IWICBitmapFrameDecode *pSource=NULL;
	IWICFormatConverter *pConverter;
	ID2D1Bitmap *pBitmap = NULL;
	HRESULT hr;
	if (!pDecorder) return NULL;
	s = SHCreateMemStream(Buffer, FSize);
	if (!s) return NULL;
	hr = pDecorder->Initialize(s, WICDecodeMetadataCacheOnDemand);
	if (FAILED(hr)) 
	{ 
		s->Release();
		return NULL; 
	}
	hr = pDecorder->GetFrame(0, &pSource);
	s->Release();
	if (FAILED(hr)) return NULL;
	hr = pIFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr))
	{
		pSource->Release();
		return NULL;
	}
	hr= pConverter->Initialize(
		pSource,GUID_WICPixelFormat32bppPBGRA,WICBitmapDitherTypeNone,
		NULL,0.0f,WICBitmapPaletteTypeMedianCut
	);
	
	if (FAILED(hr))
	{
		pConverter->Release();
		pSource->Release();
		return NULL;
	}
	hr= pRenderTarget->CreateBitmapFromWicBitmap(pConverter,NULL,&pBitmap);
	pConverter->Release();
	pSource->Release();
	if (FAILED(hr)) return NULL;
	return pBitmap;
}
ID2D1Bitmap * D2D1DrawFactory::CreateImageFromMemoryBMP(LPBYTE Buffer, UINT32 FSize)
{
	IWICBitmapDecoder *pBMPDecoder;
	HRESULT hr = pIFactory->CreateDecoder(GUID_ContainerFormatBmp, NULL, &pBMPDecoder);
	if (FAILED(hr)) return NULL;
	ID2D1Bitmap* temp=CreateImageFromMemory(pBMPDecoder, Buffer, FSize);
	pBMPDecoder->Release();
	return temp;
}
ID2D1Bitmap * D2D1DrawFactory::CreateImageFromMemoryJPG(LPBYTE Buffer, UINT32 FSize)
{
	IWICBitmapDecoder  *pJPGDecoder;
	HRESULT hr = pIFactory->CreateDecoder(GUID_ContainerFormatJpeg, NULL, &pJPGDecoder);
	if (FAILED(hr)) return NULL;
	ID2D1Bitmap* temp = CreateImageFromMemory(pJPGDecoder, Buffer, FSize);
	pJPGDecoder->Release();
	return temp;
}
ID2D1Bitmap * D2D1DrawFactory::CreateImageFromMemoryPNG(LPBYTE Buffer, UINT32 FSize)
{
	IWICBitmapDecoder *pPNGDecoder;
	HRESULT hr = pIFactory->CreateDecoder(GUID_ContainerFormatPng, NULL, &pPNGDecoder);
	if (FAILED(hr)) return NULL;
	ID2D1Bitmap* temp = CreateImageFromMemory(pPNGDecoder, Buffer, FSize);
	pPNGDecoder->Release();
	return temp;
}

ID2D1Bitmap** D2D1DrawFactory::CreateMultipleImagesFromMemoryGIF(UINT *_return_Frames, LPBYTE Buffer, UINT32 FSize)
{
	IWICBitmapDecoder  *pGIFDecoder;
	IWICFormatConverter *pConverter;
	HRESULT hr;
	hr = pIFactory->CreateDecoder(GUID_ContainerFormatGif, NULL, &pGIFDecoder);
	if (FAILED(hr)) return NULL;
	IStream *s = SHCreateMemStream(Buffer, FSize);;
	IWICBitmapFrameDecode *pSource = NULL;
	ID2D1Bitmap **ppBitmaps = NULL;

	*_return_Frames = 0;
	if (!s) return NULL;
	hr = pGIFDecoder->Initialize(s, WICDecodeMetadataCacheOnDemand);
	if (FAILED(hr))
	{
		pGIFDecoder->Release();
		s->Release();
		return NULL;
	}
	hr = pGIFDecoder->GetFrameCount(_return_Frames);
	if (FAILED(hr)|| *_return_Frames==0)
	{
		pGIFDecoder->Release();
		s->Release();
		return NULL;
	}
	ppBitmaps = new (ID2D1Bitmap*[*_return_Frames]);
	for (int i = 0; i < *_return_Frames; i++)
	{
		ppBitmaps[i] = NULL;
		hr = pGIFDecoder->GetFrame(i, &pSource);
		if (FAILED(hr)) 
		{ 
			ppBitmaps[i] = NULL;
			continue;
		}
		hr = pIFactory->CreateFormatConverter(&pConverter);
		if (FAILED(hr))
		{
			pSource->Release();
			ppBitmaps[i] = NULL;
			continue;
		}
		hr = pConverter->Initialize(
			pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
			NULL, 0.0f, WICBitmapPaletteTypeMedianCut
		);
		
		if (FAILED(hr))
		{
			pConverter->Release();
			pSource->Release();
			ppBitmaps[i] = NULL;
			continue;
		}
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, &ppBitmaps[i]);
		pConverter->Release();
		pSource->Release();
	}
	pGIFDecoder->Release();
	s->Release();
	return ppBitmaps;
}

ImagesClipD2D1 D2D1DrawFactory::CreateMultipleImagesFromMemoryGIF(LPBYTE Buffer, UINT32 FSize)
{
	ImagesClipD2D1 ic;
	ic.Images = CreateMultipleImagesFromMemoryGIF(&ic.Frames, Buffer, FSize);
	return ic;
}

DrawParametersD2D1::DrawParametersD2D1()
{
	PosCenterX = PosCenterY = PicCenterX = PicCenterY = RotationDEG = 0.0f;
	SecondaryAlpha = WScale = HScale = 1.0f;
	Visible = FALSE;
	Image = NULL;
}

ImagesClipD2D1::ImagesClipD2D1()
{
	Images = NULL;
	Frames = 0;
}



TextParametersD2D1::TextParametersD2D1()
{
	Visible = FALSE;
	SecondaryAlpha = 1.0f;
	StrBuffer = NULL;
	StrLength = 0;
	ColorF = { 1.0f,1.0f,1.0f,1.0f };
	TextFormat = NULL;
	XLeft = YTop = 0.0f;
	XRight = 1280.0f;
}
