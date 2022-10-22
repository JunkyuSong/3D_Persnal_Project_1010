#include "stdafx.h"
#include "..\Public\Card.h"
#include "GameInstance.h"

#include "Magician.h"
#include "Trail.h"

CCard::CCard(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CWeapon(pDevice, pContext)
{
}

CCard::CCard(const CCard & rhs)
	: CWeapon(rhs)
{
}

HRESULT CCard::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCard::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_WorldFloat4x4(
		static_cast<CTransform*>(pArg)->Get_WorldFloat4x4()
	);

	m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	m_pTransformCom->Turn_Angle(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.0f));
	
	_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) 
		+ XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))
		+ XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP))
			+ (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) *(-0.1f));
	
	_vPos.m128_f32[1] += 1.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
	XMStoreFloat3(&m_vOriginPos, _vPos);
	
	return S_OK;
}

void CCard::Tick(_float fTimeDelta)
{
	//m_pTrailCom->Tick(fTimeDelta, m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Go_Up(fTimeDelta*20.f);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_PLAYER_WEAPON, m_pColliderCom, this);
	//일정 거리 이상 가면 ㅃㅇ
	
	
	if (5.f < fabs(XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vOriginPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))))
	{
		
	}
}

void CCard::Tick(_float fTimeDelta, CGameObject * _pUser)
{
	//안씀
}

void CCard::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	//충돌처리
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCard::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	_uint		iNumMeshes = m_pModelCom->Get_NumMesh();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	//m_pTrailCom->Render();
#ifdef _DEBUG
	//if (nullptr != m_pColliderCom && m_bColliderOn)
		m_pColliderCom->Render();
#endif

	return S_OK;

}



HRESULT CCard::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC	_Desc;
	_Desc.fRotationPerSec = XMConvertToRadians(90.f);
	//_Desc.fSpeedPerSec = 1.5f;
	_Desc.fSpeedPerSec = 2.5f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &_Desc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Card"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Trail */
	//CTrail::TRAILINFO _tInfo;
	//
	//_tInfo._Color = _float4(100.f / 255.f, 216.f / 255.f, 201.f / 255.f, 1.f);
	//_tInfo._HighAndLow.vHigh = _float3(100.0f, 0.f, 0.f);
	//_tInfo._HighAndLow.vLow = _float3(90.0f, 0.f, 0.f);
	////_tInfo._HighAndLow.vLow = _float3(-5.f, 0.f, 0.f);
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Trail"), TEXT("Com_Trail"), (CComponent**)&m_pTrailCom, &_tInfo)))
	//{
	//	MSG_BOX(TEXT("fail to trail in Cane_Sword"));
	//	return E_FAIL;
	//}

	/* For.Com_OBB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(8.f, 16.f, 5.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

CCard * CCard::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCard*		pInstance = new CCard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCard::Clone(void * pArg)
{
	CCard*		pInstance = new CCard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCard::Free()
{
	__super::Free();
}
