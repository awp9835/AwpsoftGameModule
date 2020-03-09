#pragma once
#include <windows.h>
namespace AwpSoftGameModule
{
	class FunctionToKey
	{
	public:
		DWORD KeysOfFunction[0x100];
		//A Function Has Four Virtual Keys, Use >> And &0xFF Get,Use << And | Set 
		FunctionToKey();
		//Your Set At This

		virtual ~FunctionToKey();
		void SetKeysOfFunction(INT32 FunctionIndex, BYTE Key1, BYTE Key2 = 0x00, BYTE Key3 = 0x00, BYTE Key4 = 0x00);
		DWORD GetKeysOfFunction(INT32 FunctionIndex);
		BOOL GetFunctionState(INT32 FunctionIndex);
	};
}