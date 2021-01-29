#include "GameObject2D.h"

namespace AwpSoftGameModule
{
	GameObject2D::GameObject2D()
	{
		WillDestory = ExistLifeTime = false;
		PosCenterX = PosCenterY = PicCenterX = PicCenterY = RotationDEG = 0.0f;
		SecondaryAlpha = WScale = HScale = 1.0f;
		Visible = true;
		Enable = true;
		LifeTime = MAXINT64;
	}

	GameObject2D::~GameObject2D()
	{
	}


	bool GameObject2D::giveTime(int timeGived)
	{
		if (!__super::giveTime(timeGived) || !Enable) return false;
		if (ExistLifeTime)
		{
			LifeTime -= timeGived;
			if (LifeTime <= 0)
			{
				Enable = false;
				WillDestory = true;
			}
		}
		return true;
	}

	void GameObject2D::reset()
	{
		WillDestory = ExistLifeTime = false;
		PosCenterX = PosCenterY = PicCenterX = PicCenterY = RotationDEG = 0.0f;
		SecondaryAlpha = WScale = HScale = 1.0f;
		Visible = true;
		Enable = true;
		LifeTime = MAXINT64;
	}

	GameObjectD2D1::GameObjectD2D1()
	{
		Image = nullptr;
	}

	GameObjectD2D1::~GameObjectD2D1()
	{
		Image = nullptr;
	}

	void GameObjectD2D1::setImage(ID2D1Bitmap * img)
	{
		Image = img;
	}

	DrawParametersD2D1 GameObjectD2D1::getDrawParameters()
	{
		DrawParametersD2D1 tmp;
		if (!Enable) return tmp;
		tmp.Image = Image;
		if (!tmp.Image) return tmp;
		tmp.Visible = Visible;
		tmp.PosCenterX = PosCenterX;
		tmp.PosCenterY = PosCenterY;
		tmp.PicCenterX = PicCenterX;
		tmp.PicCenterY = PicCenterY;
		tmp.RotationDEG = RotationDEG;
		tmp.WScale = WScale;
		tmp.HScale = HScale;
		tmp.SecondaryAlpha = SecondaryAlpha;
		return tmp;
	}

	void GameObjectD2D1::draw(D2D1DrawFactory * DrawFactory)
	{
		if (!Enable || !Visible) return;
		DrawFactory->drawStep(getDrawParameters());
	}

	void GameObjectD2D1::reset()
	{
		__super::reset();
		Image = nullptr;
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

	bool MovingGameObjectD2D1::giveTime(int timeGived)
	{
		if (!__super::giveTime(timeGived)) return false;
		if (MoveTimeRemain > 0 && (long long)timeGived > MoveTimeRemain)
		{
			timeGived = (int)MoveTimeRemain;
		}
		MoveTimeRemain -= timeGived;
		PosCenterX += VelocityX * timeGived + AccelerX * (float)timeGived * (float)timeGived / 2.0f;
		PosCenterY += VelocityY * timeGived + AccelerY * (float)timeGived * (float)timeGived / 2.0f;
		RotationDEG += OmegaDEG * timeGived + EpsilonDEG * (float)timeGived * (float)timeGived / 2.0f;
		if (RotationDEG > 360.0f || RotationDEG < -360.0f)
		{
			RotationDEG -= (float)((long long)(RotationDEG / 360.0f)) * 360.0f;
		}
		VelocityX += AccelerX * timeGived;
		VelocityY += AccelerY * timeGived;
		OmegaDEG += EpsilonDEG * timeGived;
		return true;
	}

	void MovingGameObjectD2D1::reset()
	{
		__super::reset();
		VelocityX = 0.0f;
		VelocityY = 0.0f;
		AccelerX = 0.0f;
		AccelerY = 0.0f;
		OmegaDEG = 0.0f;
		EpsilonDEG = 0.0f;
		MoveTimeRemain = -1;
	}

	TextBoxD2D1::TextBoxD2D1()
	{
		Text = std::move(std::vector<wchar_t>());
		StrLength = 0;
		ColorF = { 1.0f,1.0f,1.0f,1.0f };
		TextFormat = nullptr;
		TextLeftX = TextTopY = 0.0f;
		TextRightX = 1280.0f;
		TypingMode = false;
		TypedLength = 0.0f;
		TypeSpeed = 0.020f;
		BoundAlpha = false;
		BoundTrasnform = false;
		UseRelativePos = true;
	}

	TextBoxD2D1::~TextBoxD2D1()
	{
	}

	void TextBoxD2D1::reset()
	{
		if(TypingMode) TypedLength = 0;
	}

	bool TextBoxD2D1::giveTime(int timeGived)
	{
		if (!__super::giveTime(timeGived)) return false;
		if (TypedLength < (float)StrLength)
			TypedLength += TypeSpeed * (float) timeGived;
		if (TypedLength > (float)StrLength)
			TypedLength = (float)StrLength;
		return true;
	}

	void TextBoxD2D1::skipTyping()
	{
		TypedLength = (float)StrLength;
	}

	bool TextBoxD2D1::typingComplete()
	{
		if (TypedLength + 0.5 >= (float)StrLength) return true;
		else return false;
	}

	void TextBoxD2D1::setText(wchar_t* wstr)
	{
		if (!wstr)
		{
			StrLength = 0;
			TypedLength = 0;
			Text = std::move(std::vector<wchar_t>());
			return;
		}
		TypedLength = 0.0f;
		StrLength = (int)wcslen(wstr);
		Text = std::move(std::vector<wchar_t>(wcslen(wstr) + 1, 0));
		wcscpy_s(Text.data(), StrLength + 1, wstr);
	}

	TextParametersD2D1 TextBoxD2D1::getTextParameters()
	{
		TextParametersD2D1 temp;
		if (!Enable) return temp;
		temp.Visible = Visible;
		if (!temp.Visible) return temp;
		temp.ColorF = ColorF;
		temp.TextFormat = TextFormat;
		temp.StrBuffer = Text.data();
		temp.SecondaryAlpha = BoundAlpha ? SecondaryAlpha : 1.0f;
		temp.XLeft = UseRelativePos ? (TextLeftX + PosCenterX) : TextLeftX;
		temp.XRight = UseRelativePos ? (TextRightX + PosCenterX) : TextRightX;
		temp.YTop = UseRelativePos ? (TextTopY + PosCenterY) : TextTopY;
		temp.StrLength = (!TypingMode || typingComplete()) ? StrLength : (max(0, (int)(TypedLength + 0.5)));
		return temp;
	}

	void TextBoxD2D1::draw(D2D1DrawFactory * DrawFactory)
	{
		if (!Enable || !Visible) return;
		DrawFactory->drawStep(getDrawParameters());
		DrawFactory->drawTextStep(getTextParameters(), !BoundTrasnform);
	}
};