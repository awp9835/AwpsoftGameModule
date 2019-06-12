#include "CallbackTree.h"

void StartCallbackTree(CallbackTreeFunction Root)
{
	CallbackTreeFunction Next = Root;
	while (Next)
	{
		Next = (CallbackTreeFunction)((*Next)());
	}
}
