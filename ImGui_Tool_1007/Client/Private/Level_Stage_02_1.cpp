#include "stdafx.h"
#include "..\Public\Level_Stage_02_1.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "CameraMgr.h"
#include "UI_Mgr.h"

#include "Boss_Bat.h"


CLevel_Stage_02_1::CLevel_Stage_02_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_Client(pDevice, pContext)
{
}

HRESULT CLevel_Stage_02_1::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	/*if (FAILED(Ready_Lights()))
	return E_FAIL;*/

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	CCameraMgr::Get_Instance()->Initialize();

	return S_OK;
}

void CLevel_Stage_02_1::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_Stage_02_1::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	SetWindowText(g_hWnd, TEXT("Stage_2"));

	return S_OK;
}

HRESULT CLevel_Stage_02_1::Ready_Lights()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Stage_02_1::Ready_Layer_Camera(const _tchar * pLayerTag)
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

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_Free"), LEVEL_STAGE_02_1, pLayerTag, &CameraDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Stage_02_1::Ready_Layer_Player(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_STAGE_02_1, pLayerTag)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Stage_02_1::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_STAGE_02_1, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain"), LEVEL_STAGE_02_1, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Stage_02_1"), LEVEL_STAGE_02_1, TEXT("Layer_Stage"))))
		return E_FAIL;
	/*

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, pLayerTag)))
	return E_FAIL;
	*/

	return S_OK;
}

HRESULT CLevel_Stage_02_1::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Monster_Extra01"), LEVEL_STAGE_02_1, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Monster_Extra02"), LEVEL_STAGE_02_1, pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage_02_1::Ready_Layer_UI(const _tchar * pLayerTag)
{

	AUTOINSTANCE(CGameInstance, pGameInstance);
	/*for (_uint i = 0; i < 1; ++i)
	{
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_UI"), LEVEL_GAMEPLAY, pLayerTag)))
	return E_FAIL;

	}

	*/
	CGameObject* _pGameObj = nullptr;
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_UI_PlayerHpBar"), LEVEL_STAGE_02_1, pLayerTag, nullptr, &_pGameObj)))
		return E_FAIL;
	CUI_Mgr::Get_Instance()->Add_UI(TEXT("PLAYER_HP_BAR"), _pGameObj);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_UI_Targeting"), LEVEL_STAGE_02_1, pLayerTag, nullptr, &_pGameObj)))
		return E_FAIL;
	CUI_Mgr::Get_Instance()->Add_UI(TEXT("Targeting"), _pGameObj);


	return S_OK;
}

HRESULT CLevel_Stage_02_1::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	//for (_uint i = 0; i < 10; ++i)
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Effect"), LEVEL_GAMEPLAY, pLayerTag)))
	//		return E_FAIL;

	//}




	return S_OK;
}

CLevel_Stage_02_1 * CLevel_Stage_02_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage_02_1*		pInstance = new CLevel_Stage_02_1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Stage_02_1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage_02_1::Free()
{
	__super::Free();

}


