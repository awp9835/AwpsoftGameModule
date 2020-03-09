#include "GameObject2D.h"


namespace AwpSoftGameModule
{
	GameObject2D::GameObject2D()
	{
		WillDestory = ExistLifeTime = FALSE;
		PosCenterX = PosCenterY = PicCenterX = PicCenterY = RotationDEG = 0.0f;
		SecondaryAlpha = WScale = HScale = 1.0f;
		Visible = FALSE;
		Enable = FALSE;
		LifeTime = MAXINT64;
	}

	GameObject2D::~GameObject2D()
	{
		Enable = FALSE;
		Visible = FALSE;
	}

	BOOL GameObject2D::GiveTime_ReduceLifeTime(INT32 TimeGived)
	{
		if (DisableTimeVariant || !Enable) return FALSE;
		if (ExistLifeTime)
		{
			LifeTime -= TimeGived;
			if (LifeTime <= 0)
			{
				Enable = FALSE;
				WillDestory = TRUE;
			}
		}
		return TRUE;
	}

	BOOL GameObject2D::GiveTime(INT32 TimeGived)
	{
		return GiveTime_ReduceLifeTime(TimeGived);
	}

	void GameObject2D::Reset_GameObject2D()
	{
		WillDestory = ExistLifeTime = FALSE;
		PosCenterX = PosCenterY = PicCenterX = PicCenterY = RotationDEG = 0.0f;
		SecondaryAlpha = WScale = HScale = 1.0f;
		Visible = FALSE;
		Enable = FALSE;
		LifeTime = MAXINT64;
	}

	void GameObject2D::ResetGameObject2D()
	{
		Enable = FALSE;
		TimeRemain = 0;
		DisableTimeVariant = FALSE;
		Reset_GameObject2D();
	}

	GameObjectD2D1::GameObjectD2D1()
	{
		Image = NULL;
	}

	GameObjectD2D1::~GameObjectD2D1()
	{
		Image = NULL;
	}

	void GameObjectD2D1::SetImage(ID2D1Bitmap * img)
	{
		Image = img;
	}

	DrawParametersD2D1 GameObjectD2D1::GetDrawParameters()
	{
		DrawParametersD2D1 tmp;
		if (!Enable) return tmp;
		//Lock2();
		tmp.Image = Image;
		if (!tmp.Image)
		{
			//UnLock2();
			return tmp;
		}

		tmp.Visible = Visible;
		tmp.PosCenterX = PosCenterX;
		tmp.PosCenterY = PosCenterY;
		tmp.PicCenterX = PicCenterX;
		tmp.PicCenterY = PicCenterY;
		tmp.RotationDEG = RotationDEG;
		tmp.WScale = WScale;
		tmp.HScale = HScale;
		tmp.SecondaryAlpha = SecondaryAlpha;
		//UnLock2();
		return tmp;
	}

	void GameObjectD2D1::Draw(D2D1DrawFactory * DrawFactory)
	{
		if (!Enable || !Visible) return;
		DrawFactory->DrawStep(GetDrawParameters());
	}

	void GameObjectD2D1::Reset_GameObjectD2D1()
	{
		Reset_GameObject2D();
	}

	void GameObjectD2D1::ResetGameObject2D()
	{
		Reset_GameObjectD2D1();
		Image = NULL;
	}

	MovingGameObjectD2D1::MovingGameObjectD2D1()
	{
		VelocityX = 0.0f;
		VelocityY = 0.0f;
		AccelerX = 0.0f;
		AccelerY = 0.0f;
		OmegaDEG = 0.0f;
		EpsilonDEG = 0.0f;
		MoveTimeRemain = -1;
	}

	MovingGameObjectD2D1::~MovingGameObjectD2D1()
	{
	}

	BOOL MovingGameObjectD2D1::GiveTime(INT32 TimeGived)
	{
		GiveTime_Moving(TimeGived);
		return GiveTime_ReduceLifeTime(TimeGived);
	}

	void MovingGameObjectD2D1::Reset_MovingGameObjectD2D1()
	{
		Reset_GameObjectD2D1();
		VelocityX = 0.0f;
		VelocityY = 0.0f;
		AccelerX = 0.0f;
		AccelerY = 0.0f;
		OmegaDEG = 0.0f;
		EpsilonDEG = 0.0f;
		MoveTimeRemain = -1;
	}

