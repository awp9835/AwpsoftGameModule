#include "classdmsplayer.h"
#include <process.h>
#include <cmath>
#include <cstdio>
DMSPlayer::DMSPlayer(DSCreated* dsCreated, UINT32 BufferSeconds)
{
	HRESULT hr;
	ReadyForExit = EnableLevel = Playing = ThreadWait = FALSE;
	pDestBuffer = NULL;
	if (!dsCreated) return;
	memset(&Sound, 0, sizeof(Sound));
	memset(&DmspDesc, 0, sizeof(DSBUFFERDESC));
	DmspDesc.dwSize = sizeof(DSBUFFERDESC);
	DmspDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCDEFER | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME;
	DmspDesc.dwReserved = 0;
	DmspDesc.lpwfxFormat = dsCreated->GetFormatPtr();
	DmspDesc.guid3DAlgorithm = GUID_NULL;
	DmspDesc.dwBufferBytes = BufferSeconds * nAvgBytesPerSecond;
	dsUsed = dsCreated;
	dsUsed->AddRef();
	hr = dsCreated->GetDSPtr()->CreateSoundBuffer(&DmspDesc, &DSBuffer, NULL);
	if (FAILED(hr)) 
	{ 
		dsUsed->Release();
		return; 
	}

	switch (dsCreated->GetEnableLevel())
	{
	case 0:
		return;
	case 1:
		break;
	case 2:
		DSBuffer->SetFormat(dsCreated->GetFormatPtr());
		break;
	default:
		break;
	}
	EnableLevel = TRUE;

	return;
}

void DMSPlayer::SafeSleep(INT32 nms)
{
	/*INT32 t, m,i;
	if (nms < 0) nms = 0;
	t = nms / 20;
	m = nms % 20;
	for (i = 1; i <= t; i++)
	{	
		Sleep(20);
		if (!Playing|ReadyForExit)return;
	}
	Sleep(m);*/
	Sleep(7);
}

DMSPlayer::~DMSPlayer()
{
		//MUST wait thread return in child class
	if (EnableLevel)
	{
		EnableLevel = FALSE;
		DSBuffer->Release();
		dsUsed->Release();
	}
}

void DMSPlayer::SetVolume(INT32 Volume)
{
	if (!EnableLevel) return;
	if (Volume > 100) Volume = 100;
	if (Volume < 0) Volume = 0;
	Volume = INT32(10000 * pow(Volume / 100.0, 0.17));
	if (Volume > 10000) Volume = 10000;
	if (Volume < 0) Volume = 0;
	DSBuffer->SetVolume(-10000 + Volume);
}

INT32 DMSPlayer::GetVolume()
{
	LONG Volume;
	if (!EnableLevel) return -1;
	DSBuffer->GetVolume(&Volume);
	Volume = (10000 + Volume);
	Volume = INT32(100 * pow(Volume / 10000.0, 1.0 / 0.17) + 0.5);
	if (Volume > 100) Volume = 100;
	if (Volume < 0) Volume = 0;
	return Volume;
}

 BOOL DMSPlayer::isEnable() { return EnableLevel; }

//if created successful

 BOOL DMSPlayer::isPlaying() { return Playing; }

 void DSCreated::AddRef() { Reference++; }

