#include "DoubleThreadLock.h"
namespace AwpSoftGameModule
{
	DoubleThreadLock::DoubleThreadLock()
	{
		__Lock1 = __Lock2 = __RL1 = __RL2 = FALSE;
	}

	DoubleThreadLock::~DoubleThreadLock() {}

	void DoubleThreadLock::Lock1()
	{
		__RL1 = TRUE;
		__Lock1 = TRUE;
		while (__Lock2)
		{
			if (__Lock1)
			{
				__Lock1 = FALSE;
				__Lock1 = TRUE;
			}
		}
	}
	void DoubleThreadLock::Lock2()
	{
		__RL2 = TRUE;
		__Lock2 = TRUE;
		while (__Lock1)
		{
			if (__Lock2)
			{
				__Lock2 = FALSE;
				__Lock2 = TRUE;
			}
		}
	}

	void DoubleThreadLock::Unlock1()
	{
		__Lock1 = FALSE;
		__RL1 = FALSE;
	}

	void DoubleThreadLock::Unlock2()
	{
		__Lock2 = FALSE;
		__RL2 = FALSE;
	}

	void DoubleThreadLock::WaitUnlock1()
	{
		while (__RL1);
	}

	void DoubleThreadLock::WaitUnlock2()
	{
		while (__RL2);
	}
};