#include "XAPlayer.h"
#include <cmath>
#include <cstdio>
namespace AwpSoftGameModule
{
	void DSCreated::AddRef() { Reference++; }

	void DSCreated::Release()
	{
		Reference--;
		if (!Reference)
		{
			if (pDSound)
				pDSound->Release();
			delete this;
		}
	}

	DSCreated::DSCreated(HWND hwnd)
	{
		HRESULT hr;
		EnableLevel = 0;
		Reference = 1;
		memset(&Format, 0, sizeof(WAVEFORMATEX));
		Format.cbSize = 0;
		Format.wFormatTag = WAVE_FORMAT_PCM;
		Format.nChannels = 2;
		Format.nSamplesPerSec = 44100;
		Format.wBitsPerSample = 16;
		Format.nBlockAlign = Format.wBitsPerSample*Format.nChannels / 8;
		Format.nAvgBytesPerSec = Format.nSamplesPerSec*Format.nBlockAlign;
		hr = DirectSoundCreate(NULL, &pDSound, NULL);
		if (FAILED(hr)) return;
		hr = pDSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
		if (SUCCEEDED(hr))
		{
			EnableLevel = 2;

		}
		else
		{
			hr = pDSound->SetCooperativeLevel(hwnd, DSSCL_NORMAL);
			if (FAILED(hr))
			{
				pDSound->Release();
				return;
			}
			EnableLevel = 1;
		}
	}

	DSCreated::~DSCreated() {}
};