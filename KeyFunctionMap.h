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
		void setKeysOfFunction(int functionIndex, unsigned char key1, unsigned char key2 = 0x00, unsigned char key3 = 0x00, unsigned char key4 = 0x00);
		unsigned int getKeysOfFunction(int functionIndex);
		bool getFunctionState(int functionIndex);
	};
}