#include "stdafx.h"
#include "..\Public\Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CClient_Camere(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs, CTransform::TRANSFORMDESC * pArg)
	: CClient_Camere(rhs, pArg)
{

}

HRESULT CCamera_Free::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f, 0.f, 45.f, 1.f));

	return S_OK;
}

void CCamera_Free::Tick(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	if (pGameInstance->KeyPressing(DIK_UP))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (pGameInstance->KeyPressing(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (pGameInstance->KeyPressing(DIK_LEFT))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (pGameInstance->KeyPressing(DIK_RIGHT))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	_long	MouseMove = 0;

	if ((MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))&& pGameInstance->MousePressing(DIMK_RBUTTON))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);		
	}

	if ((MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))&& pGameInstance->MousePressing(DIMK_RBUTTON))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
	}

	
	__super::Tick(fTimeDelta);



}

void CCamera_Free::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

CCamera_Free * CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free*		pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Free::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_Free*		pInstance = new CCamera_Free(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();


}
