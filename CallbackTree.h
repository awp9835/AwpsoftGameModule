#pragma once
#include<Windows.h>
namespace AwpSoftGameModule
{
	typedef LPVOID(CALLBACK *CallbackTreeFunction)(void);
	extern void StartCallbackTree(CallbackTreeFunction Root);
};