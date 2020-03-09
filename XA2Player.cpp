#include "XA2Player.h"
#include <cmath>
#include <cstdio>
namespace AwpSoftGameModule
{
	void XA2PlayerMaster::AddRef() 
	{ 
		Reference++; 
	}

	void XA2PlayerMaster::Release()
	{
		Reference--;
		if (!Reference)
		{
			if (Enable) 
			{
				Enable = FALSE;
				XA2Master->DestroyVoice();
				XA2Master = NULL;
				XA2->Release();
				XA2 = NULL;
			}
			delete this;
		}
	}
	XA2PlayerMaster::XA2PlayerMaster()
	{
		HRESULT hr;
		Reference = 1;
		Enable = FALSE;
		XA2 = NULL;
		XA2Master = NULL;
		CoInitializeEx(0, COINIT_MULTITHREADED);
		hr = XAudio2Create(&XA2, 0, XAUDIO2_DEFAULT_PROCESSOR);
		if (FAILED(hr))
		{
			return;
		}
		hr = XA2->CreateMasteringVoice(&XA2Master);
		if (FAILED(hr))
		{
			XA2->Release();
			XA2 = NULL;
			return;
		}
		Enable = TRUE;
	}
	void XA2PlayerMaster::SetVolume(FLOAT rate)
	{
		if (Enable)
		{
			XA2Master->SetVolume(rate);
		}
	}

	FLOAT XA2PlayerMaster::GetVolume()
	{
		if (!Enable) return 0.0f;
		FLOAT result;
		XA2Master->GetVolume(&result);
		return result;
	}
	BOOL XA2PlayerMaster::IsEnable()
	{
		return Enable;
	}

	IXAudio2 * XA2PlayerMaster::GetXAudio2Ptr()
	{
		return XA2;
	}

	XA2PlayerMaster::~XA2PlayerMaster() 
	{
	}



