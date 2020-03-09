#pragma once
#include <windows.h>
#include <xaudio2.h>
namespace AwpSoftGameModule
{
	class XA2PlayerMaster
	{
	private:
		INT32 Reference;
		BOOL Enable;
		IXAudio2 *XA2;
		IXAudio2MasteringVoice *XA2Master;
		~XA2PlayerMaster();
	public:
		void AddRef();
		void Release();
		XA2PlayerMaster();
		void SetVolume(FLOAT rate = 1.0f);
		FLOAT GetVolume();
		BOOL IsEnable();
		IXAudio2* GetXAudio2Ptr();
	};
	class XA2Player
	{
	protected:
		XA2PlayerMaster *Master;
		IXAudio2SourceVoice *SourcePoster;
		XAUDIO2_BUFFER LastBuffer;
		WAVEFORMATEX OriginFormat[4]; //Can Enlarge
		BOOL Enable;
	public:
		XA2Player(XA2PlayerMaster* master, WAVEFORMATEX* inputFormat = NULL);
		virtual ~XA2Player();
		BOOL IsEnable();
		BOOL IsSafeToRelease();
		void Play(XAUDIO2_BUFFER * xa2Buffer,BOOL resetIfNotChange = TRUE);
		void Play(BYTE* startAddress,UINT32 endSize, BOOL resetIfNoChange = TRUE,BOOL loop = FALSE,UINT32 loopStartOffset = 0,UINT32 loopEndOffset = 0xFFFFFFFF);
		void Replay();
		void Stop();
		void Pause();
		void Continue();
		void SetVolume(FLOAT rate = 1.0f);
		FLOAT GetVolume();
		void SetFreqRate(FLOAT rate = 1.0f);
		FLOAT GetFreqRate();
		void SetInputSampleRate(UINT sampleRate = 0);
	};
};