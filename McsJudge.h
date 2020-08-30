#pragma once
/*header*/
#include "MathList.h"
#include <windows.h>
namespace AwpSoftGameModule
{
	/*struct*/
	struct RelativeCircle
	{
		//Using left-hand-polar coordinate system
		FLOAT CenterDistance;	//Distance from center of this circle to origin
		FLOAT ThetaDEG;	//Polar angle
		FLOAT Radius; //Radius of this circle
	};

	/*function*/
	extern BOOL McsjJudgeAbsoluteCircles(
		FLOAT x1, FLOAT y1, FLOAT r1,
		FLOAT x2, FLOAT y2, FLOAT r2
	);


	extern BOOL McsjJudgeRelativeCircles(
		//About 100,000,000t/1.5s RELEASE
		//About 100,000,000t/35s DEBUG
		FLOAT OriginX1, FLOAT OriginY1,
		FLOAT CRotationDEG1, RelativeCircle Circle1,
		FLOAT OriginX2, FLOAT OriginY2,
		FLOAT CRotationDEG2, RelativeCircle Circle2
	);
	extern UINT32 McsjJudgeMultipleRelativeCirclesCNT(
		FLOAT OriginX1, FLOAT OriginY1, FLOAT CRotationDEG1,
		RelativeCircle *CircleArray1, UINT32 CircleCount1,
		FLOAT OriginX2, FLOAT OriginY2, FLOAT CRotationDEG2,
		RelativeCircle *CircleArray2, UINT32 CircleCount2
	);
	extern BOOL McsjJudgeMultipleRelativeCircles(
		FLOAT OriginX1, FLOAT OriginY1, FLOAT CRotationDEG1,
		RelativeCircle *CircleArray1, UINT32 CircleCount1,
		FLOAT OriginX2, FLOAT OriginY2, FLOAT CRotationDEG2,
		RelativeCircle *CircleArray2, UINT32 CircleCount2
	);
};