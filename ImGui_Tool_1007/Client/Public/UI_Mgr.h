#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Client)
class CUI_Mgr :
	public CBase
{
	DECLARE_SINGLETON(CUI_Mgr)
public:
	CUI_Mgr();
	virtual ~CUI_Mgr();

	virtual HRESULT Initialize(void* pArg);
	
	void Add_UI(_tchar* _pTag, CGameObject* _pUI) { m_UIs.emplace(_pTag, _pUI); Safe_AddRef(_pUI); }

	CGameObject*	Get_UI(_tchar* _pTag);

private:
	map<_tchar*, CGameObject*>	m_UIs;

	// CBase을(를) 통해 상속됨
public:
	virtual void Free() override;
};

END