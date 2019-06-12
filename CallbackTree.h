#pragma once
#include<Windows.h>
typedef LPVOID(CALLBACK *CallbackTreeFunction)(void);
extern void StartCallbackTree(CallbackTreeFunction Root);