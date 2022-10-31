#include "stdafx.h"
#include "..\Public\MoveCube.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

CMoveCube::CMoveCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMoveCube::CMoveCube(const CMoveCube & rhs)
	: CGameObject(rhs)
{
}


HRESULT CMoveCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoveCube::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	COLORCUBEDESC* pDesc = (COLORCUBEDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.f, 1.f });
	
	m_vRGB = pDesc->vColor;

	IG->AddMoveCube(this);


	return S_OK;
}

void CMoveCube::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Pressing(DIK_UP))
		m_pTransformCom->Go_Dir(_vector{ 0.f, 0.f, 1.f, 0.f }, 25.f, fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_DOWN))
		m_pTransformCom->Go_Dir(_vector{ 0.f, 0.f, -1.f, 0.f }, 25.f, fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_LEFT))
		m_pTransformCom->Go_Dir(_vector{ -1.f, 0.f, 0.f, 0.f }, 25.f, fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_RIGHT))
		m_pTransformCom->Go_Dir(_vector{ 1.f, 0.f, 0.f, 0.f }, 25.f, fTimeDelta);
	 if (pGameInstance->Key_Pressing(DIK_E))
		m_pTransformCom->Go_Dir(_vector{ 0.f, 1.f, 0.f, 0.f }, 25.f, fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_Q))
		m_pTransformCom->Go_Dir(_vector{ 0.f, -1.f, 0.f, 0.f }, 25.f, fTimeDelta);

	Safe_Release(pGameInstance);
}

void CMoveCube::LateTick(_float fTimeDelta)
{
	if (IG->GetToolCamera() && !IG->GetPlay())
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMoveCube::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}




_float3 CMoveCube::Get_Pos()
{
	_float3 vTempPos;
	XMStoreFloat3(&vTempPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return vTempPos;
}
void CMoveCube::Set_Pos(_float3 vPos)
{
	_vector vPosition = { vPos.x, vPos.y, vPos.z, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}



HRESULT CMoveCube::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For. Com_Shader*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Cube"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(TransformDesc));

	TransformDesc.fSpeedPerSec = 25.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMoveCube::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vRGB, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

CMoveCube * CMoveCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoveCube*		pInstance = new CMoveCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMoveCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMoveCube::Clone(void* pArg)
{
	CMoveCube*		pInstance = new CMoveCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMoveCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveCube::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

