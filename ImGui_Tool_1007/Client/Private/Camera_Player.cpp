#include "stdafx.h"
#include "..\Public\Camera_Player.h"
#include "GameInstance.h"
#include "Cell.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CClient_Camere(pDevice, pContext)
	, m_pPlayer(nullptr)
	, m_pTarget(nullptr)
	, m_fAngleY(0.f)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player & rhs, CTransform::TRANSFORMDESC * pArg)
	: CClient_Camere(rhs, pArg)
	, m_pPlayer(rhs.m_pPlayer)
	, m_vDis(rhs.m_vDis)
	, m_fAngleY(rhs.m_fAngleY)
	, m_pTarget(rhs.m_pTarget)
{

}

HRESULT CCamera_Player::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	XMStoreFloat3(&m_vDis, XMVectorSet(0.f, 0.f, -5.f, 1.f));
	return S_OK;
}

HRESULT CCamera_Player::Initialize(void * pArg)
{
	/*if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;*/
	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	AUTOINSTANCE(CGameInstance, pGameInstance);
	if (m_pPlayer == nullptr)
	{
		m_pPlayer = pGameInstance->Get_Player();
	}
	
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 1.f), XMConvertToRadians(60.f));
	m_fAngleY = XMConvertToRadians(60.f);

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;
	switch (g_eCurLevel)
	{
	case Client::LEVEL_GAMEPLAY:
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_GamePlay"), TEXT("Com_Navigation"), (CComponent**)&m_pNaviCom, &NaviDesc)))
			return E_FAIL;
		break;
	case Client::LEVEL_STAGE_02_1:
		if (FAILED(__super::Add_Component(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Navigation_Stage_02_1"), TEXT("Com_Navigation"), (CComponent**)&m_pNaviCom, &NaviDesc)))
			return E_FAIL;
		break;
	case Client::LEVEL_STAGE_02:
		if (FAILED(__super::Add_Component(LEVEL_STAGE_02, TEXT("Prototype_Component_Navigation_Stage_02"), TEXT("Com_Navigation"), (CComponent**)&m_pNaviCom, &NaviDesc)))
			return E_FAIL;
		break;
	case Client::LEVEL_STAGE_LAST:		
		if (FAILED(__super::Add_Component(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Navigation_Stage_Last"), TEXT("Com_Navigation"), (CComponent**)&m_pNaviCom, &NaviDesc)))
			return E_FAIL;
		break;
	case Client::LEVEL_STAGE_LOBBY:
		if (FAILED(__super::Add_Component(LEVEL_STAGE_LOBBY, TEXT("Prototype_Component_Navigation_Stage_Lobby"), TEXT("Com_Navigation"), (CComponent**)&m_pNaviCom, &NaviDesc)))
			return E_FAIL;
		break;
	}
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);
	_vector _vPos = static_cast<CTransform*>(m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE::STATE_POSITION);
	if (m_pTarget)
	{
		_vector _vTargetPos = m_pTarget->Get_State(CTransform::STATE::STATE_POSITION);
		
		_float	_fDis = fabs(XMVectorGetX(XMVector3Length(_vTargetPos - _vPos)));
		if (_fDis > 1.f)
			Targeting();
		//Check_Dis();
	}
	else
	{
		_long	MouseMove = 0;

		if ((MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))/* && CGameInstance::Get_Instance()->MousePressing(DIMK_RBUTTON)*/)
		{

			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);
		}

		if ((MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))/* && CGameInstance::Get_Instance()->MousePressing(DIMK_RBUTTON)*/)
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
			/*if (m_fAngleY <= XMConvertToDegrees(60.f) && m_fAngleY >= XMConvertToDegrees(-60.f))
			{
			m_fAngleY += MouseMove * fTimeDelta * 0.05f;
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
			}*/
		}
		//Check_Dis();
	}
	Check_Dis();
	_vector	vCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (m_pNaviCom->isMove(vCamPos, nullptr))
	{
		_float Y = m_pNaviCom->Get_PosY(vCamPos);
		if (Y + 0.5f   > vCamPos.m128_f32[1])
		{
			vCamPos.m128_f32[1] = m_pNaviCom->Get_PosY(vCamPos) + 0.5f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);
			
			//m_pTransformCom->LookAt(_vPos);
		}
	}
	
	
	__super::Tick(fTimeDelta);
}

void CCamera_Player::LateTick(_float fTimeDelta)
{
	
}

HRESULT CCamera_Player::Render()
{
	return S_OK;
}

void CCamera_Player::Check_Dis()
{
	if (m_pPlayer == nullptr)
	{
		MSG_BOX(TEXT("Camera : None Player"));
		return;
	}

	CTransform* _pTrans = static_cast<CTransform*>(m_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")));

	_vector _vPos = _pTrans->Get_State(CTransform::STATE_POSITION);

	_vector _vCamPos = XMLoadFloat4(&(m_CameraDesc.vAt));

	_float fDistance = fabs(XMVectorGetX(XMVector3Length(_vPos - _vCamPos)));

	_vector _vDir = XMVector3Normalize(_vPos - _vCamPos);

	if (fDistance > 0.7f)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			_vPos - (_vDir*0.65f));
		XMStoreFloat4(&(m_CameraDesc.vAt), _vPos - (_vDir*0.65f));
	}
	else if (fDistance > 0.03f)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			_vCamPos + (_vDir*0.025f));
		XMStoreFloat4(&(m_CameraDesc.vAt), _vCamPos + (_vDir*0.025f));
	}
	m_CameraDesc.vAt.y = _vPos.m128_f32[1] + 1.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&(m_CameraDesc.vAt)));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, (XMVector3TransformCoord(XMLoadFloat3(&m_vDis), m_pTransformCom->Get_WorldMatrix())));

	_vector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	POINT			ptMouse;
	ptMouse.x = g_iWinSizeX / 2;
	ptMouse.y = g_iWinSizeY / 2;
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CCamera_Player::Targeting()
{
	_vector _vCurLook = m_pTransformCom->Get_State(CTransform::STATE::STATE_LOOK);
	_vector _vNextLook = m_pTarget->Get_State(CTransform::STATE::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	//_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	
	_vector vLook, vRight, vUp;
	vLook = (XMVector3Normalize(_vCurLook) * 0.9f) + (XMVector3Normalize(_vNextLook)*(0.1f));
	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f,1.f,0.f,0.f), vLook));
	vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = m_pTransformCom->Get_Scale();

	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

CCamera_Player * CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Player*		pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Player::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_Player*		pInstance = new CCamera_Player(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	__super::Free();

	Safe_Release(m_pNaviCom);
}
