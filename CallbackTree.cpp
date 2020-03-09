#include "CallbackTree.h"
namespace AwpSoftGameModule
{
	void StartCallbackTree(CallbackTreeFunction Root)
	{
		CallbackTreeFunction Next = Root;
		while (Next)
		{
			Next = (CallbackTreeFunction)((*Next)());
		}
	}
};