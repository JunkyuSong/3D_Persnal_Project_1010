#include "stdafx.h"
#include "Stage_Last.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"

CStage_Last::CStage_Last(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage_Last::CStage_Last(const CStage_Last & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage_Last::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage_Last::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Initialize(pArg);
	m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f, 0.1f, 45.f, 1.f));

	return S_OK;
}

void CStage_Last::Tick( _float fTimeDelta)
{
	ImGuiTick();

}

void CStage_Last::LateTick( _float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CStage_Last::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;



	SetUp_ShaderResources();

	_uint		iNumMeshes = m_pModelCom->Get_NumMesh();//메쉬 갯수를 알고 메쉬 갯수만큼 렌더를 할 것임. 여기서!

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

_bool CStage_Last::Picking(_float3 & _vPos)
{
	_bool			_bPick;
	_vector			vPickPos;
	if (_bPick = m_pModelCom->Picking(m_pTransformCom, vPickPos))
	{
		XMStoreFloat3(&_vPos, vPickPos);
	}
	return _bPick;
}

void CStage_Last::ImGuiTick()
{

}

HRESULT CStage_Last::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC	_Desc;
	_Desc.fRotationPerSec = XMConvertToRadians(90.f);
	//_Desc.fSpeedPerSec = 1.5f;
	_Desc.fSpeedPerSec =2.5f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &_Desc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	__super::Add_Component(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Stage_Last"), TEXT("Com_Model"), (CComponent**)&m_pModelCom);

	return S_OK;
}

HRESULT CStage_Last::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CStage_Last * CStage_Last::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage_Last*		pInstance = new CStage_Last(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage_Last"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage_Last::Clone(void * pArg)
{
	CStage_Last*		pInstance = new CStage_Last(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage_Last"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage_Last::Free()
{
	__super::Free();

	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

}