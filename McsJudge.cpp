#include "McsJudge.h"

namespace AwpSoftGameModule
{

	BOOL McsjJudgeAbsoluteCircles(FLOAT x1, FLOAT y1, FLOAT r1, FLOAT x2, FLOAT y2, FLOAT r2)
	{
		return (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) < (r1 + r2)*(r1 + r2);

	}

	BOOL McsjJudgeRelativeCircles(
		FLOAT OriginX1, FLOAT OriginY1,
		FLOAT CRotationDEG1, RelativeCircle Circle1,
		FLOAT OriginX2, FLOAT OriginY2,
		FLOAT CRotationDEG2, RelativeCircle Circle2
	)
	{
		return McsjJudgeAbsoluteCircles(
			OriginX1 + Circle1.CenterDistance*MathListCosDEG(Circle1.ThetaDEG + CRotationDEG1),
			OriginY1 + Circle1.CenterDistance*MathListSinDEG(Circle1.ThetaDEG + CRotationDEG1),
			Circle1.Radius,
			OriginX2 + Circle2.CenterDistance*MathListCosDEG(Circle2.ThetaDEG + CRotationDEG2),
			OriginY2 + Circle2.CenterDistance*MathListSinDEG(Circle2.ThetaDEG + CRotationDEG2),
			Circle2.Radius
		);
	}

	UINT32 McsjJudgeMultipleRelativeCirclesCNT(
		FLOAT OriginX1, FLOAT OriginY1, FLOAT CRotationDEG1,
		RelativeCircle *CircleArray1, UINT32 CircleCount1,
		FLOAT OriginX2, FLOAT OriginY2, FLOAT CRotationDEG2,
		RelativeCircle *CircleArray2, UINT32 CircleCount2
	)
	{
		UINT32 i, j, cnt = 0;
		for (i = 0; i < CircleCount1; i++)
			for (j = 0; j < CircleCount2; j++)
			{
				cnt += McsjJudgeRelativeCircles(
					OriginX1, OriginY1,
					CRotationDEG1, CircleArray1[i],
					OriginX2, OriginY2,
					CRotationDEG2, CircleArray2[j]
				);
			}

		return cnt;
	}


	BOOL McsjJudgeMultipleRelativeCircles(
		FLOAT OriginX1, FLOAT OriginY1, FLOAT CRotationDEG1,
		RelativeCircle *CircleArray1, UINT32 CircleCount1,
		FLOAT OriginX2, FLOAT OriginY2, FLOAT CRotationDEG2,
		RelativeCircle *CircleArray2, UINT32 CircleCount2
	)
	{
		UINT32 i, j;
		for (i = 0; i < CircleCount1; i++)
			for (j = 0; j < CircleCount2; j++)
			{
				if (
					McsjJudgeRelativeCircles(
						OriginX1, OriginY1,
						CRotationDEG1, CircleArray1[i],
						OriginX2, OriginY2,
						CRotationDEG2, CircleArray2[j]
					)
					)
				{
					return TRUE;
				}
			}
		return FALSE;
	}
};