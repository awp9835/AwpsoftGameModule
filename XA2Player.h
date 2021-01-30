#pragma once
#include <windows.h>
#include <xaudio2.h>
namespace AwpSoftGameModule
{
	class XA2PlayerMaster
	{
	private:
		int Reference;
		bool Enable;
		IXAudio2 *XA2;
		IXAudio2MasteringVoice *XA2Master;
		~XA2PlayerMaster();
	public:
		XA2PlayerMaster();
		void addRef();
		void release();
		void setVolume(float volume = 1.0f);
		float getVolume();
		bool isEnable();
		IXAudio2* getInnerXAudio2Ptr();
	};
	class XA2Player
	{
	protected:
		XA2PlayerMaster *Master;
		IXAudio2SourceVoice *SourcePoster;
		XAUDIO2_BUFFER LastBuffer;
		WAVEFORMATEX OriginFormat[4]; //Can Enlarge
		bool Enable;
	public:
		XA2Player(XA2PlayerMaster* master, WAVEFORMATEX* inputFormat = nullptr);
		virtual ~XA2Player();
		bool isEnable();
		bool isSafeToReleasePostedBuffers();
		void play(XAUDIO2_BUFFER * xa2Buffer,bool resetIfNotChange = true);
		void play(unsigned char* startAddress,unsigned int endSize, bool resetIfNoChange = true,bool loop = false,unsigned int loopStartOffset = 0,unsigned int loopEndOffset = 0xFFFFFFFF);
		void replay();
		void stop();
		void pause();
		void continuePlay();
		void setVolume(float volume = 1.0f);
		float getVolume();
		void setFreqRate(float rate = 1.0f);
		float getFreqRate();
		void setInputSampleRate(unsigned int sampleRate = 0);
	};
};