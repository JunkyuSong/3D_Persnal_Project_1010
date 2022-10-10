#pragma once

#include "Base.h"

template<typename T>
class CAutoInstance
{
public:
	CAutoInstance(T* _Instance)
		: m_Instance(_Instance)
	{
		Safe_AddRef(m_Instance);
	}

	~CAutoInstance()
	{
		Safe_Release(m_Instance);
	}

	inline T* operator->() { return m_Instance; }

private:
	T* m_Instance;
};

#define	AUTOINSTANCE(T, N)	CAutoInstance<T> N(T::Get_Instance());