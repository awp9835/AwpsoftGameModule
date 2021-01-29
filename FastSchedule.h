#pragma once
#include <windows.h>
#include <new>
#include <vector>
namespace AwpSoftGameModule
{
	template <class T> class FastSchedule
	{
	protected:
		std::vector<T*> ScheduleList;
		int Length, Pos, Occupied, Save;
	public:
		FastSchedule(int len);
		~FastSchedule();
		std::vector<T*>& getScheduleListReference();
		int getLength();
		bool full();
		T* gotoNextPos();
		int gotoNextNullPos();
		T* getCurrentObject();
		void clearAll();
		void saveCurrentPos();
		void loadSavedPos();
		void clearCurrentObject();
		int takeOverObject(T* ptr);
		T* takeOutCurrentObject();
	};

	template<class T>
	inline FastSchedule<T>::FastSchedule(int len)
	{
		if (len < 0x10) len = 0x10;
		Length = len;
		Pos = Length - 1;
		Occupied = 0;
		Save = 0;
		ScheduleList = std::move(std::vector<T*>(len, nullptr));
	}

	template<class T>
	inline FastSchedule<T>::~FastSchedule()
	{
		if (!ScheduleList.empty()) ClearAllField();
	}

	template<class T>
	std::vector<T*>& FastSchedule<T>::getScheduleListReference()
	{
		return ScheduleList;
	}

	template<class T>
	inline int FastSchedule<T>::getLength()
	{
		return Length;
	}

	template<class T>
	inline bool FastSchedule<T>::full()
	{
		return Occupied == Length;
	}

	template<class T>
	inline T * FastSchedule<T>::gotoNextPos()
	{
		if (Pos + 1 >= Length)
		{
			Pos = 0;
		}
		else
		{
			Pos++;
		}
		return ScheduleList[Pos];
	}

	template<class T>
	inline int FastSchedule<T>::gotoNextNullPos()
	{
		if (Length - Occupied == 0) return -1;
		for (int i = 1; i <= Length; i++)
		{
			gotoNextPos();
			if (currentElementIsNull())return Pos;
		}
		return 0xFFFFFFFF;
	}

	template<class T>
	inline T * FastSchedule<T>::getCurrentObject()
	{
		return ScheduleList[Pos];
	}

	template<class T>
	inline void FastSchedule<T>::clearCurrentObject()
	{
		if (ScheduleList[Pos])
		{
			delete ScheduleList[Pos];
			ScheduleList[Pos] = nullptr;
			Occupied--;
		}
	}

	template<class T>
	inline void FastSchedule<T>::clearAll()
	{
		for ((T*)& r : ScheduleList)
		{
			if (r) delete r;
			r = nullptr;
		}
	}

	template<class T>
	inline int FastSchedule<T>::takeOverObject(T* ptr)
	{
		if (full())
		{
			if (ptr) delete ptr;
			return -1;
		}
		if (!ptr) return Pos;
		Occupied++;
		ScheduleList[Pos] = ptr;
		return Pos;
	}

	template<class T>
	inline T* FastSchedule<T>::takeOutCurrentObject()
	{
		T* temp  = ScheduleList[Pos];
		ScheduleList[Pos] = nullptr;
		if(temp) Occupied--;
		return temp;
	}

	template<class T>
	inline void FastSchedule<T>::saveCurrentPos()
	{
		Save = Pos;
	}

	template<class T>
	inline void FastSchedule<T>::loadSavedPos()
	{
		Pos = Save;
	}
};