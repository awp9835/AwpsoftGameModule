#include "KeyFunctionMap.h"
#include <cstdio>
namespace AwpSoftGameModule
{
	FunctionToKey::FunctionToKey()
	{
		memset(KeysOfFunction, 0x00, 0x400);
	}

	FunctionToKey::~FunctionToKey()
	{
	}

	void FunctionToKey::SetKeysOfFunction(int FunctionIndex, unsigned char Key1, unsigned char Key2, unsigned char Key3, unsigned char Key4)
	{

		if (FunctionIndex < 0x0 || FunctionIndex >= 0x100)
		{
			return;
		}

		unsigned int temp = Key4;
		temp <<= 8;
		temp |= Key3;
		temp <<= 8;
		temp |= Key2;
		temp <<= 8;
		temp |= Key1;
		KeysOfFunction[FunctionIndex] = temp;
	}

	unsigned int FunctionToKey::GetKeysOfFunction(int FunctionIndex)
	{
		if (FunctionIndex < 0x0 || FunctionIndex >= 0x100)
		{
			return 0x0;
		}

		return KeysOfFunction[FunctionIndex];
	}

	bool FunctionToKey::GetFunctionState(int FunctionIndex)
	{
		if (FunctionIndex < 0x0 || FunctionIndex >= 0x100)
		{
			return false;
		}
		unsigned int temp = KeysOfFunction[FunctionIndex];
		if (GetAsyncKeyState(temp & 0xFF) < 0) return true;
		temp >>= 8;
		if (GetAsyncKeyState(temp & 0xFF) < 0) return true;
		temp >>= 8;
		if (GetAsyncKeyState(temp & 0xFF) < 0) return true;
		temp >>= 8;
		if (GetAsyncKeyState(temp & 0xFF) < 0) return true;
		return false;
	}
};