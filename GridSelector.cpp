#include "GridSelector.h"
#include <new>
namespace AwpSoftGameModule
{
	unsigned int GridSelector::primaryAdd()
	{
		unsigned int i, s, t;
		i = CurrentSelect & 0x0000FFFF;
		s = CurrentSelect & 0xFFFF0000;
		switch (MenuStyle)
		{
		case MNSTYLE_H:
		case MNSTYLE_V:
		case MNSTYLE_HL:
		case MNSTYLE_VL:
			while (i < PKinds - 1)
			{
				i++;
				t = UnitAttribute[flatten((unsigned short)i, 0)];
				if ((t & 0xF0000000) && (t & 0x0F000000))
				{
					CurrentSelect = i;
					return CurrentSelect;
				}
			}
			break;
		case MNSTYLE_GR:
			while (i < PKinds - 1)
			{
				i++;
				t = UnitAttribute[flatten((unsigned short)i, s >> 16)];
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

	unsigned int GridSelector::primarySub()
	{
		unsigned int i, s, t;
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
				t = UnitAttribute[(unsigned short)flatten((unsigned short)i, 0)];
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
				t = getUnitAttribute((unsigned short)i, s >> 16);
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

	unsigned int GridSelector::secondaryAdd()
	{
		unsigned int i, s, t;
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
				t = UnitAttribute[flatten((unsigned short)i, (unsigned short)s)];
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
	unsigned int GridSelector::secondarySub()
	{
		unsigned int i, s, t;
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
				t = UnitAttribute[flatten((unsigned short)i, (unsigned short)s)];
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

	unsigned int GridSelector::flatten(unsigned short index, unsigned short secondaryIndex)
	{
		return index * SKinds + secondaryIndex;
	}

	GridSelector::GridSelector(SelectorMenuStyle style, unsigned short kinds, unsigned short secondaryKinds)
	{
		MenuStyle = style;
		PKinds = kinds;
		SKinds = secondaryKinds;
		CurrentSelect = 0x0;
		switch (style)
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
		UnitAttribute = std::move(std::vector<unsigned int>(SKinds * PKinds , 0xFF000000));
	}

	GridSelector::~GridSelector()
	{
	}

	void GridSelector::setUnitAttribute(unsigned int attribute, unsigned short index, unsigned short secondaryIndex)
	{
		index = (unsigned short)min((unsigned int)index, PKinds - 1);
		secondaryIndex = (unsigned short)min((unsigned int)secondaryIndex, SKinds - 1);
		UnitAttribute[flatten(index, secondaryIndex)] = attribute;
	}

	void GridSelector::setCurrentAttribute(unsigned int attribute)
	{
		unsigned int temp = CurrentSelect;
		UnitAttribute[flatten(temp & 0x0000FFFF, (temp >> 16) & 0x0000FFFF)] = attribute;
	}

	unsigned int GridSelector::getUnitAttribute(unsigned short index, unsigned short secondaryIndex)
	{
		index = (unsigned short)min((unsigned int)index, PKinds - 1);
		secondaryIndex = (unsigned short)min((unsigned int)secondaryIndex, SKinds - 1);
		return UnitAttribute[flatten(index, secondaryIndex)];
	}

	unsigned short GridSelector::getCurrentSelectIndex()
	{
		return (unsigned short)(CurrentSelect & 0x0000FFFF);
	}
	unsigned short GridSelector::getCurrentSelectSecondaryIndex()
	{
		return (unsigned short)((CurrentSelect & 0xFFFF0000) >> 16);
	}
	void GridSelector::SetCurrentSelectIndex(unsigned short index, unsigned short secondaryIndex)
	{
		index = (unsigned short)min((unsigned int)index, PKinds - 1);
		secondaryIndex = (unsigned short)min((unsigned int)secondaryIndex, SKinds - 1);
		CurrentSelect = (secondaryIndex << 16) | index;
	}

	unsigned int GridSelector::getCurrentSelectAttribute()
	{
		unsigned int temp = CurrentSelect;
		return UnitAttribute[flatten(temp & 0x0000FFFF, (temp >> 16) & 0x0000FFFF)];
	}

	unsigned int GridSelector::up()
	{
		switch (MenuStyle)
		{
		case MNSTYLE_H:
			return CurrentSelect;
		case MNSTYLE_V:
		case MNSTYLE_VL:
		case MNSTYLE_GR:
			return primarySub();
		case MNSTYLE_HL:
			return secondarySub();
		default:
			break;
		}
		return CurrentSelect;
	}

	unsigned int GridSelector::down()
	{
		switch (MenuStyle)
		{
		case MNSTYLE_H:
			return CurrentSelect;
		case MNSTYLE_V:
		case MNSTYLE_VL:
		case MNSTYLE_GR:
			return primaryAdd();
		case MNSTYLE_HL:
			return secondaryAdd();
		default:
			break;
		}
		return CurrentSelect;
	}

	unsigned int GridSelector::left()
	{
		switch (MenuStyle)
		{
		case MNSTYLE_V:
			return CurrentSelect;
		case MNSTYLE_H:
		case MNSTYLE_HL:
			return primarySub();
		case MNSTYLE_GR:
		case MNSTYLE_VL:
			return secondarySub();
		default:
			break;
		}
		return CurrentSelect;
	}
	unsigned int GridSelector::right()
	{
		switch (MenuStyle)
		{
		case MNSTYLE_V:
			return CurrentSelect;
		case MNSTYLE_H:
		case MNSTYLE_HL:
			return primaryAdd();
		case MNSTYLE_GR:
		case MNSTYLE_VL:
			return secondaryAdd();
		default:
			break;
		}
		return CurrentSelect;
	}
};