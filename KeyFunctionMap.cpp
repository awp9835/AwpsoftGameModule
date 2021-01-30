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

	void FunctionToKey::setKeysOfFunction(int functionIndex, unsigned char key1, unsigned char key2, unsigned char key3, unsigned char key4)
	{

		if (functionIndex < 0x0 || functionIndex >= 0x100)
		{
			return;
		}

		unsigned int temp = key4;
		temp <<= 8;
		temp |= key3;
		temp <<= 8;
		temp |= key2;
		temp <<= 8;
		temp |= key1;
		KeysOfFunction[functionIndex] = temp;
	}

	unsigned int FunctionToKey::getKeysOfFunction(int functionIndex)
	{
		if (functionIndex < 0x0 || functionIndex >= 0x100)
		{
			return 0x0;
		}

		return KeysOfFunction[functionIndex];
	}

	bool FunctionToKey::getFunctionState(int functionIndex)
	{
		if (functionIndex < 0x0 || functionIndex >= 0x100)
		{
			return false;
		}
		unsigned int temp = KeysOfFunction[functionIndex];
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