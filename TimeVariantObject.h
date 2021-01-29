#pragma once
#include <windows.h>
namespace AwpSoftGameModule
{
	class TimeVariantObject
	{
	public:
		long long TimeRemain;
		bool DisableTimeVariant;
		TimeVariantObject();
		virtual ~TimeVariantObject();
		void clearTimeRemain();
		virtual void reset();
		virtual bool giveTime(int timeGived);
	};
	class TimerTrigger :public TimeVariantObject
	{

	public:
		bool Ready;
		int Cycle;
		TimerTrigger(int timerCycle = 1);
		void clearStates();
		bool tryTriggerOnce();
		virtual void reset();
		virtual bool giveTime(int timeGived);
	};
	class TimerClip :public TimeVariantObject
	{
	public:
		int Charged;
		int MaxCharge;
		int Cycle;
		TimerClip(int maximumCharge = 1, int timerCycle = 1);
		void clearStates();
		bool tryConsume(int count = 1);
		int tryConsumePart(int count = 1);
		virtual void reset();
		virtual bool giveTime(int timeGived);
	};
};