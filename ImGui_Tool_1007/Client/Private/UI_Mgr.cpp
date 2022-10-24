#include "stdafx.h"
#include "..\Public\UI_Mgr.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUI_Mgr)

CUI_Mgr::CUI_Mgr()
{
}


CUI_Mgr::~CUI_Mgr()
{
}

HRESULT CUI_Mgr::Initialize(void * pArg)
{
	return S_OK;
}

CGameObject * CUI_Mgr::Get_UI(_tchar * _pTag)
{
	auto& iter = find_if(m_UIs.begin(), m_UIs.end(), CTag_Finder(_pTag));
	if (iter == m_UIs.end())
	{
		return nullptr;
	}
	return iter->second;
}

void CUI_Mgr::Free()
{
	for (auto& _pUI : m_UIs)
	{
		Safe_Release(_pUI.second);
	}
	m_UIs.clear();
}
