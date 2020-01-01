#pragma once
#include <windows.h>
#include <new>
template <class T> class FastSchedule 
{
protected:
	T **ScheduleList;
	UINT32 Length,Pos,Occupied,Save;
	virtual BOOL Vacancy();
public:
	FastSchedule(UINT32 Len);
	virtual ~FastSchedule();
	T **GetScheduleList();
	UINT32 GetLength();
	T* GotoNextField();
	UINT32 GotoNextVacancy();
	T* GetCurrentField();
	void SetCurrentPos(UINT32 In);
	virtual void ClearCurrentField();
	void ClearAllField();
	virtual UINT32 TakeOverObject(T* pObject);
	void SaveCurrentPos();
	void LoadSavedPos();
};

template<class T>
inline BOOL FastSchedule<T>::Vacancy()
{
	if (ScheduleList[Pos]) return FALSE;
	else return TRUE;
}

template<class T>
inline FastSchedule<T>::FastSchedule(UINT32 Len)
{
	Length = Len;
	Pos = Length-1;
	Occupied = 0;
	Save = 0;
	try
	{
		ScheduleList = new T*[Length];
	}
	catch (std::bad_alloc &)
	{
		MessageBox(NULL, L"Not Enough Memory!", L"Fatal Error!", MB_ICONERROR);
		exit(0);
	}
	memset(ScheduleList, 0, sizeof(T*)*Length);
}

template<class T>
inline FastSchedule<T>::~FastSchedule()
{

	if (!ScheduleList)
	{
		ClearAllField();
		delete ScheduleList;
	}
	Length = 0;
	ScheduleList = NULL;
}

template<class T>
inline T ** FastSchedule<T>::GetScheduleList()
{
	return ScheduleList;
}

template<class T>
inline UINT32 FastSchedule<T>::GetLength()
{
	return Length;
}

template<class T>
inline T * FastSchedule<T>::GotoNextField()
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
inline UINT32 FastSchedule<T>::GotoNextVacancy()
{
	UINT32 i;
	if(Length - Occupied==0) return 0xFFFFFFFF;
	for (i = 1; i <= Length; i++)
	{
		GotoNextField();
		if (Vacancy())return Pos;
	}
	return 0xFFFFFFFF;
}

template<class T>
inline T * FastSchedule<T>::GetCurrentField()
{
	return ScheduleList[Pos];
}

template<class T>
inline void FastSchedule<T>::SetCurrentPos(UINT32 In)
{
	if (Length)
	{
		Pos = In % Length;
	}
}

template<class T>
inline void FastSchedule<T>::ClearCurrentField()
{
	if (ScheduleList[Pos])
	{
		delete ScheduleList[Pos];
		ScheduleList[Pos] = NULL;
		Occupied--;
	}
}

template<class T>
inline void FastSchedule<T>::ClearAllField()
{
	for (UINT i = 1; i <= Length; i++)
	{
		GotoNextField();
		if (Vacancy()) continue;
		ClearCurrentField();
		if (!Occupied) return;
	}
}

template<class T>
inline UINT32 FastSchedule<T>::TakeOverObject(T * pObject)
{

	if (GotoNextVacancy() == 0xFFFFFFFF)
	{
		//MessageBox(NULL, L"BUGBUG", L"BUGBUG", 0);
		if (pObject) delete pObject;
		return 0xFFFFFFFF;
	}
	if (!pObject) return Pos;
	Occupied++;
	ScheduleList[Pos] = pObject;
	return Pos;
}

template<class T>
inline void FastSchedule<T>::SaveCurrentPos()
{
	Save = Pos;
}

template<class T>
inline void FastSchedule<T>::LoadSavedPos()
{
	Pos = Save;
}
