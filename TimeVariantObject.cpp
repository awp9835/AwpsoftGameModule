#include "TimeVariantObject.h"
#include <Windows.h>
namespace AwpSoftGameModule
{
	long long TVOSafeModInt64(long long a, long long b)
	{
		if (b == 0) return 0;
		return a % b;
	}
	long long TVOSafeDivInt64(long long a, long long b)
	{
		if (b == 0)
		{
			if (a > 0) return MAXINT64;
			if (a == 0) return 1;
			if (a < 0) return MININT64;
		}
		return a / b;
	}

	TimeVariantObject::TimeVariantObject()
	{
		DisableTimeVariant = false;
		TimeRemain = 0;
	}
	void TimeVariantObject::reset()
	{
		DisableTimeVariant = false;
		TimeRemain = 0;
	}
	TimeVariantObject::~TimeVariantObject()
	{
	}

	void TimeVariantObject::clearTimeRemain()
	{
		TimeRemain = 0;
	}

	bool TimeVariantObject::giveTime(int timeGived)
	{
		if (DisableTimeVariant) return false;
		if (timeGived < 0) return false;
		TimeRemain += timeGived;
		return true;
	}

	bool TimerTrigger::giveTime(int timeGived)
	{
		if (!__super::giveTime(timeGived)) return false;
		if (Cycle == 0)
		{
			TimeRemain = 0;
			Ready = true;
			return true;
		}
		if (TimeRemain >= Cycle) 
		{
			TimeRemain = TVOSafeModInt64(TimeRemain, Cycle);
			Ready = true;
			return true;
		}
		return true;
	}

	TimerTrigger::TimerTrigger(int timerCycle)
	{
		if (timerCycle < 0) timerCycle = 0;
		Cycle = timerCycle;
		Ready = false;
	}

	void TimerTrigger::clearStates()
	{
		TimeRemain = 0;
		Ready = 0;
	}

	bool TimerTrigger::tryTriggerOnce()
	{
		if (Ready)
		{
			Ready = false;
			return true;
		}
		return false;
	}

	void TimerTrigger::reset()
	{
		clearStates();
	}

	TimerClip::TimerClip(int maximumCharge, int timerCycle)
	{
		if (timerCycle < 0) timerCycle = 0;
		if (maximumCharge < 0) maximumCharge = 0;
		Cycle = timerCycle;
		MaxCharge = maximumCharge;
		Charged = 0;
	}

	void TimerClip::clearStates()
	{
		TimeRemain = 0;
		Charged = 0;
	}

	bool TimerClip::tryConsume(int count)
	{
		if (count <= 0) return true;
		if (Charged >= count)
		{
			Charged -= count;
			return true;
		}
		return false;
	}

	int TimerClip::tryConsumePart(int count)
	{

		if (count <= 0) return 1;
		int TempCharged = Charged;
		count = min(TempCharged, count);
		Charged -= count;
		return count;
	}

	void TimerClip::reset()
	{
		clearStates();
	}

	bool TimerClip::giveTime(int timeGived)
	{
		if (Charged >= MaxCharge)
		{
			TimeRemain = 0;
			return false;
		}
		if (!__super::giveTime(timeGived)) return false;
		if (Cycle == 0 && Charged < MaxCharge)
		{
			Charged = MaxCharge;
			TimeRemain = 0;
			return true;
		}
		int ChargeInc = (int)TVOSafeDivInt64(TimeRemain, Cycle);
		if (!ChargeInc) return false;
		if (Charged + ChargeInc >= MaxCharge || Charged + ChargeInc < 0)
		{
			Charged = MaxCharge;
			TimeRemain = 0;
			return true;
		}
		Charged += ChargeInc;
		TimeRemain = TVOSafeModInt64(TimeRemain, Cycle);
		return true;
	}
};