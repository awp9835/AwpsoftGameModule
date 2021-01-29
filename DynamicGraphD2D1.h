#pragma once
#include "GameObject2D.h"
#include "vector"
namespace AwpSoftGameModule
{
	class DynamicGraphs :TimeVariantObject
	{
	protected:
		bool Loop;
		int CurrentPos;
		float SwitchSpeed, Remnant;
		std::vector<ID2D1Bitmap*> ThisClip;
	public:
		DynamicGraphs();
		DynamicGraphs(std::vector<ID2D1Bitmap*> clip);
		virtual ~DynamicGraphs();
		void setClip(std::vector<ID2D1Bitmap*> clip, bool resetPos = true);
		void setSwitchParams(float speed, bool enableLoop);
		void setSwitchSpeed(float speed);
		void setEnableLoop(bool enableLoop = false);
		void resetCurrentPos();
		ID2D1Bitmap* getCurrentFrame();
		ID2D1Bitmap* getLastFrame();
		ID2D1Bitmap* getFirstFrame();
		ID2D1Bitmap* getFrame(int index);
		virtual bool giveTime(int timeGived);
	};
};