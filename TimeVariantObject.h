#pragma once
#include <windows.h>
#include <atomic>
class TimeVariantObject
{
public:
	volatile std::atomic<INT64> TimeRemain;
	volatile std::atomic<BOOL> DisableTimeVariant;
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
	volatile std::atomic<BOOL> Ready;
	volatile std::atomic<UINT32> Cycle;
	TimerTrigger(INT32 TimerCycle = 1);
	void ClearStates();
	BOOL TryTriggerOnce();
	virtual BOOL GiveTime(INT32 TimeGived);
};
class TimerClip :public TimeVariantObject
{
public:
	volatile std::atomic<INT32> Charged;
	volatile std::atomic<INT32> MaxCharge;
	volatile std::atomic<UINT32> Cycle;
	TimerClip(INT32 MaximumCharge = 1, INT32 TimerCycle = 1);
	void ClearStates();
	BOOL TryConsume(INT32 Count = 1);
	INT32 TryConsumePart(INT32 Count = 1);
	virtual BOOL GiveTime(INT32 TimeGived);
};