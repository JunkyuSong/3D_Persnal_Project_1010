#include "stdafx.h"
#include "..\Public\Level_Stage_Lobby.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "CameraMgr.h"
#include "UI_Mgr.h"


CLevel_Stage_Lobby::CLevel_Stage_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_Client(pDevice, pContext)
{
}

HRESULT CLevel_Stage_Lobby::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;

	if (FAILED(Ready_Lights()))
		return E_FAIL;


	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	CCameraMgr::Get_Instance()->Initialize();

	return S_OK;
}

void CLevel_Stage_Lobby::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_Stage_Lobby::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));

	return S_OK;
}

HRESULT CLevel_Stage_Lobby::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage_Lobby::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.0f;

	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_Free"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Stage_Lobby::Ready_Layer_Player(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Stage_Lobby::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Stage_01"), LEVEL_GAMEPLAY, TEXT("Layer_Stage"))))
		return E_FAIL;
/*

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;
*/


	return S_OK;
}

HRESULT CLevel_Stage_Lobby::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	//for (_uint i = 0; i < 3; ++i)
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Monster"), LEVEL_GAMEPLAY, pLayerTag)))
	//		return E_FAIL;

	//}

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Magician"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Stage_Lobby::Ready_Layer_UI(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	/*for (_uint i = 0; i < 1; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_UI"), LEVEL_GAMEPLAY, pLayerTag)))
			return E_FAIL;

	}

*/
	CGameObject* _pGameObj = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_UI_PlayerHpBar"), LEVEL_GAMEPLAY, pLayerTag, nullptr, &_pGameObj)))
		return E_FAIL;
	CUI_Mgr::Get_Instance()->Add_UI(TEXT("PLAYER_HP_BAR"), _pGameObj);




	return S_OK;
}

HRESULT CLevel_Stage_Lobby::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	//for (_uint i = 0; i < 10; ++i)
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Effect"), LEVEL_GAMEPLAY, pLayerTag)))
	//		return E_FAIL;

	//}




	return S_OK;
}

CLevel_Stage_Lobby * CLevel_Stage_Lobby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage_Lobby*		pInstance = new CLevel_Stage_Lobby(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Stage_Lobby"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage_Lobby::Free()
{
	__super::Free();

}


