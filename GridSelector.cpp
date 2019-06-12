#include "GridSelector.h"
#include <new>
DWORD GridSelector::PrimaryAdd()
{
	DWORD i,s,t;
	i = CurrentSelect & 0x0000FFFF;
	s = CurrentSelect & 0xFFFF0000;
	switch(MenuStyle)
	{
	case MNSTYLE_H:
	case MNSTYLE_V:
	case MNSTYLE_HL:
	case MNSTYLE_VL:
		while (i < PKinds - 1)
		{
			i++;
			t = UnitAttribute[Flatten((USHORT)i, 0)];
			if ((t & 0xF0000000) && (t & 0x0F000000)) 
			{
				CurrentSelect = i ;
				return CurrentSelect;
			}
		}
		break;
	case MNSTYLE_GR:
		while (i < PKinds - 1)
		{
			i++;
			t = UnitAttribute[Flatten((USHORT)i, s>>16)];
			if ((t & 0xF0000000) && (t & 0x0F000000))
			{
				CurrentSelect = i|s;
				return CurrentSelect;
			}
		}
		break;
	default:
		break;
	}
	return CurrentSelect;
}

DWORD GridSelector::PrimarySub()
{
	DWORD i, s, t;
	i = CurrentSelect & 0x0000FFFF;
	s = CurrentSelect & 0xFFFF0000;
	switch (MenuStyle)
	{
	case MNSTYLE_H:
	case MNSTYLE_V:
	case MNSTYLE_HL:
	case MNSTYLE_VL:
		while (i > 0)
		{
			i--;
			t = UnitAttribute[(USHORT)Flatten((USHORT)i,0)];
			if ((t & 0xF0000000) && (t & 0x0F000000))
			{
				CurrentSelect = i;
				return CurrentSelect;
			}
		}
		break;
	case MNSTYLE_GR:
		while (i > 0)
		{
			i--;
			t = GetUnitAttribute((USHORT)i, s >> 16);
			if ((t & 0xF0000000) && (t & 0x0F000000))
			{
				CurrentSelect = i | s;
				return CurrentSelect;
			}
		}
		break;
	default:
		break;
	}
	return CurrentSelect;
}

DWORD GridSelector::SecondaryAdd()
{
	DWORD i, s, t;
	i = CurrentSelect & 0x0000FFFF;
	s = CurrentSelect & 0xFFFF0000;
	s >>= 16;
	switch (MenuStyle)
	{
	case MNSTYLE_H:
	case MNSTYLE_V:
		return CurrentSelect;
	case MNSTYLE_HL:
	case MNSTYLE_VL:
	case MNSTYLE_GR:
		while (s < SKinds - 1)
		{
			s++;
			t = UnitAttribute[Flatten((USHORT)i, (USHORT)s)];
			if ((t & 0xF0000000) && (t & 0x0F000000))
			{
				CurrentSelect = (s<<16)|i;
				return CurrentSelect;
			}
		}
		break;
	default:
		break;
	}
	return CurrentSelect;
}
DWORD GridSelector::SecondarySub()
{
	DWORD i, s, t;
	i = CurrentSelect & 0x0000FFFF;
	s = CurrentSelect & 0xFFFF0000;
	s >>= 16;
	switch (MenuStyle)
	{
	case MNSTYLE_H:
	case MNSTYLE_V:
		return CurrentSelect;
	case MNSTYLE_HL:
	case MNSTYLE_VL:
	case MNSTYLE_GR:
		while (s > 0)
		{
			s--;
			t = UnitAttribute[Flatten((USHORT)i, (USHORT)s)];
			if ((t & 0xF0000000) && (t & 0x0F000000))
			{
				CurrentSelect = (s << 16) | i;
				return CurrentSelect;
			}
		}
		break;
	default:
		break;
	}
	return CurrentSelect;
}

UINT GridSelector::Flatten(USHORT Index, USHORT SecondaryIndex)
{
	return Index * SKinds + SecondaryIndex;
}

