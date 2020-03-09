#pragma once
#include <windows.h>
namespace AwpSoftGameModule
{
	class DSCreated
	{
	private:
		INT32 Reference;
		~DSCreated();
	public:
		void AddRef();
		void Release();
		DSCreated(HWND hwnd);
	};
};