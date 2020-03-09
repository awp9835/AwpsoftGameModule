#pragma once
#include <windows.h>
class TimeVariantObject
{
public:
	INT64 TimeRemain;
	BOOL DisableTimeVariant;
	TimeVariantObject();
	virtual ~TimeVariantObject();
	void ClearTimeRemain();
	virtual BOOL GiveTime(INT32 TimeGived);
protected:
	BOOL GiveTime_Default(INT32 TimeGived);
};
class TimerTrigger :public TimeVariantObject
{

public:
	BOOL Ready;
	UINT32 Cycle;
	TimerTrigger(INT32 TimerCycle = 1);
	void ClearStates();
	BOOL TryTriggerOnce();
	virtual BOOL GiveTime(INT32 TimeGived);
};
class TimerClip :public TimeVariantObject
{
public:
	INT32 Charged;
	INT32 MaxCharge;
	UINT32 Cycle;
	TimerClip(INT32 MaximumCharge = 1, INT32 TimerCycle = 1);
	void ClearStates();
	BOOL TryConsume(INT32 Count = 1);
	INT32 TryConsumePart(INT32 Count = 1);
	virtual BOOL GiveTime(INT32 TimeGived);
};