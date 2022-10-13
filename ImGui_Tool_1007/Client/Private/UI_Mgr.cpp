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

void CUI_Mgr::Free()
{
	for (auto& _pUI : m_UIs)
	{
		Safe_Release(_pUI.second);
	}
	m_UIs.clear();
}
