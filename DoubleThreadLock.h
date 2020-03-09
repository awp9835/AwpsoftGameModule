#pragma once
#include <Windows.h>
#include <atomic>
namespace AwpSoftGameModule
{
	class DoubleThreadLock
	{
	private:
		volatile std::atomic<WORD> __Lock1, __Lock2, __RL1, __RL2;
	public:
		DoubleThreadLock();
		virtual ~DoubleThreadLock();
		void Lock1();
		void Lock2();
		void Unlock1();
		void Unlock2();
		void WaitUnlock1();
		void WaitUnlock2();
	};
}