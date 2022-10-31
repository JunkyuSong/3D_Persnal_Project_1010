#include "stdafx.h"
#include "..\Public\PosCube.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

CPosCube::CPosCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPosCube::CPosCube(const CPosCube & rhs)
	: CGameObject(rhs)
{
}


HRESULT CPosCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPosCube::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	POSCUBEDESC* pDesc = (POSCUBEDESC*)pArg;

	_vector vPos = XMLoadFloat3(&(pDesc->vPos));
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Scale(_vector{ 1.f,1.f,1.f,0.f });
	m_vRGB = pDesc->vColor;
	m_fCamSpeed = pDesc->fCamSpeed;
	m_fStopLimit = pDesc->fStopLimit;

	IG->AddPosCube(this);

	return S_OK;
}

void CPosCube::Tick(_float fTimeDelta)
{
	char Temp[30];
	_itoa_s(IG->GetPosIndex(), Temp, 10);
	if (m_sTag == Temp && IG->GetPosEdit())
		GetKeyInput(fTimeDelta);


}

void CPosCube::LateTick(_float fTimeDelta)
{
	if (IG->GetToolCamera() && !IG->GetPlay())
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPosCube::Render()
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




void CPosCube::GetKeyInput(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Pressing(DIK_NUMPAD5))
		m_pTransformCom->Go_Dir(_vector{ 0.f, 0.f, 1.f, 0.f }, 25.f, fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_NUMPAD2))
		m_pTransformCom->Go_Dir(_vector{ 0.f, 0.f, -1.f, 0.f }, 25.f, fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_NUMPAD1))
		m_pTransformCom->Go_Dir(_vector{ -1.f, 0.f, 0.f, 0.f }, 25.f, fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_NUMPAD3))
		m_pTransformCom->Go_Dir(_vector{ 1.f, 0.f, 0.f, 0.f }, 25.f, fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_NUMPAD4))
		m_pTransformCom->Go_Dir(_vector{ 0.f, 1.f, 0.f, 0.f }, 25.f, fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_NUMPAD6))
		m_pTransformCom->Go_Dir(_vector{ 0.f, -1.f, 0.f, 0.f }, 25.f, fTimeDelta);

	Safe_Release(pGameInstance);
}

_float3 CPosCube::Get_Pos()
{
	_float3 vTempPos;
	XMStoreFloat3(&vTempPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return vTempPos;
}
void CPosCube::Set_Pos(_float3 vPos)
{
	_vector temp = XMLoadFloat4(&_float4{ vPos.x,vPos.y,vPos.z,1.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp);
}

void CPosCube::Stop(_float fTimeDelta)
{
	m_fStopAcc += fTimeDelta * 1.f;
	if (m_fStopAcc >= m_fStopLimit)
	{
		m_bStop = false;
		m_fStopAcc = 0.f;
	}
}




HRESULT CPosCube::SetUp_Components()
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

HRESULT CPosCube::SetUp_ShaderResources()
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

CPosCube * CPosCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPosCube*		pInstance = new CPosCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPosCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPosCube::Clone(void* pArg)
{
	CPosCube*		pInstance = new CPosCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPosCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPosCube::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

