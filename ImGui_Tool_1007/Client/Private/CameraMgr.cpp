#include "stdafx.h"
#include "..\Public\CameraMgr.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Camera_Free.h"
#include "Camera_Player.h"

#include "PipeLine.h"

IMPLEMENT_SINGLETON(CCameraMgr)

CCameraMgr::CCameraMgr()
{
	for (auto& iter : m_pCameras)
		iter = nullptr;
}


CCameraMgr::~CCameraMgr()
{
}

void CCameraMgr::Initialize()
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

	//if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_Free"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
	m_pCameras[CAMERA_FREE] = static_cast<CCamera*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc));
	m_pCameras[CAMERA_PLAYER] = static_cast<CCamera*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc));

}

void CCameraMgr::Tick(_float fTimedelta)
{
	if (g_eCurLevel == LEVEL_LOADING)
		return;
	AUTOINSTANCE(CGameInstance, pInstance);
	if (pInstance->KeyDown(DIK_TAB))
	{
		if (m_pCameras[CAMERA_FREE] != nullptr && m_pCameras[CAMERA_PLAYER] != nullptr)
		{
			if (m_eCameraType == CAMERA_FREE)
				Change_Camera(CAMERA_PLAYER);
			else
				Change_Camera(CAMERA_FREE);
		}	
		
	}
	if (m_pCameras[m_eCameraType])
		m_pCameras[m_eCameraType]->Tick(fTimedelta);

	//CPipeLine::Get_Instance()->Update();
}

void CCameraMgr::Change_Camera(CAMERATYPE _eCameraType)
{
	_float4x4 matCamera;
	XMStoreFloat4x4( &matCamera, m_pCameras[m_eCameraType]->Get_Transform()->Get_WorldMatrix());
	m_eCameraType = _eCameraType;
	m_pCameras[m_eCameraType]->Get_Transform()->Set_WorldFloat4x4(matCamera);
}

CCamera * CCameraMgr::Get_Cam(CAMERATYPE _eCameraType)
{
	return m_pCameras[_eCameraType];
}

void CCameraMgr::Free()
{
	for (auto& camera : m_pCameras)
		Safe_Release(camera);
}