	void XA2Player::Replay()
	{
		if (!Enable) return;
		if (!LastBuffer.pAudioData) return;
		SourcePoster->Stop();
		SourcePoster->FlushSourceBuffers();
		SourcePoster->SubmitSourceBuffer(&LastBuffer);
		SourcePoster->Start();
	}
	void XA2Player::Stop()
	{
		if (Enable)
		{
			SourcePoster->Stop();
			SourcePoster->FlushSourceBuffers();
		}
	}
	void XA2Player::Pause()
	{
		if (Enable)
		{
			SourcePoster->Stop();
		}
	}
	void XA2Player::Continue()
	{
		if (Enable)
		{
			SourcePoster->Start();
		}
	}
	void XA2Player::SetVolume(FLOAT rate)
	{
		if (Enable)
		{
			SourcePoster->SetVolume(rate);
		}
	}
	FLOAT XA2Player::GetVolume()
	{
		if (!Enable) return 0.0f;
		FLOAT result;
		SourcePoster->GetVolume(&result);
		return result;
	}
	void XA2Player::SetFreqRate(FLOAT rate)
	{
		if (Enable)
		{
			SourcePoster->SetFrequencyRatio(rate);
		}
	}
	FLOAT XA2Player::GetFreqRate()
	{
		if (!Enable) return 1.0f;
		FLOAT result;
		SourcePoster->GetFrequencyRatio(&result);
		return result;
	}
	void XA2Player::SetInputSampleRate(UINT sampleRate)
	{
		if (!Enable) return;
		if (sampleRate == 0) sampleRate = OriginFormat[0].nSamplesPerSec;
		SourcePoster->SetSourceSampleRate(sampleRate);
	}
	XA2Player::XA2Player(XA2PlayerMaster * master, WAVEFORMATEX * inputFormat)
	{
		Enable = FALSE;
		SourcePoster = NULL;
		memset(&LastBuffer, 0, sizeof(XAUDIO2_BUFFER));
		if (!master) return;
		if (!master->IsEnable()) return;

		if (!inputFormat)
		{
			OriginFormat[0].wFormatTag = WAVE_FORMAT_PCM;
			OriginFormat[0].nAvgBytesPerSec = 176400;
			OriginFormat[0].nBlockAlign = 4;
			OriginFormat[0].nSamplesPerSec = 44100;
			OriginFormat[0].wBitsPerSample = 16;
			OriginFormat[0].nChannels = 2;
			OriginFormat[0].cbSize = 0;
		}
		else if (inputFormat->cbSize == 0 ||inputFormat->wFormatTag == WAVE_FORMAT_PCM)
		{
			OriginFormat[0] = *inputFormat;
		}
		else
		{
			memcpy(OriginFormat, inputFormat, sizeof(WAVEFORMATEX) + inputFormat->cbSize);
		}
		
		HRESULT hr = master->GetXAudio2Ptr()->CreateSourceVoice(&SourcePoster,OriginFormat);
		if (FAILED(hr)) return;
		Master = master;
		master ->AddRef();
		Enable = TRUE;
	}
	XA2Player::~XA2Player()
	{
		if (Enable)
		{
			Enable = FALSE;
			SourcePoster->DestroyVoice();
			SourcePoster = NULL;
			Master->Release();
			Master = NULL;
		}
	}
	BOOL XA2Player::IsEnable()
	{
		return Enable;
	}
	BOOL XA2Player::IsSafeToRelease()
	{
		if (!Enable) return TRUE;
		XAUDIO2_VOICE_STATE s;
		SourcePoster->GetState(&s);
		return !s.BuffersQueued;
	}
	void XA2Player::Play(XAUDIO2_BUFFER *xa2Buffer, BOOL resetIfNoChange)
	{
		if (!Enable) return;
		if (!xa2Buffer)
		{
			memset(&LastBuffer, 0, sizeof(XAUDIO2_BUFFER));
			SourcePoster->Stop();
			SourcePoster->FlushSourceBuffers();
			return;
		}
		else if(!xa2Buffer->pAudioData)
		{
			memset(&LastBuffer, 0, sizeof(XAUDIO2_BUFFER));
			SourcePoster->Stop();
			SourcePoster->FlushSourceBuffers();
			return;
		}

		if (resetIfNoChange) //默认强制重播
		{
			LastBuffer = *xa2Buffer;
			SourcePoster->Stop();
			SourcePoster->FlushSourceBuffers();
			SourcePoster->SubmitSourceBuffer(xa2Buffer);
			SourcePoster->Start();
		}
		else if ( 
			xa2Buffer->AudioBytes != LastBuffer.AudioBytes
			|| xa2Buffer->pAudioData != LastBuffer.pAudioData
			|| xa2Buffer->PlayBegin != LastBuffer.PlayBegin
			|| xa2Buffer->PlayLength != LastBuffer.PlayLength
			|| xa2Buffer->LoopBegin != LastBuffer.LoopBegin
			|| xa2Buffer->LoopLength != LastBuffer.LoopLength
			|| xa2Buffer->LoopCount != LastBuffer.LoopCount
			)//重播
		{
			LastBuffer = *xa2Buffer;
			SourcePoster->Stop();
			SourcePoster->FlushSourceBuffers();
			SourcePoster->SubmitSourceBuffer(xa2Buffer);
			SourcePoster->Start();
		}
		else //可能无须重播
		{
			XAUDIO2_VOICE_STATE s;
			SourcePoster->GetState(&s);
			if (s.BuffersQueued == 0) //还是需要重播
			{
				SourcePoster->Stop();
				SourcePoster->SubmitSourceBuffer(xa2Buffer);
				SourcePoster->Start();
			}
		}
	}
	void XA2Player::Play(BYTE * startAddress, UINT32 endOffset_orSize, BOOL resetIfNoChange, BOOL loop, UINT32 loopStartOffset, UINT32 loopEndOffset)
	{
		XAUDIO2_BUFFER bemp;
		memset(&bemp, 0, sizeof(XAUDIO2_BUFFER));
		bemp.AudioBytes = endOffset_orSize;
		bemp.PlayBegin = 0;
		bemp.PlayLength = endOffset_orSize / OriginFormat->nBlockAlign;
		bemp.pAudioData = startAddress;

		if (loopEndOffset > endOffset_orSize) loopEndOffset = endOffset_orSize;
		if (loop && loopEndOffset > loopStartOffset)
		{
			bemp.LoopCount = XAUDIO2_LOOP_INFINITE;
			bemp.LoopBegin = loopStartOffset / OriginFormat->nBlockAlign;
			bemp.LoopLength = (loopEndOffset - loopStartOffset) / OriginFormat->nBlockAlign;
		}
		Play(&bemp, resetIfNoChange);
	}
};