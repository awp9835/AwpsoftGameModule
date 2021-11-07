#include "D2D1DrawFactory.h"
#include <process.h>
#include <thread>
#pragma comment(lib,"Winmm.lib")
namespace AwpSoftGameModule
{
	D2D1Created::D2D1Created()
	{
		HRESULT hr;
		FactoryPtr = nullptr;
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &FactoryPtr);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Adapter or driver not support D2D1/DirectX11 !", L"Fatal Error!", MB_ICONERROR);
			exit(0);
		}
	}

	D2D1Created::~D2D1Created()
	{
		if (FactoryPtr) FactoryPtr->Release();
		FactoryPtr = nullptr;
	}

	D2D1DrawFactory::D2D1DrawFactory(HWND hwnd, unsigned int width, unsigned int height)
	{
		HRESULT hr;
		RenderTargetPtr = nullptr;
		ImageFactoryPtr = nullptr;
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&ImageFactoryPtr);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Create Image Factory Failed!", L"Fatal Error!", MB_ICONERROR);
			exit(0);
		}

		hr = FactoryPtr->CreateHwndRenderTarget
		(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(width, height)),
			&RenderTargetPtr
		);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Create Render Target Failed!", L"Fatal Error!", MB_ICONERROR);
			exit(0);
		}
		CurrentFrequency = 60;
		LimitFrequency = 60;
		Counter1 = 0;
		Counter2 = 0;
	}

	D2D1DrawFactory::~D2D1DrawFactory()
	{
		if (RenderTargetPtr) RenderTargetPtr->Release();
		RenderTargetPtr = nullptr;
		if (ImageFactoryPtr) ImageFactoryPtr->Release();
		ImageFactoryPtr = nullptr;
	}

	void D2D1DrawFactory::drawStep(DrawParametersD2D1 drawPara, bool forceTransform)
	{
		if (forceTransform || (drawPara.Visible && drawPara.Image != nullptr))
		{
			D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Identity();
			if (drawPara.HScale != 1.0f || drawPara.WScale != 1.0f)
			{
				transform = transform * D2D1::Matrix3x2F::Scale(drawPara.WScale, drawPara.HScale, D2D1::Point2F(drawPara.PosCenterX, drawPara.PosCenterY));
			}
			if (drawPara.RotationDEG != 0.0f)
			{
				transform = transform * D2D1::Matrix3x2F::Rotation(drawPara.RotationDEG, D2D1::Point2F(drawPara.PosCenterX, drawPara.PosCenterY));
			}
			RenderTargetPtr->SetTransform(transform);
		}
		if (!drawPara.Visible || drawPara.Image == nullptr) return;
		D2D1_SIZE_F imageSize = drawPara.Image->GetSize();
		float leftTopX = drawPara.PosCenterX - drawPara.PicCenterX;
		float leftTopY = drawPara.PosCenterY - drawPara.PicCenterY;
		RenderTargetPtr->DrawBitmap(drawPara.Image, D2D1::RectF(leftTopX, leftTopY, leftTopX + imageSize.width, leftTopY + imageSize.height), drawPara.SecondaryAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, nullptr);
	}
	void D2D1DrawFactory::drawTextStep(TextParametersD2D1 textPara, bool resetTransform)
	{

		if (!textPara.Visible) return;
		if (!textPara.StrBuffer || !textPara.StrLength) return;
		if (!textPara.TextFormat)return;
		textPara.ColorF.a *= textPara.SecondaryAlpha;
		ID2D1SolidColorBrush *colorBrush = nullptr;
		HRESULT hr = RenderTargetPtr->CreateSolidColorBrush(textPara.ColorF, (ID2D1SolidColorBrush **)&colorBrush);
		if (FAILED(hr)) return;
		D2D1_RECT_F layoutRect = D2D1::RectF(textPara.XLeft, textPara.YTop, textPara.XRight, 65536.0f);
		if (resetTransform)
		{
			RenderTargetPtr->SetTransform(D2D1::Matrix3x2F::Identity());
		}
		RenderTargetPtr->DrawText(textPara.StrBuffer, textPara.StrLength, textPara.TextFormat, layoutRect, colorBrush);
		colorBrush->Release();
	}
	void D2D1DrawFactory::beginDraw()
	{
		RenderTargetPtr->BeginDraw();
	}

	bool D2D1DrawFactory::endDraw()
	{
		int limitfreq = min(LimitFrequency, CurrentFrequency);
		int counterex = Counter1 + CurrentFrequency;
		Counter2 += limitfreq;
		while (Counter2 < counterex)
		{
			RenderTargetPtr->EndDraw();
			RenderTargetPtr->BeginDraw();
			Counter2 += limitfreq;
		}
		Counter2 -= counterex;
		Counter1 = 0;
		HRESULT hr = RenderTargetPtr->EndDraw();
		if (FAILED(hr)) return false;
		return true;
	}

	void D2D1DrawFactory::setDpi(float dpi)
	{
		RenderTargetPtr->SetDpi(dpi, dpi);
	}

	void D2D1DrawFactory::setLimitFrequency(int limit)
	{
		if (limit > 1024) limit = 1024;
		else if (limit < 1) limit = 1;
		LimitFrequency = limit;
	}

	void D2D1DrawFactory::correctCurrentFrequency(int current)
	{
		if (current > 1024) current = 1024;
		else if (current < 1) current = 1;
		CurrentFrequency = current;
	}


	ID2D1HwndRenderTarget* D2D1DrawFactory::getInnerHwndRenderTarget()
	{
		return RenderTargetPtr;
	}

	ID2D1Bitmap* D2D1DrawFactory::createImageFromMemory(IWICBitmapDecoder * decorderPtr, unsigned char * buffer, unsigned int size)
	{
		IStream *stream = nullptr;
		IWICBitmapFrameDecode *source = nullptr;
		IWICFormatConverter *converter = nullptr;
		ID2D1Bitmap *bitmap = nullptr;
		HRESULT hr;
		if (!decorderPtr) return nullptr;
		stream = SHCreateMemStream(buffer, size);
		if (!stream) return nullptr;
		hr = decorderPtr->Initialize(stream, WICDecodeMetadataCacheOnDemand);
		if (FAILED(hr))
		{
			stream->Release();
			return nullptr;
		}
		hr = decorderPtr->GetFrame(0, &source);
		stream->Release();
		if (FAILED(hr)) return nullptr;
		hr = ImageFactoryPtr->CreateFormatConverter(&converter);
		if (FAILED(hr))
		{
			source->Release();
			return nullptr;
		}
		hr = converter->Initialize(
			source, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
			NULL, 0.0f, WICBitmapPaletteTypeMedianCut
		);

		if (FAILED(hr))
		{
			converter->Release();
			source->Release();
			return nullptr;
		}
		hr = RenderTargetPtr->CreateBitmapFromWicBitmap(converter, NULL, &bitmap);
		converter->Release();
		source->Release();
		if (FAILED(hr)) return nullptr;
		return bitmap;
	}
	ID2D1Bitmap* D2D1DrawFactory::createImageFromMemoryBMP(unsigned char* buffer, unsigned int size)
	{
		IWICBitmapDecoder* decoder;
		HRESULT hr = ImageFactoryPtr->CreateDecoder(GUID_ContainerFormatBmp, NULL, &decoder);
		if (FAILED(hr)) return nullptr;
		ID2D1Bitmap* temp = createImageFromMemory(decoder, buffer, size);
		decoder->Release();
		return temp;
	}
	ID2D1Bitmap* D2D1DrawFactory::createImageFromMemoryJPG(unsigned char* buffer, unsigned int size)
	{
		IWICBitmapDecoder* decoder;
		HRESULT hr = ImageFactoryPtr->CreateDecoder(GUID_ContainerFormatJpeg, nullptr, &decoder);
		if (FAILED(hr)) return nullptr;
		ID2D1Bitmap* temp = createImageFromMemory(decoder, buffer, size);
		decoder->Release();
		return temp;
	}
	ID2D1Bitmap* D2D1DrawFactory::createImageFromMemoryPNG(unsigned char* buffer, unsigned int size)
	{
		IWICBitmapDecoder* decoder;
		HRESULT hr = ImageFactoryPtr->CreateDecoder(GUID_ContainerFormatPng, nullptr, &decoder);
		if (FAILED(hr)) return nullptr;
		ID2D1Bitmap* temp = createImageFromMemory(decoder, buffer, size);
		decoder->Release();
		return temp;
	}

	std::vector<ID2D1Bitmap*> D2D1DrawFactory::createMultipleImagesFromMemoryGIF(unsigned char* buffer, unsigned int size)
	{
		IWICBitmapDecoder* decoder;
		IWICFormatConverter* converter;
		HRESULT hr;
		hr = ImageFactoryPtr->CreateDecoder(GUID_ContainerFormatGif, NULL, &decoder);
		if (FAILED(hr)) return std::vector<ID2D1Bitmap*>();
		IStream *stream = SHCreateMemStream(buffer, size);;
		IWICBitmapFrameDecode *source = nullptr;

		if (!stream) return std::vector<ID2D1Bitmap*>();
		hr = decoder->Initialize(stream, WICDecodeMetadataCacheOnDemand);
		if (FAILED(hr))
		{
			decoder->Release();
			stream->Release();
			return std::vector<ID2D1Bitmap*>();
		}
		unsigned int frames;
		hr = decoder->GetFrameCount(&frames);
		if (FAILED(hr) || frames == 0)
		{
			decoder->Release();
			source->Release();
			return std::vector<ID2D1Bitmap*>();
		}
		std::vector<ID2D1Bitmap*> bitmaps(frames, nullptr);
		for (unsigned int i = 0; i < frames; i++)
		{
			bitmaps[i] = nullptr;
			hr = decoder->GetFrame(i, &source);
			if (FAILED(hr))
			{
				bitmaps[i] = nullptr;
				continue;
			}
			hr = ImageFactoryPtr->CreateFormatConverter(&converter);
			if (FAILED(hr))
			{
				source->Release();
				bitmaps[i] = nullptr;
				continue;
			}
			hr = converter->Initialize(
				source, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
				NULL, 0.0f, WICBitmapPaletteTypeMedianCut
			);

			if (FAILED(hr))
			{
				converter->Release();
				source->Release();
				bitmaps[i] = nullptr;
				continue;
			}
			hr = RenderTargetPtr->CreateBitmapFromWicBitmap(converter, nullptr, &bitmaps[i]);
			converter->Release();
			source->Release();
		}
		decoder->Release();
		source->Release();
		return std::move(bitmaps);
	}

	DrawParametersD2D1::DrawParametersD2D1()
	{
		PosCenterX = PosCenterY = PicCenterX = PicCenterY = RotationDEG = 0.0f;
		SecondaryAlpha = WScale = HScale = 1.0f;
		Visible = false;
		Image = nullptr;
	}

	TextParametersD2D1::TextParametersD2D1()
	{
		Visible = false;
		SecondaryAlpha = 1.0f;
		StrBuffer = nullptr;
		StrLength = 0;
		ColorF = { 1.0f,1.0f,1.0f,1.0f };
		TextFormat = nullptr;
		XLeft = YTop = 0.0f;
		XRight = 1280.0f;
	}
};