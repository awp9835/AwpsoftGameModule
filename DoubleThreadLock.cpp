#include "DoubleThreadLock.h"
#include <thread>
namespace AwpSoftGameModule
{
	DoubleThreadLock::DoubleThreadLock()
	{
		__Lock1 = __Lock2 = __RL1 = __RL2 = false;
	}

	DoubleThreadLock::~DoubleThreadLock() {}

	void DoubleThreadLock::lock1()
	{
		__RL1 = true;
		__Lock1 = true;
		while (__Lock2)
		{
			if (__Lock1)
			{
				__Lock1 = false;
				std::this_thread::yield();
				__Lock1 = true;
			}
		}
	}
	void DoubleThreadLock::lock2()
	{
		__RL2 = true;
		__Lock2 = true;
		while (__Lock1)
		{
			if (__Lock2)
			{
				__Lock2 = false;
				std::this_thread::yield();
				__Lock2 = true;
			}
		}
	}

	void DoubleThreadLock::unlock1()
	{
		__Lock1 = false;
		__RL1 = false;
	}

	void DoubleThreadLock::unlock2()
	{
		__Lock2 = false;
		__RL2 = false;
	}

	void DoubleThreadLock::waitUnlock1()
	{
		while (__RL1) std::this_thread::yield();
	}

	void DoubleThreadLock::waitUnlock2()
	{
		while (__RL2) std::this_thread::yield();
	}
};