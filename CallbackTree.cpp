#include "CallbackTree.h"
namespace AwpSoftGameModule
{
	void EnterStdcallCallbackTree(StdcallCallbackTreeFunctionAddress root, void* sharedParamsAddress)
	{
		StdcallCallbackTreeFunctionAddress next = root;
		while (next)
		{
			next = (StdcallCallbackTreeFunctionAddress)((*next)(sharedParamsAddress));
		}
	}
	void EnterCdeclCallbackTree(CdeclCallbackTreeFunctionAddress root, void* sharedParamsAddress)
	{
		CdeclCallbackTreeFunctionAddress next = root;
		while (next)
		{
			next = (CdeclCallbackTreeFunctionAddress)((*next)(sharedParamsAddress));
		}
	}
};