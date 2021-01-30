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
		VerticalSync = true;
		WaitNextFrame = false;
		ThreadState = 10;
		MaxFps = 0;
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
	}

	D2D1DrawFactory::D2D1DrawFactory(int maxfps, HWND hwnd, unsigned int width, unsigned int height)
	{
		HRESULT hr;
		RenderTargetPtr = nullptr;
		ImageFactoryPtr = nullptr;
		VerticalSync = false;
		WaitNextFrame = false;
		ThreadState = 0;
		if (maxfps < 0) maxfps = 0;
		MaxFps = maxfps;
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
			D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(width, height), D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS),
			&RenderTargetPtr
		);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Create Render Target Failed!", L"Fatal Error!", MB_ICONERROR);
			exit(0);
		}
		ThreadState = 0;
		if (_beginthread(&FpsLimitThread, 0, (void*)this) != -1)
		{
			ThreadState = 1;
		}
		else
		{
			VerticalSync = true; //no effect, only for mask
			MessageBox(NULL, L"Create FPS Limit Thread Failed!", L"Warning!", MB_ICONWARNING);
		}
	}

	D2D1DrawFactory::~D2D1DrawFactory()
	{
		if (RenderTargetPtr) RenderTargetPtr->Release();
		RenderTargetPtr = nullptr;
		if (ImageFactoryPtr) ImageFactoryPtr->Release();
		ImageFactoryPtr = nullptr;
		if (VerticalSync) return;
		while (ThreadState == 1) std::this_thread::yield();
		ThreadState = 3;
		while (ThreadState == 3) std::this_thread::yield();
	}

	void D2D1DrawFactory::drawStep(DrawParametersD2D1 drawPara)
	{
		if (!drawPara.Image) return;
		D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Identity();
		float leftTopX = drawPara.PosCenterX - drawPara.PicCenterX;
		float leftTopY = drawPara.PosCenterY - drawPara.PicCenterY;
		D2D1_SIZE_F imageSize = drawPara.Image->GetSize();
		if (!drawPara.Visible) return;

		if (drawPara.HScale != 1.0f || drawPara.WScale != 1.0f)
		{
			transform = transform * D2D1::Matrix3x2F::Scale(drawPara.WScale, drawPara.HScale, D2D1::Point2F(drawPara.PosCenterX, drawPara.PosCenterY));
		}
		if (drawPara.RotationDEG != 0.0f)
		{
			transform = transform * D2D1::Matrix3x2F::Rotation(drawPara.RotationDEG, D2D1::Point2F(drawPara.PosCenterX, drawPara.PosCenterY));
		}
		RenderTargetPtr->SetTransform(transform);
		RenderTargetPtr->DrawBitmap(drawPara.Image, D2D1::RectF(leftTopX, leftTopY, leftTopX + imageSize.width, leftTopY + imageSize.height), drawPara.SecondaryAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, nullptr);
	}
	void D2D1DrawFactory::drawTextStep(TextParametersD2D1 textPara, bool resetTransform = true)
	{

		if (!textPara.Visible) return;
		if (!textPara.StrBuffer || !textPara.StrLength) return;
		if (!textPara.TextFormat)return;
		textPara.ColorF.a *= textPara.SecondaryAlpha;
		ID2D1SolidColorBrush *colorBrush = nullptr;
		HRESULT hr = RenderTargetPtr->CreateSolidColorBrush(textPara.ColorF, (ID2D1SolidColorBrush **)&colorBrush);
		if (FAILED(hr)) return;
		D2D1_RECT_F layoutRect = D2D1::RectF(textPara.XLeft, textPara.YTop, textPara.XRight, 4096.0f);
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
		if (!VerticalSync)
		{
			while (WaitNextFrame) std::this_thread::yield();
			WaitNextFrame = true;
		}
		HRESULT hr = RenderTargetPtr->EndDraw();
		if (SUCCEEDED(hr)) return true;
		return false;
	}

	void D2D1DrawFactory::setDpi(float dpi)
	{
		RenderTargetPtr->SetDpi(dpi, dpi);
	}

	bool D2D1DrawFactory::changeMaxFps(int maxfps)
	{
		if (VerticalSync) return false;
		if (maxfps < 0) return false;
		while (ThreadState == 1) std::this_thread::yield();
		ThreadState = 3;
		while (ThreadState == 3) std::this_thread::yield();
		MaxFps = maxfps;
		ThreadState = 0;
		if (_beginthread(&FpsLimitThread, 0, (void*)this) != -1)
		{
			ThreadState = 1;
			return true;
		}
		else
		{
			VerticalSync = true; //no effect, only for mask
			MessageBox(NULL, L"Create FPS Limit Thread Failed!", L"Warning!", MB_ICONWARNING);
			return false;
		}
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
	void __cdecl D2D1DrawFactory::FpsLimitThread(void* _thisPtr)
	{
		D2D1DrawFactory* thisPtr = (D2D1DrawFactory*)_thisPtr;
		while (thisPtr->ThreadState == 0) std::this_thread::yield();
		thisPtr->ThreadState = 2;
		bool nolimit = (thisPtr->MaxFps <= 0);
		int rmf = thisPtr->MaxFps; 
		int rmt = 1000;
		int expect = rmt / rmf; //[0, 1000]
		while (thisPtr->ThreadState == 2)
		{
			if (nolimit)
			{
				std::this_thread::yield();
				thisPtr->WaitNextFrame = false;
				continue;
			}
			thisPtr->sleepMicroSeconds(expect * 1000);
			thisPtr->WaitNextFrame = false;
			rmf -= 1;
			rmt -= expect;
			if (rmf == 0)
			{
				rmf += thisPtr->MaxFps;
				rmt += 1000;
			}
			if (rmt <= 0)
			{
				expect = 0;
			}
			else
			{
				expect = rmt / rmf;
			}
		}
		thisPtr->ThreadState = 4;
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

	void D2D1DrawFactory::sleepMicroSeconds(int microSeconds)
	{
		if (microSeconds < 0) return;
		if (microSeconds < 1000 && microSeconds >= 100) microSeconds = 1000;
		Sleep(microSeconds / 1000);
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