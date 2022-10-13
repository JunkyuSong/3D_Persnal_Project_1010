#include "stdafx.h"
#include "..\Public\UI_PlayerHpBar.h"
#include "GameInstance.h"

#include "Status.h"

CUI_PlayerHpBar::CUI_PlayerHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_PlayerHpBar::CUI_PlayerHpBar(const CUI_PlayerHpBar & rhs)
	: CGameObject(rhs)
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CUI_PlayerHpBar::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	return S_OK;
}

HRESULT CUI_PlayerHpBar::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pTransCom[BAR_LEFTEDGE]->Set_Scale(XMVectorSet(30.f, 15.f,0.f,1.f));
	m_pTransCom[BAR_LEFTEDGE]->Set_State(CTransform::STATE_POSITION, XMVectorSet(-555.7f, -261.f,0.f,1.f));
	m_pTransCom[BAR_BACK]->Set_Scale(XMVectorSet(300.f, 15.f, 0.f, 1.f));
	m_pTransCom[BAR_BACK]->Set_State(CTransform::STATE_POSITION, XMVectorSet(-400.f, -261.f, 0.f, 1.f));
	m_pTransCom[BAR_RIGHTEDGE]->Set_Scale(XMVectorSet(-30.f, 15.f, 0.f, 1.f));
	m_pTransCom[BAR_RIGHTEDGE]->Set_State(CTransform::STATE_POSITION, XMVectorSet(-245.7f, -261.f, 0.f, 1.f));
	m_pTransCom[BAR_HP]->Set_Scale(XMVectorSet(296.f, 9.f, 0.f, 1.f));
	m_pTransCom[BAR_HP]->Set_State(CTransform::STATE_POSITION, XMVectorSet(-400.f, -261.f, 0.f, 1.f));

	AUTOINSTANCE(CGameInstance, _pInstance);
	m_pTarget = static_cast<CStatus*>(_pInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Status")));
	Safe_AddRef(m_pTarget);
	
	m_fPreHP = m_pTarget->Get_MaxHp();
	m_fMaxHP = m_pTarget->Get_MaxHp();
	return S_OK;
}

void CUI_PlayerHpBar::Tick(_float fTimeDelta)
{

}

void CUI_PlayerHpBar::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_fPreHP > m_pTarget->Get_Hp())
	{
		m_fPreHP -= 10.f*fTimeDelta;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_PlayerHpBar::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	for (int i = 0; i < BAR_END-1; ++i)
	{
		m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransCom[i]->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
		

		if (FAILED(m_pTextureCom[i]->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	_float _fHpPer = m_pTarget->Get_Hp() / m_fMaxHP;
	_float _fPreHpPer = m_fPreHP / m_fMaxHP;

	m_pShaderCom->Set_RawValue("g_HpPer", &_fHpPer, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_PreHpPer", &_fPreHpPer, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransCom[BAR_HP]->Get_WorldFloat4x4_TP(), sizeof(_float4x4));


	if (FAILED(m_pTextureCom[BAR_HP]->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CUI_PlayerHpBar * CUI_PlayerHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_PlayerHpBar*		pInstance = new CUI_PlayerHpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Plus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_PlayerHpBar::Clone(void * pArg)
{
	CGameObject*		pInstance = new CUI_PlayerHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Plus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerHpBar::Free()
{
	__super::Free();
	for (int i = 0; i < BAR_END; ++i)
	{
		Safe_Release(m_pTransCom[i]);
		Safe_Release(m_pTextureCom[i]);
		
	}
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTarget);
}

HRESULT CUI_PlayerHpBar::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform_Leftedge"), (CComponent**)&m_pTransCom[BAR_LEFTEDGE])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform_Back"), (CComponent**)&m_pTransCom[BAR_BACK])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform__RightEdge"), (CComponent**)&m_pTransCom[BAR_RIGHTEDGE])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform_Hp"), (CComponent**)&m_pTransCom[BAR_HP])))
		return E_FAIL;


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge"), TEXT("Com_Texture_Bar_Leftedge"), (CComponent**)&m_pTextureCom[BAR_LEFTEDGE])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge2"), TEXT("Com_Texture_Bar_Back"), (CComponent**)&m_pTextureCom[BAR_BACK])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge"), TEXT("Com_Texture_Bar_Rightedge"), (CComponent**)&m_pTextureCom[BAR_RIGHTEDGE])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Hp"), TEXT("Com_Texture_Bar_Hp"), (CComponent**)&m_pTextureCom[BAR_HP])))
		return E_FAIL;

	return S_OK;
}
