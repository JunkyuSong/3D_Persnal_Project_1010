#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CReleaseMgr :
	public CBase
{
	DECLARE_SINGLETON(CReleaseMgr)
private:
	CReleaseMgr();
	virtual ~CReleaseMgr() = default;

public:
	void				Add_Char(char* _szTemp) { m_Delete_Array_Char.push_back(_szTemp); }
	void				Add_Tchar(_tchar* _szTemp) { m_Delete_Array_Tchar.push_back(_szTemp); }
	virtual void Free() override;

private:
	list<char*>	m_Delete_Array_Char;
	list<_tchar*>	m_Delete_Array_Tchar;
	list<CGameObject*>	m_GameObj;
};

END