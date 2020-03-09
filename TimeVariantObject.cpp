#include "TimeVariantObject.h"
#include <Windows.h>
namespace AwpSoftGameModule
{
	INT64 TVOSafeModINT64(INT64 a, INT64 b)
	{
		if (b == 0) return 0;
		return a % b;
	}
	INT64 TVOSafeDivINT64(INT64 a, INT64 b)
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
		DisableTimeVariant = FALSE;
		TimeRemain = 0;
	}
	TimeVariantObject::~TimeVariantObject()
	{
	}
	inline void TimeVariantObject::ClearTimeRemain()
	{
		TimeRemain = 0;
	}

	BOOL TimeVariantObject::GiveTime(INT32 TimeGived)
	{
		GiveTime_Default(TimeGived);
		return FALSE;
	}

	BOOL TimeVariantObject::GiveTime_Default(INT32 TimeGived)
	{
		if (DisableTimeVariant) return FALSE;
		if (TimeGived < 0) return FALSE;
		if (TimeRemain + TimeGived < 0)
		{
			TimeRemain = MAXINT64;
		}
		else
		{
			TimeRemain += TimeGived;
		}
		return TRUE;
	}

	BOOL TimerTrigger::GiveTime(INT32 TimeGived)
	{
		if (!GiveTime_Default(TimeGived)) return FALSE;
		if (Cycle == 0)
		{
			TimeRemain = 0;
			Ready = TRUE;
			return TRUE;
		}
		if (TimeRemain >= Cycle) {
			TimeRemain = TVOSafeModINT64(TimeRemain, Cycle);
			Ready = TRUE;
			return TRUE;
		}
		return FALSE;
	}

	TimerTrigger::TimerTrigger(INT32 TimerCycle)
	{
		if (TimerCycle < 0) TimerCycle = 0;
		Cycle = TimerCycle;
		Ready = FALSE;
	}

	void TimerTrigger::ClearStates()
	{
		TimeRemain = 0;
		Ready = 0;
	}

	BOOL TimerTrigger::TryTriggerOnce()
	{
		if (Ready)
		{
			Ready = FALSE;
			return TRUE;
		}
		return FALSE;
	}

	TimerClip::TimerClip(INT32 MaximumCharge, INT32 TimerCycle)
	{
		if (TimerCycle < 0) TimerCycle = 0;
		if (MaximumCharge < 0) MaximumCharge = 0;
		Cycle = TimerCycle;
		MaxCharge = MaximumCharge;
		Charged = 0;
	}

	void TimerClip::ClearStates()
	{
		TimeRemain = 0;
		Charged = 0;
	}

	BOOL TimerClip::TryConsume(INT32 Count)
	{
		if (Count <= 0) return TRUE;
		if (Charged >= Count)
		{
			Charged -= Count;
			return TRUE;
		}
		return FALSE;
	}

	INT32 TimerClip::TryConsumePart(INT32 Count)
	{

		if (Count <= 0) return 1;
		INT32 TempCharged = Charged;
		Count = min(TempCharged, Count);
		Charged -= Count;
		return Count;
	}

	BOOL TimerClip::GiveTime(INT32 TimeGived)
	{
		if (Charged >= MaxCharge)
		{
			TimeRemain = 0;
			return FALSE;
		}
		if (!GiveTime_Default(TimeGived)) return FALSE;
		if (Cycle == 0 && Charged < MaxCharge)
		{
			Charged = MaxCharge;
			TimeRemain = 0;
			return TRUE;
		}
		INT32 ChargeInc = (INT32)TVOSafeDivINT64(TimeRemain, Cycle);
		if (!ChargeInc) return FALSE;
		if (Charged + ChargeInc >= MaxCharge || Charged + ChargeInc < 0)
		{
			Charged = MaxCharge;
			TimeRemain = 0;
			return TRUE;
		}
		Charged += ChargeInc;
		TimeRemain = TVOSafeModINT64(TimeRemain, Cycle);
		return TRUE;
	}
};