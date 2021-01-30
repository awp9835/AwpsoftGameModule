#pragma once
#include <windows.h>
#include <d2d1.h>
#include <dshow.h>
#include "TimeVariantObject.h"
#include "DoubleThreadLock.h"
#include "D2D1DrawFactory.h"
#pragma comment  (lib,"Dxguid.lib")
#pragma comment  (lib, "d2d1")
#include <vector>
using namespace D2D1;
namespace AwpSoftGameModule
{
	class GameObject2D :public TimeVariantObject
	{
	public:
		bool Enable, Visible, WillDestory, ExistLifeTime;
		float PosCenterX, PosCenterY, PicCenterX, PicCenterY, RotationDEG, SecondaryAlpha, WScale, HScale;
		long long LifeTime;	
		GameObject2D();
		virtual ~GameObject2D();
		virtual bool giveTime(int timeGived);
		virtual void reset();
	};
	class GameObjectD2D1 :public GameObject2D
	{
	protected:
		ID2D1Bitmap* Image;//必须用对应的RenderTarget绘制
	public:
		GameObjectD2D1();
		virtual ~GameObjectD2D1();
		void setImage(ID2D1Bitmap* img);
		virtual DrawParametersD2D1 getDrawParameters();
		virtual void draw(D2D1DrawFactory *drawFactory);
		virtual void reset();
	};
	class MovingGameObjectD2D1 :public GameObjectD2D1
	{
	public:
		float VelocityX, VelocityY, AccelerX, AccelerY, OmegaDEG, EpsilonDEG;
		long long MoveTimeRemain;
		MovingGameObjectD2D1();
		virtual ~MovingGameObjectD2D1();
		virtual bool giveTime(int timeGived);
		virtual void reset();
	};
	class TextBoxD2D1 :public MovingGameObjectD2D1
	{
	protected:
		std::vector<wchar_t> Text;
		int StrLength;
	public:
		float TextLeftX, TextRightX, TextTopY, TypedLength, TypeSpeed;
		bool BoundAlpha, BoundTrasnform, UseRelativePos, TypingMode;
		IDWriteTextFormat *TextFormat;
		D2D_COLOR_F ColorF;
		TextBoxD2D1();
		virtual ~TextBoxD2D1();
		void skipTyping();
		bool isTypingComplete();
		void setText(wchar_t* wstr);
		virtual void reset();
		virtual bool giveTime(int timeGived);
		virtual TextParametersD2D1 getTextParameters();
		virtual void draw(D2D1DrawFactory *drawFactory);
	};
};