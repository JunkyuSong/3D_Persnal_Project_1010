#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Stage_02_1.h"
#include "Level_Stage_02.h"
#include "Level_Stage_Lobby.h"
#include "Level_Stage_Last.h"

#include "Loader.h"

#include "CameraMgr.h"
#include "TerrainMgr.h"
#include "UI_Mgr.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_Client(pDevice, pContext)
{
}

//HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
//{
//	if (FAILED(__super::Initialize()))
//		return E_FAIL;
//
//	m_eNextLevel = eNextLevel;
//
//	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
//	if (nullptr == m_pLoader)
//		return E_FAIL;
//
//	return S_OK;
//}

HRESULT CLevel_Loading::Initialize(const _uint& _eNextLevel)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;
	//if (!m_bOne)
	//{
	//	if (FAILED(Ready_Layer()))
	//		return E_FAIL;
	//	m_bOne = true;
	//}

	CCameraMgr::Get_Instance()->Free();
	CTerrainMgr::Get_Instance()->Free();
	CUI_Mgr::Get_Instance()->Free();

	m_eNextLevel = (LEVEL)_eNextLevel;


	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		//m_LoadingMax = 21;
		break;

	case LEVEL_GAMEPLAY:
		//m_LoadingMax = 0;
		break;
	case LEVEL_STAGE_02:
		//pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_STAGE_LAST:
		//pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_STAGE_LOBBY:
		//pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_END:
		return S_OK;
	}

	m_pLoader = CLoader::Create(m_pDevice,m_pContext, (LEVEL)_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (true == m_pLoader->Get_Finished())
	{
		if (GetKeyState(VK_RETURN) & 0x8000)
		{

			AUTOINSTANCE(CGameInstance, pGameInstance);

			CLevel*		pNewLevel = nullptr;

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_GAMEPLAY:
				pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_STAGE_02_1:
				pNewLevel = CLevel_Stage_02_1::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_STAGE_02:
				pNewLevel = CLevel_Stage_02::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_STAGE_LAST:
				pNewLevel = CLevel_Stage_Last::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_STAGE_LOBBY:				
				pNewLevel = CLevel_Stage_Lobby::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pNewLevel || ((FAILED(pGameInstance->Open_Level(m_eNextLevel, pNewLevel))) || true))
			{
				Safe_Release(m_pLoader);
				return;
			}
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;

}


void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);

}