void DSCreated::Release() 
{
	Reference--; 
	if (!Reference)
	{
		if(pDSound)
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

DSCreated::~DSCreated()
{
	EnableLevel = 0;
}

 WAVEFORMATEX * DSCreated::GetFormatPtr() { return &Format; }

 LPDIRECTSOUND DSCreated::GetDSPtr() { return pDSound; }

 DWORD DSCreated::GetEnableLevel() { return EnableLevel; }

 UINT32 DSCreated::GetReferenceNum() { return Reference; }


DSPlayer::DSPlayer(DSCreated * dsCreated, UINT32 MaximumSeconds):DMSPlayer(dsCreated, MaximumSeconds)
{
}

DSPlayer::~DSPlayer()
{
	if (EnableLevel)
	{
		DSBuffer->Stop();
		DSBuffer->SetCurrentPosition(0);
	}
	
}

void DSPlayer::PlayThread()
{
	if (!Sound.base || Sound.size <= 0)
	{
		Playing = FALSE;
		ThreadWait = FALSE;
		return;
	}
	DSBuffer->Stop();
	DSBuffer->SetCurrentPosition(0);
	
	DSBuffer->Lock(0, DmspDesc.dwBufferBytes, &pDestBuffer, &DmspDesc.dwBufferBytes, NULL, 0, DSBLOCK_ENTIREBUFFER);
	if (Sound.size < INT32(DmspDesc.dwBufferBytes))
	{
		memset((BYTE*)pDestBuffer + Sound.size, 0, DmspDesc.dwBufferBytes - Sound.size);
		memcpy(pDestBuffer, Sound.base + Sound.offset, Sound.size);
	}
	else
	{
		memcpy(pDestBuffer, Sound.base + Sound.offset, DmspDesc.dwBufferBytes);
	}
	DSBuffer->Unlock(pDestBuffer, DmspDesc.dwBufferBytes, NULL, 0);
	DSBuffer->Play(0, 0, 0);
	ThreadWait = FALSE;
}

 void DSPlayer::PlayThreadCaller(LPVOID ThisPtr)
{
	((DSPlayer*)ThisPtr)->PlayThread();
}

void DSPlayer::Replay()
{
	if (!EnableLevel) return;
	while (ThreadWait);
	ThreadWait = TRUE;
	uintptr_t th = _beginthread(PlayThreadCaller, 0, this);
	if (!th) {
		ThreadWait = FALSE;
	}
	while (ThreadWait);
}

void DSPlayer::Play(DmspSound s)
{
	if (!EnableLevel) return;
	while (ThreadWait);
	Sound = s;
	MemoryBarrier();
	ThreadWait = TRUE;
	uintptr_t th= _beginthread(PlayThreadCaller, 0, this);
	if (!th) {
		ThreadWait = FALSE;
	}
	while (ThreadWait);
}

void DSPlayer::Stop()
{
	if (!EnableLevel) return;
	DSBuffer->Stop();
	DSBuffer->SetCurrentPosition(0);
}

void DSPlayer::Pause()
{
	if (!EnableLevel) return;
	DSBuffer->Stop();
}

void DSPlayer::Continue()
{
	if (!EnableLevel) return;
	DWORD P1, P2;
	DWORD buffersize = min(INT32(DmspDesc.dwBufferBytes), Sound.size);
	DSBuffer->GetCurrentPosition(&P1, &P2);
	if (P1 == 0 || P1 >= buffersize) return;
	DSBuffer->Play(0, 0, 0);
}

void DSBGMPlayer::PlayThread()
{
	DWORD P1, P2;
	DWORD buffersize = DmspDesc.dwBufferBytes;
	DSBuffer->Stop();
	DSBuffer->SetCurrentPosition(0);
	while (1)
	{
		if (ReadyForExit)
		{
			ReadyForExit = 2;
			ActivedBuffer = NULL;
			return;
		}
		if (MCStandBy[0].loop) //change
		{
			DSBuffer->Stop();
			DSBuffer->SetCurrentPosition(0);
			MCursor = MCStandBy[0].offset;
			ActivedBuffer = MCStandBy[1].base;
			Sound = MCStandBy[1];
			if (MCursor > Sound.size - 1)	MCursor = Sound.size - 1;
			MCStandBy[0].loop = FALSE;
		}
		if (MCursor < 0 || !Sound.base || Sound.size <= 0)
		{
			DSBuffer->Stop();
			DSBuffer->SetCurrentPosition(0);
			Playing = FALSE;
			ActivedBuffer = NULL;
			Sleep(10);
			continue;
		}
		Playing = TRUE;
		if (INT32(MCursor + buffersize) >= Sound.size)
		{
			while (1)
			{
				if (ReadyForExit)
				{
					ReadyForExit = 2;
					return;
				}
				//DSBuffer->GetCurrentPosition(&P1, &P2);
				DWORD st;
				DSBuffer->GetStatus(&st);
				//if (P1 == 0 && !(st & DSBSTATUS_PLAYING)) break;
				if(!(st & DSBSTATUS_PLAYING)) break;
				Sleep(7);
			}
			DSBuffer->Lock(0, DmspDesc.dwBufferBytes, &pDestBuffer, &DmspDesc.dwBufferBytes, NULL, 0, DSBLOCK_ENTIREBUFFER);
			memcpy(pDestBuffer, Sound.base + Sound.offset + MCursor,(Sound.size - MCursor>0)?(Sound.size - MCursor):0);
			DSBuffer->Unlock(pDestBuffer, DmspDesc.dwBufferBytes, NULL, 0);
			DSBuffer->Play(0, 0, 0);
			while (1)
			{
				if (ReadyForExit)
				{
					ReadyForExit = 2;
					return;
				}
				DSBuffer->GetCurrentPosition(&P1, &P2);
				DWORD st;
				DSBuffer->GetStatus(&st);
				//if (P1 == 0 && !(st & DSBSTATUS_PLAYING) || INT32(P1) >= Sound.size - MCursor) break;
				if (!(st & DSBSTATUS_PLAYING) || INT32(P1) >= Sound.size - MCursor) break;
				Sleep(7);
			}
			DSBuffer->Stop();
			DSBuffer->SetCurrentPosition(0);
			if (Sound.loop > -1)
			{
				MCursor = Sound.loop;
				if (MCursor > Sound.size - 1)	MCursor = Sound.size - 1;
			}
			else
			{
				MCursor = -1;
				Playing = FALSE;
				ActivedBuffer = NULL;
			}
		}
		else
		{
			while (1)
			{
				if (ReadyForExit)
				{
					ReadyForExit = 2;
					return;
				}
				//DSBuffer->GetCurrentPosition(&P1, &P2);
				DWORD st;
				DSBuffer->GetStatus(&st);
				//if (P1 == 0 && !(st & DSBSTATUS_PLAYING)) break;
				if (!(st & DSBSTATUS_PLAYING)) break;
				Sleep(7);
			}
			DSBuffer->Lock(0, DmspDesc.dwBufferBytes, &pDestBuffer, &DmspDesc.dwBufferBytes, NULL, 0, DSBLOCK_ENTIREBUFFER);
			memcpy(pDestBuffer, Sound.base + Sound.offset + MCursor, buffersize);
			DSBuffer->Unlock(pDestBuffer, DmspDesc.dwBufferBytes, NULL, 0);
			MCursor += buffersize;
			DSBuffer->Play(0, 0, 0);
		}
	}

}

 void DSBGMPlayer::PlayThreadCaller(LPVOID ThisPtr)
{
	((DSBGMPlayer*)ThisPtr)->PlayThread();
}



 BOOL DSBGMPlayer::IsSourceBufferSafeToRelease(BYTE * Base) 
 { 
	 return !(
		 Base == ActivedBuffer
		 || Base == PauseBGM.base
		 ); 
 }

 void DSBGMPlayer::ChangeAndPlay(DmspSound s)
{
	if (!EnableLevel) return;
	DSBuffer->Stop();
	MCStandBy[1] = s;
	MCStandBy[0].offset = 0;		//cursor = 0  , not offset
	MemoryBarrier();
	MCStandBy[0].loop = 1;	//will change , not loop
	MCStandBy[0].size = FALSE; //cancel paused
	memset(&PauseBGM, 0, sizeof(DmspSound));
}

void DSBGMPlayer::Stop()
{
	if (!EnableLevel) return;
	DmspSound s;
	memset(&s, 0, sizeof(DmspSound));
	DSBuffer->Stop();
	DSBuffer->SetCurrentPosition(0);
	MCStandBy[1] = s;
	MemoryBarrier();
	MCStandBy[0].loop = 1;	//will change , not loop
	MCStandBy[0].size = FALSE;//cancel paused
	memset(&PauseBGM, 0, sizeof(DmspSound));
	Playing = FALSE;
}

void DSBGMPlayer::Pause()
{
	if (!EnableLevel) return;
	if (MCStandBy[0].loop == 1) return;
	PauseBGM = Sound;
	PauseCursor = MCursor;
	MCStandBy[0].size = 1;	//pause
	DmspSound s;
	memset(&s, 0, sizeof(DmspSound));
	DSBuffer->Stop();
	DSBuffer->SetCurrentPosition(0);
	MCStandBy[1] = s;
	MemoryBarrier();
	MCStandBy[0].loop = 1;	//will change , not loop
	Playing = FALSE;
}

void DSBGMPlayer::Continue()
{
	if (!EnableLevel) return;
	if (MCStandBy[0].size == 0) return;

	DSBuffer->Stop();
	if(PauseBGM.base) ActivedBuffer = PauseBGM.base; 
	MCStandBy[1] = PauseBGM;
	memset(&PauseBGM, 0, sizeof(DmspSound));
	MCStandBy[0].offset = PauseCursor;		//cursor = 0  , not offset
	MemoryBarrier();
	MCStandBy[0].loop = 1;	//will change , not loop
	MCStandBy[0].size = 0;	//continue
	Playing = TRUE;
}

DSBGMPlayer::DSBGMPlayer(DSCreated * dsCreated, UINT32 BufferSeconds):DMSPlayer(dsCreated,BufferSeconds)
{
	if (!EnableLevel) return;
	MCursor = 0;
	memset(MCStandBy,0,2*sizeof(DmspSound));
	memset(&PauseBGM, 0, sizeof(DmspSound));
	PauseCursor = 0;
	BGMBufferSeconds = BufferSeconds;
	ActivedBuffer = NULL;
	uintptr_t th = _beginthread(PlayThreadCaller, 0, this);
	if (!th) {
		EnableLevel = FALSE;
		dsUsed->Release();
	}
	while (ThreadWait);
}

DSBGMPlayer::~DSBGMPlayer()
{
	if (EnableLevel)
	{
		Stop();
		ReadyForExit = 1;
		while (ReadyForExit != 2);
	}
}
