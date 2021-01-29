#include "XA2Player.h"
#include <cmath>
#include <cstdio>
namespace AwpSoftGameModule
{
	void XA2PlayerMaster::addRef() 
	{ 
		Reference++; 
	}

	void XA2PlayerMaster::release()
	{
		Reference--;
		if (!Reference)
		{
			if (Enable) 
			{
				Enable = false;
				XA2Master->DestroyVoice();
				XA2Master = nullptr;
				XA2->Release();
				XA2 = nullptr;
			}
			delete this;
		}
	}
	XA2PlayerMaster::XA2PlayerMaster()
	{
		HRESULT hr;
		Reference = 1;
		Enable = false;
		XA2 = nullptr;
		XA2Master = nullptr;
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
			XA2 = nullptr;
			return;
		}
		Enable = true;
	}
	void XA2PlayerMaster::setVolume(float volume)
	{
		if (Enable)
		{
			XA2Master->SetVolume(volume);
		}
	}

	float XA2PlayerMaster::getVolume()
	{
		if (!Enable) return 0.0f;
		float result;
		XA2Master->GetVolume(&result);
		return result;
	}
	bool XA2PlayerMaster::enable()
	{
		return Enable;
	}

	IXAudio2 * XA2PlayerMaster::getInnerXAudio2Ptr()
	{
		return XA2;
	}

	XA2PlayerMaster::~XA2PlayerMaster() 
	{
	}



	void XA2Player::replay()
	{
		if (!Enable) return;
		if (!LastBuffer.pAudioData) return;
		SourcePoster->Stop();
		SourcePoster->FlushSourceBuffers();
		SourcePoster->SubmitSourceBuffer(&LastBuffer);
		SourcePoster->Start();
	}
	void XA2Player::stop()
	{
		if (Enable)
		{
			SourcePoster->Stop();
			SourcePoster->FlushSourceBuffers();
		}
	}
	void XA2Player::pause()
	{
		if (Enable)
		{
			SourcePoster->Stop();
		}
	}
	void XA2Player::continuePlay()
	{
		if (Enable)
		{
			SourcePoster->Start();
		}
	}
	void XA2Player::setVolume(float volume)
	{
		if (Enable)
		{
			SourcePoster->SetVolume(volume);
		}
	}
	float XA2Player::getVolume()
	{
		if (!Enable) return 0.0f;
		float result;
		SourcePoster->GetVolume(&result);
		return result;
	}
	void XA2Player::setFreqRate(float rate)
	{
		if (Enable)
		{
			SourcePoster->SetFrequencyRatio(rate);
		}
	}
	float XA2Player::getFreqRate()
	{
		if (!Enable) return 1.0f;
		float result;
		SourcePoster->GetFrequencyRatio(&result);
		return result;
	}
	void XA2Player::setInputSampleRate(unsigned int sampleRate)
	{
		if (!Enable) return;
		if (sampleRate == 0) sampleRate = OriginFormat[0].nSamplesPerSec;
		SourcePoster->SetSourceSampleRate(sampleRate);
	}
	XA2Player::XA2Player(XA2PlayerMaster * master, WAVEFORMATEX * inputFormat)
	{
		Enable = false;
		SourcePoster = nullptr;
		memset(&LastBuffer, 0, sizeof(XAUDIO2_BUFFER));
		if (!master) return;
		if (!master->enable()) return;

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
		
		HRESULT hr = master->getInnerXAudio2Ptr()->CreateSourceVoice(&SourcePoster,OriginFormat);
		if (FAILED(hr)) return;
		Master = master;
		master ->addRef();
		Enable = true;
	}
	XA2Player::~XA2Player()
	{
		if (Enable)
		{
			Enable = false;
			SourcePoster->DestroyVoice();
			SourcePoster = nullptr;
			Master->release();
			Master = nullptr;
		}
	}
	bool XA2Player::enable()
	{
		return Enable;
	}
	bool XA2Player::isSafeToReleasePostedBuffers()
	{
		if (!Enable) return true;
		XAUDIO2_VOICE_STATE s;
		SourcePoster->GetState(&s);
		return !s.BuffersQueued;
	}
	void XA2Player::play(XAUDIO2_BUFFER *xa2Buffer, bool resetIfNoChange)
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
	void XA2Player::play(unsigned char * startAddress, unsigned int endOffset_orSize, bool resetIfNoChange, bool loop, unsigned int loopStartOffset, unsigned int loopEndOffset)
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
		play(&bemp, resetIfNoChange);
	}
};