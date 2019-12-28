#pragma once
#include <windows.h>
#include <d2d1.h>
#include <dshow.h>
#include "TimeVariantObject.h"
#include "DoubleThreadLock.h"
#include <atomic>
#include "D2D1DrawFactory.h"
#pragma comment  (lib,"Dxguid.lib")
#pragma comment  (lib, "d2d1")
using namespace D2D1;

class GameObject2D:public TimeVariantObject
{	
public:
	FLOAT PosCenterX,PosCenterY,PicCenterX, PicCenterY, RotationDEG, SecondaryAlpha, WScale, HScale;
	volatile std::atomic<BYTE> Enable, Visible, WillDestory, ExistLifeTime;
	volatile std::atomic<INT64> LifeTime;
	GameObject2D();
	virtual ~GameObject2D();
	BOOL GiveTime_ReduceLifeTime(INT32 TimeGived);
	virtual BOOL GiveTime(INT32 TimeGived);
	void Reset_GameObject2D();
	virtual void ResetGameObject2D();
};
class GameObjectD2D1:public GameObject2D
{
protected:
	volatile std::atomic<ID2D1Bitmap*> Image;//必须用对应的RenderTarget绘制
public:
	GameObjectD2D1();
	virtual ~GameObjectD2D1();
	void SetImage(ID2D1Bitmap* img);
	virtual DrawParametersD2D1 GetDrawParameters();  
	virtual void Draw(D2D1DrawFactory *DrawFactory);
	void Reset_GameObjectD2D1();
	virtual void ResetGameObject2D();
};
class MovingGameObjectD2D1:public GameObjectD2D1
{
public:
	FLOAT VelocityX,VelocityY,AccelerX, AccelerY, OmegaDEG, EpsilonDEG;
	INT64 MoveTimeRemain;
	MovingGameObjectD2D1();
	virtual ~MovingGameObjectD2D1();
	BOOL GiveTime_Moving(INT32 TimeGived);
	virtual BOOL GiveTime(INT32 TimeGived);
	void Reset_MovingGameObjectD2D1();
	virtual void ResetGameObject2D();
};
class TextBoxD2D1 :public MovingGameObjectD2D1
{
public:
	TextBoxD2D1();
	virtual ~TextBoxD2D1();
	FLOAT TextLeftX, TextRightX ,TextTopY, TypedLength,TypeSpeed;
	D2D_COLOR_F ColorF;
	BYTE SuperAlpha, SuperTrasnform, SuperRelativePos,TypeMode;
	IDWriteTextFormat *TextFormat;
	WCHAR *StrBuffer;
	UINT32 StrLength;
	virtual void ResetGameObject2D();//Only Reset StrLength and TypeMode's TypedLength!
	virtual BOOL GiveTime(INT32 TimeGived);
	virtual BOOL GiveTime_TypeText(INT32 TimeGived);
	void SkipTyping();
	BOOL TypingComplete();
	TextParametersD2D1 GetTextParameters();
	virtual void Draw(D2D1DrawFactory *DrawFactory);
};