#pragma once
#include <windows.h>
namespace AwpSoftGameModule
{
	class FunctionToKey
	{
	public:
		unsigned int KeysOfFunction[0x100];
		//A Function Has Four Virtual Keys, Use >> And &0xFF Get,Use << And | Set 
		FunctionToKey();
		//Your Set At This
		virtual ~FunctionToKey();
		void SetKeysOfFunction(int FunctionIndex, unsigned char Key1, unsigned char Key2 = 0x00, unsigned char Key3 = 0x00, unsigned char Key4 = 0x00);
		unsigned int GetKeysOfFunction(int FunctionIndex);
		bool GetFunctionState(int FunctionIndex);
	};
}