#include "stdafx.h"
#include "..\Public\Dummy.h"
#include "MotionTrail.h"
#include "GameInstance.h"


CDummy::CDummy(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy::CDummy(const CDummy & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy::Initialize_Prototype(CMotionTrail * _pModel, _float4x4 _ParentWorld)
{
	m_pModelCom = _pModel;

	

	CTransform::TRANSFORMDESC	_Desc;
	_Desc.fRotationPerSec = XMConvertToRadians(90.f);
	//_Desc.fSpeedPerSec = 1.5f;
	_Desc.fSpeedPerSec = 4.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransCom, &_Desc)))
		return E_FAIL;
	
	m_pTransCom->Set_WorldFloat4x4(_ParentWorld);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CDummy::Tick(_float fTimeDelta)
{
	m_pModelCom->Tick(fTimeDelta);
}

HRESULT CDummy::Render()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	_float	fAlpha = 1.f - m_pModelCom->Get_Tick() / 0.5f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMesh();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, 2, i)))
			return E_FAIL;
	}

	return E_NOTIMPL;
}

void CDummy::Set_World(_float4x4 _ParentWorld)
{
	m_pTransCom->Set_WorldFloat4x4(_ParentWorld);
}

_float CDummy::Get_Tick()
{
	return m_pModelCom->Get_Tick();
}

void CDummy::Set_Tick(_float _tick)
{
	m_pModelCom->Set_Tick(_tick);
}

void CDummy::Set_CombinedMat(vector<class CHierarchyNode*>* _vecParentBones, _float4x4 _pParentWorld)
{
	m_pTransCom->Set_WorldFloat4x4(_pParentWorld);
	m_pModelCom->Set_CombinedMat(_vecParentBones, _pParentWorld);
}

CGameObject * CDummy::Clone(void * pArg)
{
	return nullptr;
}

CDummy * CDummy::Creat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CMotionTrail * _pModel, _float4x4 _ParentWorld)
{
	CDummy*		pInstance = new CDummy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pModel, _ParentWorld)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransCom);
}
