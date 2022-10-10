#include "..\Public\Level_Manager.h"
#include "Level.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager);

CLevel_Manager::CLevel_Manager()
	: m_iCurrentLevel(NULL)
	, m_iReserveLevel(NULL)
	, m_pCurrentLevel(nullptr)
	, m_pLoadingLevel(nullptr)
	, m_bLoading(false)
{
}

HRESULT CLevel_Manager::Check_Level()
{
	if (m_iCurrentLevel != m_iReserveLevel && m_iCurrentLevel != m_iLoadingLevel)
	{
		m_pLoadingLevel->Initialize(m_iReserveLevel);
		Open_Level(m_iLoadingLevel, m_pLoadingLevel);
	}

	return S_OK;
}

HRESULT CLevel_Manager::Loading_Level(const _uint & _iLoadingLevel, CLevel * _pLoadingLevel)
{
	if (_pLoadingLevel == nullptr)
	{
		return E_FAIL;
	}
	m_iLoadingLevel = _iLoadingLevel;
	m_pLoadingLevel = _pLoadingLevel;

	return S_OK;
}

HRESULT CLevel_Manager::Reserve_Level(const _uint & _iReserveLevel)
{
	//if(m_iReserveLevel)
	m_iReserveLevel = _iReserveLevel;

	return S_OK;
}

HRESULT CLevel_Manager::Open_Level(const _uint & _iLevelIndex, CLevel * _pNewLevel)
{
	if (nullptr != m_pCurrentLevel && m_iLoadingLevel != m_iCurrentLevel)
	{
		CAutoInstance<CGameInstance> _pGameInstance(CGameInstance::Get_Instance());
		
		
		_pGameInstance->Clear(m_iCurrentLevel);

	}
	if (/*_iLevelIndex != m_iLoadingLevel &&*/ _iLevelIndex != m_iReserveLevel)
	{
		Safe_Release(m_pCurrentLevel);
	}

	if (m_iCurrentLevel == m_iLoadingLevel)
	{
		m_bLoading = true;
	}
	else
	{
		m_bLoading = false;
	}

	m_pCurrentLevel = _pNewLevel;

	m_iCurrentLevel = _iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Tick(const _float & _fTimeDelta)
{
	Check_Level();
	m_pCurrentLevel->NowLevel(m_iReserveLevel);

	if (nullptr == m_pCurrentLevel)
	{
		MSG_BOX(TEXT("LevelMgr : CurrentLevel is Null"));
		return;
	}



	m_pCurrentLevel->Tick(_fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
	{
		MSG_BOX(TEXT("LevelMgr : CurrentLevel is Null"));
		return E_FAIL;
	}


	return m_pCurrentLevel->Render();
}

void CLevel_Manager::Free()
{
	Safe_Release<CLevel*>(m_pCurrentLevel);
	Safe_Release<CLevel*>(m_pLoadingLevel);
}
