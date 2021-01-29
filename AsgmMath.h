#pragma once
namespace AwpSoftGameModule
{
	extern const float PentagramX[6];
	extern const float PentagramY[6];
	extern float AsgmCosD(float thetaDEG);
	extern float AsgmSinD(float thetaDEG);
	extern float AsgmArctanD(float x); //x:[-1, 1]
	extern float AsgmSqrt1AddX(float x);//x:[0, 1]
	extern float AsgmDirectionD(float deltaX, float deltaY);
	extern float AsgmDistance(float deltaX, float deltaY);
	extern float AsgmDistance(float deltaX, float deltaY, float deltaZ);
	extern float AsgmDistanceSquare(float deltaX, float deltaY);
	extern float AsgmDistanceSquare(float deltaX, float deltaY, float deltaZ);
};