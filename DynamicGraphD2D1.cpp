#include "DynamicGraphD2D1.h"
namespace AwpSoftGameModule
{
	DynamicGraphs::DynamicGraphs()
	{
		CurrentFrame = 0;
		SwitchSpeed = 0.0f;
		Remnant = 0.0f;
		Loop = TRUE;
	}

	DynamicGraphs::DynamicGraphs(ImagesClipD2D1 Clip)
	{
		CurrentFrame = 0;
		SwitchSpeed = 0.0f;
		Remnant = 0.0f;
		Loop = TRUE;
		SetThisClip(Clip);
	}

	DynamicGraphs::~DynamicGraphs()
	{
		CurrentFrame = 0;
		SwitchSpeed = 0.0f;
		Remnant = 0.0f;
		Loop = FALSE;
	}

	void DynamicGraphs::SetThisClip(ImagesClipD2D1 Clip)
	{
		CurrentFrame = 0;
		if (Clip.Frames >= ThisClip.Frames)
		{
			ThisClip.Images = Clip.Images;
			ThisClip.Frames = Clip.Frames;
		}
		else
		{
			ThisClip.Frames = Clip.Frames;
			ThisClip.Images = Clip.Images;
		}
	}

	void DynamicGraphs::SetSwitchPara(FLOAT Speed, BOOL EnableReplay)
	{
		SwitchSpeed = Speed;
		Loop = EnableReplay;
	}

	void DynamicGraphs::SetSwitchSpeed(FLOAT Speed)
	{
		SwitchSpeed = Speed;
	}

	void DynamicGraphs::SetEnableLoop(BOOL EnableReplay)
	{
		Loop = EnableReplay;
	}

	void DynamicGraphs::ResetCurrentFrame()
	{
		CurrentFrame = 0;
	}

	ID2D1Bitmap * DynamicGraphs::GetCurrentFrame()
	{
		if (!ThisClip.Images || !ThisClip.Frames) return NULL;
		return ThisClip.Images[CurrentFrame];
	}

	ID2D1Bitmap * DynamicGraphs::GetLastFrame()
	{
		if (!ThisClip.Images || !ThisClip.Frames) return NULL;
		return ThisClip.Images[ThisClip.Frames - 1];
	}

	ID2D1Bitmap * DynamicGraphs::GetFirstFrame()
	{
		if (!ThisClip.Images || !ThisClip.Frames) return NULL;
		return ThisClip.Images[0];
	}

	ID2D1Bitmap * DynamicGraphs::GetFrame(INT32 Index)
	{
		if (!ThisClip.Images || !ThisClip.Frames) return NULL;
		if (Index < 0) ThisClip.Images[0];
		if (Index >= (INT32)ThisClip.Frames) ThisClip.Images[ThisClip.Frames - 1];
		return ThisClip.Images[Index];
	}

	BOOL DynamicGraphs::GiveTime(INT32 TimeGived)
	{
		if (DisableTimeVariant) return FALSE;
		if (SwitchSpeed == 0.0f) return TRUE;
		if (CurrentFrame == ThisClip.Frames && !Loop) return TRUE;
		FLOAT temp1;
		INT32 temp2;
		temp1 = SwitchSpeed * (FLOAT)TimeGived + Remnant;
		Remnant = temp1 - (FLOAT)((INT32)temp1);
		temp2 = CurrentFrame + (INT32)temp1;
		if (Loop)
		{
			if (!ThisClip.Frames)
			{
				CurrentFrame = 0;
				return TRUE;
			}
			temp2 = temp2 % ThisClip.Frames;
			if (temp2 < 0) temp2 += ThisClip.Frames;
			CurrentFrame = temp2;
			return TRUE;
		}
		if (temp2 < 0) temp2 = 0;
		if (temp2 >= (INT32)ThisClip.Frames) temp2 = (INT32)ThisClip.Frames - 1;
		if (temp2 < 0) temp2 = 0;
		CurrentFrame = temp2;
		return TRUE;
	}
};