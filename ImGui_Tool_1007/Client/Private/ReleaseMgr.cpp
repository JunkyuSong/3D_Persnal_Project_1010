#include "stdafx.h"
#include "..\Public\ReleaseMgr.h"

IMPLEMENT_SINGLETON(CReleaseMgr)

CReleaseMgr::CReleaseMgr()
{
}

void CReleaseMgr::Free()
{
	for (auto& iter : m_Delete_Array_Char)
		Safe_Delete_Array(iter);
	for (auto& iter : m_Delete_Array_Tchar)
		Safe_Delete_Array(iter);
	for (auto& iter : m_GameObj)
		Safe_Release(iter);
}