	void MovingGameObjectD2D1::ResetGameObject2D()
	{
		Reset_MovingGameObjectD2D1();
	}

	BOOL MovingGameObjectD2D1::GiveTime_Moving(INT32 TimeGived)
	{
		if (DisableTimeVariant || !Enable) return FALSE;
		if (MoveTimeRemain == 0) return FALSE;
		if (MoveTimeRemain > 0 && (long long)TimeGived > MoveTimeRemain)
		{
			TimeGived = (INT32)MoveTimeRemain;
		}
		MoveTimeRemain -= TimeGived;
		PosCenterX += VelocityX * TimeGived + AccelerX * (FLOAT)TimeGived * (FLOAT)TimeGived / 2.0f;
		PosCenterY += VelocityY * TimeGived + AccelerY * (FLOAT)TimeGived * (FLOAT)TimeGived / 2.0f;
		RotationDEG += OmegaDEG * TimeGived + EpsilonDEG * (FLOAT)TimeGived * (FLOAT)TimeGived / 2.0f;
		if (RotationDEG > 360.0f || RotationDEG < -360.0f)
		{
			RotationDEG -= (FLOAT)((INT64)(RotationDEG / 360.0f))*360.0f;
		}
		VelocityX += AccelerX * TimeGived;
		VelocityY += AccelerY * TimeGived;
		OmegaDEG += EpsilonDEG * TimeGived;
		return TRUE;
	}



	TextBoxD2D1::TextBoxD2D1()
	{
		StrBuffer = NULL;
		StrLength = 0;
		ColorF = { 1.0f,1.0f,1.0f,1.0f };
		TextFormat = NULL;
		TextLeftX = TextTopY = 0.0f;
		TextRightX = 1280.0f;
		TypeMode = FALSE;
		TypedLength = 0;
		TypeSpeed = 0.020f;
		SuperAlpha = FALSE;
		SuperTrasnform = FALSE;
		SuperRelativePos = TRUE;
	}

	TextBoxD2D1::~TextBoxD2D1()
	{
	}

	void TextBoxD2D1::ResetGameObject2D()
	{
		Reset_MovingGameObjectD2D1();
		StrLength = 0;
		if (TypeMode)TypedLength = 0;
	}

	BOOL TextBoxD2D1::GiveTime(INT32 TimeGived)
	{
		GiveTime_Moving(TimeGived);
		GiveTime_TypeText(TimeGived);
		return GiveTime_ReduceLifeTime(TimeGived);
	}

	BOOL TextBoxD2D1::GiveTime_TypeText(INT32 TimeGived)
	{
		if (DisableTimeVariant || !Enable) return FALSE;
		if (TypedLength < (FLOAT)StrLength)
			TypedLength += TypeSpeed * (FLOAT)TimeGived;
		if (TypedLength > (FLOAT)StrLength)
			TypedLength = (FLOAT)StrLength;
		return TRUE;
	}

	void TextBoxD2D1::SkipTyping()
	{
		TypedLength = (FLOAT)StrLength;
	}

	BOOL TextBoxD2D1::TypingComplete()
	{
		if (TypedLength + 0.5 >= (FLOAT)StrLength)
			return TRUE;
		else
			return FALSE;
	}

	TextParametersD2D1 TextBoxD2D1::GetTextParameters()
	{
		TextParametersD2D1 temp;
		if (!Enable) return temp;
		temp.Visible = Visible;
		if (!temp.Visible) return temp;
		temp.ColorF = ColorF;
		temp.TextFormat = TextFormat;
		temp.StrBuffer = StrBuffer;
		temp.SecondaryAlpha = SuperAlpha ? SecondaryAlpha : 1.0f;
		temp.XLeft = SuperRelativePos ? (TextLeftX + PosCenterX) : TextLeftX;
		temp.XRight = SuperRelativePos ? (TextRightX + PosCenterX) : TextRightX;
		temp.YTop = SuperRelativePos ? (TextTopY + PosCenterY) : TextTopY;
		temp.StrLength = (!TypeMode || TypingComplete()) ? StrLength : (max(0, (INT32)(TypedLength + 0.5)));
		return temp;
	}

	void TextBoxD2D1::Draw(D2D1DrawFactory * DrawFactory)
	{
		if (!Enable || !Visible) return;
		DrawFactory->DrawStep(GetDrawParameters());
		DrawFactory->DrawTextStep(GetTextParameters(), !SuperTrasnform);
	}
};