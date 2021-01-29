#include "McsJudge.h"

namespace AwpSoftGameModule
{

	bool AsgmJudgeAbsoluteCircles(float x1, float y1, float r1, float x2, float y2, float r2)
	{
		return (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) < (r1 + r2)*(r1 + r2);
	}

	bool AsgmJudgeRelativeCircles(
		float originX1, float originY1,
		float centerOfRotationDEG1, RelativeCircle circle1,
		float originX2, float originY2,
		float centerOfRotationDEG2, RelativeCircle circle2
	)
	{
		return AsgmJudgeAbsoluteCircles(
			originX1 + circle1.CenterDistance * AsgmCosD(circle1.ThetaDEG + centerOfRotationDEG1),
			originY1 + circle1.CenterDistance * AsgmSinD(circle1.ThetaDEG + centerOfRotationDEG1),
			circle1.Radius,
			originX2 + circle2.CenterDistance * AsgmCosD(circle2.ThetaDEG + centerOfRotationDEG2),
			originY2 + circle2.CenterDistance * AsgmSinD(circle2.ThetaDEG + centerOfRotationDEG2),
			circle2.Radius
		);
	}

	int AsgmJudgeMultipleRelativeCirclesCNT(
		float originX1, float originY1, float centerOfRotationDEG1,
		RelativeCircle *circleArray1, int circleCount1,
		float originX2, float originY2, float centerOfRotationDEG2,
		RelativeCircle *circleArray2, int circleCount2
	)
	{
		int cnt = 0;
		for (int i = 0; i < circleCount1; i++)
		{
			for (int j = 0; j < circleCount2; j++)
			{
				cnt += AsgmJudgeRelativeCircles(
					originX1, originY1,
					centerOfRotationDEG1, circleArray1[i],
					originX2, originY2,
					centerOfRotationDEG2, circleArray2[j]
				);
			}
		}
		return cnt;
	}


	bool AsgmJudgeMultipleRelativeCircles(
		float originX1, float originY1, float centerOfRotationDEG1,
		RelativeCircle *circleArray1, int circleCount1,
		float OriginX2, float OriginY2, float centerOfRotationDEG2,
		RelativeCircle *circleArray2, int circleCount2
	)
	{
		for (int i = 0; i < circleCount1; i++)
		{
			for (int j = 0; j < circleCount2; j++)
			{
				if (
					AsgmJudgeRelativeCircles(
						originX1, originY1,
						centerOfRotationDEG1, circleArray1[i],
						OriginX2, OriginY2,
						centerOfRotationDEG2, circleArray2[j]
					)
					)
				{
					return true;
				}
			}
		}
		return false;
	}
};