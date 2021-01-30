#include "DynamicGraphD2D1.h"
namespace AwpSoftGameModule
{
	DynamicGraphs::DynamicGraphs()
	{
		CurrentPos = 0;
		SwitchSpeed = 0.0f;
		Remnant = 0.0f;
		Loop = true;
	}

	DynamicGraphs::DynamicGraphs(std::vector<ID2D1Bitmap*> clip)
	{
		CurrentPos = 0;
		SwitchSpeed = 0.0f;
		Remnant = 0.0f;
		Loop = true;
		setClip(std::move(clip));
	}

	DynamicGraphs::~DynamicGraphs()
	{
	}

	void DynamicGraphs::setClip(std::vector<ID2D1Bitmap*> clip, bool resetPos)
	{
		if (resetPos)
		{
			Remnant = 0.0f;
			CurrentPos = 0;
		}
		else if (clip.empty())
		{
			CurrentPos = 0;
		}
		else if (Loop)
		{
			CurrentPos %= clip.size();
		}
		else if (CurrentPos > (int)clip.size())
		{
			CurrentPos = (int)clip.size() - 1;
		}
		ThisClip = std::move(clip);
	}

	void DynamicGraphs::setSwitchParams(float speed, bool enableLoop)
	{
		SwitchSpeed = speed;
		Loop = enableLoop;
	}

	void DynamicGraphs::setSwitchSpeed(float speed)
	{
		SwitchSpeed = speed;
	}

	void DynamicGraphs::setEnableLoop(bool enableLoop)
	{
		Loop = enableLoop;
	}

	void DynamicGraphs::resetCurrentPos()
	{
		Remnant = 0.0f;
		CurrentPos = 0;
	}

	ID2D1Bitmap * DynamicGraphs::getCurrentFrame()
	{
		if (ThisClip.empty()) return nullptr;
		else return ThisClip[CurrentPos];
	}

	ID2D1Bitmap * DynamicGraphs::getLastFrame()
	{
		if (ThisClip.empty()) return nullptr;
		else return ThisClip[ThisClip.size() - 1];
	}

	ID2D1Bitmap * DynamicGraphs::getFirstFrame()
	{
		if (ThisClip.empty()) return nullptr;
		else return ThisClip[0];
	}

	ID2D1Bitmap * DynamicGraphs::getFrame(int index)
	{
		if (ThisClip.empty()) return nullptr;
		else if (index >= (int)ThisClip.size()) return ThisClip[ThisClip.size() - 1];
		else if (index < 0) return ThisClip[0];
		else return ThisClip[index];
	}

	bool DynamicGraphs::giveTime(int TimeGived)
	{
		if (DisableTimeVariant) return false;
		else if (SwitchSpeed == 0.0f) return true;
		else if (!Loop)
		{
			if (SwitchSpeed > 0.0f && CurrentPos == (int)ThisClip.size() - 1)return true;
			if (SwitchSpeed < 0.0f && CurrentPos == 0)return true;
		}
		float temp1;
		int temp2;
		temp1 = SwitchSpeed * (float)TimeGived + Remnant;
		Remnant = temp1 - (float)((int)temp1);
		temp2 = CurrentPos + (int)temp1;
		if (Loop)
		{
			if (ThisClip.empty())
			{
				CurrentPos = 0;
				return true;
			}
			temp2 = temp2 % ThisClip.size();
			if (temp2 < 0) temp2 += (int)ThisClip.size();
			CurrentPos = temp2;
			return true;
		}
		if (temp2 < 0) temp2 = 0;
		if (temp2 >= (int)ThisClip.size()) temp2 = (int)ThisClip.size() - 1;
		if (temp2 < 0) temp2 = 0;
		CurrentPos = temp2;
		return true;
	}
};