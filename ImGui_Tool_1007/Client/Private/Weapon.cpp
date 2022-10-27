#include "stdafx.h"
#include "..\Public\Weapon.h"

#include "GameInstance.h"

#include "Trail.h"
#include "Transform.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void * pArg)
{
	return S_OK;
}

void CWeapon::Tick(_float fTimeDelta)
{
	
}

void CWeapon::Tick(_float fTimeDelta, CGameObject * _pUser)
{
}

void CWeapon::LateTick(_float fTimeDelta)
{
}

HRESULT CWeapon::Render()
{
	return S_OK;
}

HRESULT CWeapon::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTrailCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
}


_bool CWeapon::Trail_GetOn()
{
	return m_pTrailCom->Get_On();
}

void CWeapon::TrailOn()
{
	m_pTrailCom->TrailOn(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
}

void CWeapon::TrailOff()
{
	m_pTrailCom->TrailOff();
}
