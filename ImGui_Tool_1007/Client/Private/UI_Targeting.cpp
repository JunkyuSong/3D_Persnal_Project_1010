#include "stdafx.h"
#include "..\Public\UI_Targeting.h"
#include "GameInstance.h"

#include "Status.h"

CUI_Targeting::CUI_Targeting(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Targeting::CUI_Targeting(const CUI_Targeting & rhs)
	: CGameObject(rhs)

{
}

HRESULT CUI_Targeting::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Targeting::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	

	AUTOINSTANCE(CGameInstance, _pInstance);
	
	return S_OK;
}

void CUI_Targeting::Tick(_float fTimeDelta)
{
}

void CUI_Targeting::Targeting(_vector _vPlayerPos, _vector _vMonsterPos)
{
	//중간값 y : 플레이어와 몬스터 사이의 y값 + 1.5f로 하자
	//각 

	_vPlayerPos.m128_f32[1] += 1.5f;
	_vMonsterPos.m128_f32[1] += 1.5f;
	_vector _vMiddlePos = (_vPlayerPos + _vMonsterPos) * 0.5f ;
	_vMiddlePos.m128_f32[1] += 1.5f;

	_float	_fRatio = 0.f;
	_vector _vLook =  XMVector3Normalize(_vMonsterPos - _vPlayerPos);
	_vector _vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), _vLook));
	_vLook = XMVector3Normalize(XMVector3Cross(_vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	_vector _vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat3(&m_vRight, _vRight);
	XMStoreFloat3(&m_vUp, _vUp);


	for (_uint i = 0; i < 21; _fRatio += 0.05f, ++i)
	{
		XMStoreFloat3(&(m_vPos[i]), (1 - _fRatio)*(1 - _fRatio)*_vMonsterPos + 2 * (1 - _fRatio)*_fRatio*_vMiddlePos + _fRatio*_fRatio*_vPlayerPos);
	}

	// 10개 채워서 렌더링할때 2개씩 보내고
	// 그걸로 지오메트리 한다

	// 월드행렬 해줄 필요없다.
	// 월드행렬은 필요없지만, 플레이어에서 몬스터를 바라보는 방향벡터와 그에따른 라&업이 필요하다
	// 

	if (nullptr == m_pRendererCom)
		return;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}


void CUI_Targeting::LateTick(_float fTimeDelta)
{
	
}

HRESULT CUI_Targeting::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;
	AUTOINSTANCE(CGameInstance, pGameInstance);
		
	//m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_matLook)), sizeof(_float4x4));
	_float		_fWidth = 0.01f;

	m_pShaderCom->Set_RawValue("g_Right", &m_vRight, sizeof(_float3));
	m_pShaderCom->Set_RawValue("g_Up", &m_vUp, sizeof(_float3));
	m_pShaderCom->Set_RawValue("g_Width", &_fWidth, sizeof(_float));

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	/*if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;*/



	for (_uint i = 0; i < 19; ++i)
	{
		_float3 vPos[2];
		vPos[0] = m_vPos[i];
		vPos[1] = m_vPos[i+1];
		/*vPos[0] = { m_vPos[i].x ,m_vPos[i].y ,m_vPos[i].z ,1.f};
		vPos[1] = { m_vPos[i + 1] .x,m_vPos[i + 1].y,m_vPos[i + 1].z,1.f };*/
		m_pShaderCom->Set_RawValue("g_vPos_1", &(vPos[0]), sizeof(_float3));
		m_pShaderCom->Set_RawValue("g_vPos_2", &(vPos[1]), sizeof(_float3));
		//m_pShaderCom->Set_RawValue("g_PointList", vPos, sizeof(_float4) * 2);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	_float3 vPos[2];
	vPos[0] = m_vPos[19];
	vPos[1] = m_vPos[20];

	/*vPos[0] = { m_vPos[9].x ,m_vPos[9].y ,m_vPos[9].z ,1.f };
	vPos[1] = { m_vPos[9 + 1].x,m_vPos[9 + 1].y,m_vPos[9 + 1].z,1.f };
	m_pShaderCom->Set_RawValue("g_PointList", vPos, sizeof(_float4) * 2);*/

	m_pShaderCom->Set_RawValue("g_vPos_1", &(vPos[0]), sizeof(_float3));
	m_pShaderCom->Set_RawValue("g_vPos_2", &(vPos[1]), sizeof(_float3));

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CUI_Targeting * CUI_Targeting::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Targeting*		pInstance = new CUI_Targeting(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Plus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Targeting::Clone(void * pArg)
{
	CGameObject*		pInstance = new CUI_Targeting(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Plus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Targeting::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransCom);
}

HRESULT CUI_Targeting::Ready_Components()
{

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Point"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}
