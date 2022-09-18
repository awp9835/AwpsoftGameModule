#pragma once
/*header*/
#include "AsgmMath.h"
#include <windows.h>
namespace AwpSoftGameModule
{
	/*struct*/
	struct RelativeCircle
	{
		//Using left-hand-polar coordinate system
		float CenterDistance;	//Distance from center of this circle to origin
		float ThetaDEG;	//Polar angle
		float Radius; //Radius of this circle
	};

	/*function*/
	extern bool AsgmJudgeAbsoluteCircles(
		float x1, float y1, float r1,
		float x2, float y2, float r2
	);

	extern bool AsgmJudgeRelativeCircles(
		float originX1, float originY1,
		float centerOfRotationDEG1, RelativeCircle circle1,
		float originX2, float originY2,
		float centerOfRotationDEG2, RelativeCircle circle2
	);
	extern int AsgmJudgeMultipleRelativeCirclesCNT(
		float originX1, float originY1, float centerOfRotationDEG1,
		RelativeCircle* circleArray1, int circleCount1,
		float originX2, float originY2, float centerOfRotationDEG2,
		RelativeCircle* circleArray2,  int circleCount2
	);
	extern bool AsgmJudgeMultipleRelativeCircles(
		float originX1, float originY1, float centerOfRotationDEG1,
		RelativeCircle* circleArray1,  int circleCount1,
		float originX2, float originY2, float centerOfRotationDEG2,
		RelativeCircle* circleArray2,  int circleCount2
	);
};