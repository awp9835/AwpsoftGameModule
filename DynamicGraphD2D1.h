#pragma once
#include "GameObject2D.h"





class DynamicGraphs :TimeVariantObject
{
protected:
	ImagesClipD2D1 ThisClip;
	volatile  std::atomic<INT32> CurrentFrame;
	FLOAT SwitchSpeed,Remnant;
	BOOL Loop;
public:
	DynamicGraphs();
	DynamicGraphs(ImagesClipD2D1 Clip);
	virtual ~DynamicGraphs();
	void SetThisClip(ImagesClipD2D1 Clip);
	void SetSwitchPara(FLOAT Speed,BOOL EnableReplay);
	void SetSwitchSpeed(FLOAT Speed);
	void SetEnableLoop(BOOL EnableLoop);
	void ResetCurrentFrame();
	ID2D1Bitmap* GetCurrentFrame();
	ID2D1Bitmap* GetLastFrame();
	ID2D1Bitmap* GetFirstFrame();
	ID2D1Bitmap* GetFrame(INT32 Index);
	virtual BOOL GiveTime(INT32 TimeGived);
};



