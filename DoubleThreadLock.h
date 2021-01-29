#pragma once
#include <Windows.h>
#include <atomic>
namespace AwpSoftGameModule
{
	class DoubleThreadLock
	{
	private:
		volatile std::atomic<bool> __Lock1, __Lock2, __RL1, __RL2;
	public:
		DoubleThreadLock();
		virtual ~DoubleThreadLock();
		void lock1();
		void lock2();
		void unlock1();
		void unlock2();
		void waitUnlock1();
		void waitUnlock2();
	};
}