GridSelector::GridSelector(SelectorMenuStyle Style, USHORT Kinds, USHORT SecondaryKinds)
{
	MenuStyle = Style;
	PKinds = Kinds;
	SKinds = SecondaryKinds;
	CurrentSelect = 0x0;
	switch (Style)
	{
	case MNSTYLE_V:
	case MNSTYLE_H:
		SKinds = 1;
		break;
	case MNSTYLE_HL:
	case MNSTYLE_VL:
	case MNSTYLE_GR:
		break;
		
	}
	
	try
	{
		UnitAttribute = new DWORD[PKinds*SKinds];
	}
	catch (std::bad_alloc &)
	{
		MessageBox(NULL, L"Not Enough Memory!", L"Fatal Error!", MB_ICONERROR);
		exit(0);
	}
	memset(UnitAttribute, 0, sizeof(DWORD)*PKinds*SKinds);
	for (UINT i = 0; i < PKinds * SKinds; i++)
	{
		UnitAttribute[i] = 0xFF000000;
	}
}

GridSelector::~GridSelector()
{
	if(UnitAttribute) delete UnitAttribute;
	UnitAttribute = NULL;
}

void GridSelector::SetUnitAttribute(DWORD Attribute, USHORT Index, USHORT SecondaryIndex)
{
	Index = min(Index, PKinds - 1);
	SecondaryIndex = min(SecondaryIndex, SKinds - 1);
	UnitAttribute[Flatten(Index, SecondaryIndex)] = Attribute;
}

void GridSelector::SetCurrentAttribute(DWORD Attribute)
{
	DWORD temp = CurrentSelect;
	UnitAttribute[Flatten(temp & 0x0000FFFF, (temp >> 16) & 0x0000FFFF)] = Attribute;
}

DWORD GridSelector::GetUnitAttribute(USHORT Index, USHORT SecondaryIndex)
{
	Index = min(Index, PKinds - 1);
	SecondaryIndex = min(SecondaryIndex, SKinds - 1);
	return UnitAttribute[Flatten(Index, SecondaryIndex)] ;
}

DWORD GridSelector::GetCurrentSelectIndex()
{
	return CurrentSelect;
}

void GridSelector::SetCurrentSelectIndex(USHORT Index, USHORT SecondaryIndex)
{
	Index = min(Index, PKinds - 1);
	SecondaryIndex = min(SecondaryIndex, SKinds - 1);
	CurrentSelect = (SecondaryIndex << 16) | Index;
}

DWORD GridSelector::GetCurrentSelectAttribute()
{
	DWORD temp = CurrentSelect;
	return UnitAttribute[Flatten(temp & 0x0000FFFF, (temp >> 16) & 0x0000FFFF)];
}

DWORD GridSelector::Up()
{
	switch (MenuStyle)
	{
	case MNSTYLE_H:
		return CurrentSelect;
	case MNSTYLE_V:
	case MNSTYLE_VL:
	case MNSTYLE_GR:
		return PrimarySub();
	case MNSTYLE_HL:
		return SecondarySub();
	default:
		break;
	}
	return CurrentSelect;
}

DWORD GridSelector::Down()
{
	switch (MenuStyle)
	{
	case MNSTYLE_H:
		return CurrentSelect;
	case MNSTYLE_V:
	case MNSTYLE_VL:
	case MNSTYLE_GR:
		return PrimaryAdd();
	case MNSTYLE_HL:
		return SecondaryAdd();
	default:
		break;
	}
	return CurrentSelect;
}

DWORD GridSelector::Left()
{
	switch (MenuStyle)
	{
	case MNSTYLE_V:
		return CurrentSelect;
	case MNSTYLE_H:
	case MNSTYLE_HL:
		return PrimarySub();
	case MNSTYLE_GR:
	case MNSTYLE_VL:
		return SecondarySub();
	default:
		break;
	}
	return CurrentSelect;
}
DWORD GridSelector::Right()
{
	switch (MenuStyle)
	{
	case MNSTYLE_V:
		return CurrentSelect;
	case MNSTYLE_H:
	case MNSTYLE_HL:
		return PrimaryAdd();
	case MNSTYLE_GR:
	case MNSTYLE_VL:
		return SecondaryAdd();
	default:
		break;
	}
	return CurrentSelect;
}