#pragma once
#include<Windows.h>
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
		DWORD PrimaryAdd();
		DWORD PrimarySub();
		DWORD SecondaryAdd();
		DWORD SecondarySub();
	protected:
		DWORD CurrentSelect; // Secondary:&0xFFFF0000  Primary:&0x0000FFFF
		SelectorMenuStyle MenuStyle;
		DWORD *UnitAttribute;     // Valid:&0xFF000000 Unlock:&&0x00FF0000 Value:&0x0000FFFF
		UINT PKinds, SKinds;
		UINT Flatten(USHORT Index, USHORT SecondaryIndex);
	public:
		GridSelector(SelectorMenuStyle Style, USHORT Sorts, USHORT SecondarySorts = 1);
		virtual ~GridSelector();
		// Valid:&0xF0000000 Unlock:&&0x0F000000 Value:&0x00FFFFFF
		void SetUnitAttribute(DWORD Attribute, USHORT Index, USHORT SecondaryIndex = 0);
		void SetCurrentAttribute(DWORD Attribute);
		DWORD GetUnitAttribute(USHORT Index, USHORT SecondaryIndex = 0);
		DWORD GetCurrentSelectIndex();
		void SetCurrentSelectIndex(USHORT Index, USHORT SecondaryIndex = 0);
		DWORD GetCurrentSelectAttribute();
		DWORD Up();
		DWORD Down();
		DWORD Left();
		DWORD Right();
	};

};