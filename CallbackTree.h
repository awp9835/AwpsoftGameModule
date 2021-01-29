#pragma once
#include <process.h>
namespace AwpSoftGameModule
{
	typedef void* (__stdcall *StdcallCallbackTreeFunctionAddress)(void*);
	typedef void* (__cdecl *CdeclCallbackTreeFunctionAddress)(void*);
	extern void EnterStdcallCallbackTree(StdcallCallbackTreeFunctionAddress root, void* sharedParamsAddress);
	extern void EnterCdeclCallbackTree(CdeclCallbackTreeFunctionAddress root, void* sharedParamsAddress);
};