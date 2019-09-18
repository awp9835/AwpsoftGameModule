#include "KeyFunctionMap.h"
#include <cstdio>
FunctionToKey::FunctionToKey()
{
	memset(KeysOfFunction, 0x00, 0x400);
}

FunctionToKey::~FunctionToKey()
{
}

void FunctionToKey::SetKeysOfFunction(INT32 FunctionIndex, BYTE Key1, BYTE Key2, BYTE Key3, BYTE Key4)
{
	
	if (FunctionIndex < 0x0 || FunctionIndex >= 0x100)
	{
		return;
	}
		
	DWORD temp = Key4;
	temp <<= 8;
	temp |= Key3;
	temp <<= 8;
	temp |= Key2;
	temp <<= 8;
	temp |= Key1;
	KeysOfFunction[FunctionIndex] = temp;
}

DWORD FunctionToKey::GetKeysOfFunction(INT32 FunctionIndex)
{
	if (FunctionIndex < 0x0 || FunctionIndex >= 0x100)
	{
		return 0x0;
	}
		
	return KeysOfFunction[FunctionIndex];
}

BOOL FunctionToKey::GetFunctionState(INT32 FunctionIndex)
{
	if (FunctionIndex < 0x0 || FunctionIndex >= 0x100)
	{
		return FALSE;
	}
	DWORD temp = KeysOfFunction[FunctionIndex];
	if (GetAsyncKeyState(temp & 0xFF) < 0) return TRUE;
	temp >>= 8;
	if (GetAsyncKeyState(temp & 0xFF) < 0) return TRUE;
	temp >>= 8;
	if (GetAsyncKeyState(temp & 0xFF) < 0) return TRUE;
	temp >>= 8;
	if (GetAsyncKeyState(temp & 0xFF) < 0) return TRUE;
	return FALSE;
}
