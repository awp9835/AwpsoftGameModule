#pragma once
#include <Windows.h>
#include <vector>
namespace AwpSoftGameModule
{
	enum SelectorMenuStyle
	{
		MNSTYLE_V = 0,
		MNSTYLE_H = 1,
		MNSTYLE_VL = 2,
		MNSTYLE_HL = 3,
		MNSTYLE_GR = 4
	};
	class GridSelector
	{
	private:
		unsigned int primaryAdd();
		unsigned int primarySub();
		unsigned int secondaryAdd();
		unsigned int secondarySub();
	protected:
		SelectorMenuStyle MenuStyle;
		unsigned int CurrentSelect; // Secondary:&0xFFFF0000  Primary:&0x0000FFFF
		unsigned int PKinds, SKinds;
		std::vector<unsigned int> UnitAttribute;     // Valid:&0xFF000000 Unlock:&&0x00FF0000 Value:&0x0000FFFF
		unsigned int flatten(unsigned short id, unsigned short secondaryIndex);
	public:
		static constexpr auto Attribute_Valid = 0xF0000000;
		static constexpr auto Attribute_Unlock = 0x0F000000;
		static constexpr auto Attribute_Enable = 0x0F000000;
		GridSelector(SelectorMenuStyle style, unsigned short sorts, unsigned short cecondarySorts = 1);
		virtual ~GridSelector();
		// Valid:&0xF0000000 Unlock:&&0x0F000000 Value:&0x00FFFFFF
		void setUnitAttribute(unsigned int attribute, unsigned short id, unsigned short secondaryIndex = 0);
		void setCurrentAttribute(unsigned int attribute);
		unsigned int getUnitAttribute(unsigned short index, unsigned short secondaryIndex = 0);
		unsigned short getCurrentSelectIndex();
		unsigned short getCurrentSelectSecondaryIndex();
		void setCurrentSelectIndex(unsigned short index, unsigned short secondaryIndex = 0);
		unsigned int getCurrentSelectAttribute();
		unsigned int up();
		unsigned int down();
		unsigned int left();
		unsigned int right();
	};

};