#include "stdafx.h"
#include "..\Public\UI_MonsterHpBar.h"
#include "GameInstance.h"

#include "Status.h"

CUI_MonsterHpBar::CUI_MonsterHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_MonsterHpBar::CUI_MonsterHpBar(const CUI_MonsterHpBar & rhs)
	: CGameObject(rhs)
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CUI_MonsterHpBar::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	return S_OK;
}

HRESULT CUI_MonsterHpBar::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fBarSize = 12.f;

	m_pTransCom[BAR_LEFTEDGE]->Set_Scale(XMVectorSet(30.f, m_fBarSize,0.f,1.f));
	m_pTransCom[BAR_LEFTEDGE]->Set_State(CTransform::STATE_POSITION, XMVectorSet(-232.2f, 285.4f,0.f,1.f));
	m_pTransCom[BAR_BACK]->Set_Scale(XMVectorSet(500.f, m_fBarSize, 0.f, 1.f));
	m_pTransCom[BAR_BACK]->Set_State(CTransform::STATE_POSITION, XMVectorSet(31.5f, 285.4f, 0.f, 1.f));
	m_pTransCom[BAR_RIGHTEDGE]->Set_Scale(XMVectorSet(-30.f, m_fBarSize, 0.f, 1.f));
	m_pTransCom[BAR_RIGHTEDGE]->Set_State(CTransform::STATE_POSITION, XMVectorSet(281.7f, 285.4f, 0.f, 1.f));
	m_pTransCom[BAR_HP]->Set_Scale(XMVectorSet(500.f, m_fBarSize - 4.f, 0.f, 1.f));
	m_pTransCom[BAR_HP]->Set_State(CTransform::STATE_POSITION, XMVectorSet(25.f, 285.4f, 0.f, 1.f));

	AUTOINSTANCE(CGameInstance, _pInstance);
	
	if (pArg)
	{	
		m_pTarget = static_cast<CStatus*>(static_cast<CGameObject*>(pArg)->Get_ComponentPtr(TEXT("Com_Status")));
		Safe_AddRef(m_pTarget);

		m_fPreHP = m_pTarget->Get_MaxHp();
		m_fMaxHP = m_pTarget->Get_MaxHp();
	}

	return S_OK;
}

void CUI_MonsterHpBar::Tick(_float fTimeDelta)
{

}

void CUI_MonsterHpBar::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_fPreHP != m_pTarget->Get_Hp())
	{
		//새로운 렉트 추가

		RECT _tRect;
		_tRect.vCenter.z = 0.f;
		_tRect.vCenter.y = 285.4f;
		
		_float _fStartPoint = 25.f - 250.f;
		_float _fCurHPBar = m_pTarget->Get_Hp() / m_fMaxHP  * 500.f;
		_float _fPreHPBar = m_fPreHP / m_fMaxHP  * 500.f;

		_tRect.vCenter.x = ((_fStartPoint + _fCurHPBar) +(_fStartPoint + _fPreHPBar)) * 0.5f;
		_tRect.fAlpha = 1.f;
		_tRect.fSize.x = _fPreHPBar - _fCurHPBar;
		_tRect.fSize.y = m_fBarSize - 4.f;
		m_tRects.push_back(_tRect);

		m_fPreHP = m_pTarget->Get_Hp();
	}

	//기존 렉트들 알파값 추가 및 Y값 커지기 -> 알파값이 0되면 그냥 사라짐

	for (list<RECT>::iterator& _tRect = m_tRects.begin(); _tRect != m_tRects.end(); )
	{
		_tRect->fSize.y += fTimeDelta * 30.f;
		_tRect->fAlpha -= fTimeDelta*1.25f;
		if (_tRect->fAlpha < 0.f)
		{
			_tRect = m_tRects.erase(_tRect);
		}
		else
		{
			++_tRect;
		}
	}


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_MonsterHpBar::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	for (int i = 0; i < BAR_END-2; ++i)
	{
		m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransCom[i]->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
		

		if (FAILED(m_pTextureCom[i]->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	_float _fHpPer = m_fPreHP / m_fMaxHP;
	

	m_pShaderCom->Set_RawValue("g_HpPer", &_fHpPer, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransCom[BAR_HP]->Get_WorldFloat4x4_TP(), sizeof(_float4x4));


	if (FAILED(m_pTextureCom[BAR_HP]->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom_Point->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom_Point->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom[BAR_DISAPPEAR]->Set_SRV(m_pShaderCom_Point, "g_DiffuseTexture")))
		return E_FAIL;

	for (auto& _tRect : m_tRects)
	{
		m_pShaderCom_Point->Set_RawValue("g_Center", &_tRect.vCenter, sizeof(_float3));
		m_pShaderCom_Point->Set_RawValue("g_Size", &_tRect.fSize, sizeof(_float2));
		m_pShaderCom_Point->Set_RawValue("g_Alpha", &_tRect.fAlpha, sizeof(_float));

		if (FAILED(m_pShaderCom_Point->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom_Point->Render()))
			return E_FAIL;
	}

	return S_OK;
}

void CUI_MonsterHpBar::Set_Target(CStatus * _pTarget)
{
	Safe_Release(m_pTarget);
	m_pTarget = _pTarget;
	Safe_AddRef(m_pTarget);
}

CUI_MonsterHpBar * CUI_MonsterHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_MonsterHpBar*		pInstance = new CUI_MonsterHpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Plus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_MonsterHpBar::Clone(void * pArg)
{
	CGameObject*		pInstance = new CUI_MonsterHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Plus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MonsterHpBar::Free()
{
	__super::Free();
	for (int i = 0; i < BAR_END; ++i)
	{
		Safe_Release(m_pTransCom[i]);
		Safe_Release(m_pTextureCom[i]);
		
	}
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderCom_Point);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom_Point);
	Safe_Release(m_pTarget);
}

HRESULT CUI_MonsterHpBar::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform_Leftedge"), (CComponent**)&m_pTransCom[BAR_LEFTEDGE])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform_Back"), (CComponent**)&m_pTransCom[BAR_BACK])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform_RightEdge"), (CComponent**)&m_pTransCom[BAR_RIGHTEDGE])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform_Hp"), (CComponent**)&m_pTransCom[BAR_HP])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform_Disappear"), (CComponent**)&m_pTransCom[BAR_DISAPPEAR])))
		return E_FAIL;


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Point"), TEXT("Com_Shader_Point"), (CComponent**)&m_pShaderCom_Point)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"), TEXT("Com_VIBuffer_Point"), (CComponent**)&m_pVIBufferCom_Point)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge"), TEXT("Com_Texture_Bar_Leftedge"), (CComponent**)&m_pTextureCom[BAR_LEFTEDGE])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge2"), TEXT("Com_Texture_Bar_Back"), (CComponent**)&m_pTextureCom[BAR_BACK])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge"), TEXT("Com_Texture_Bar_Rightedge"), (CComponent**)&m_pTextureCom[BAR_RIGHTEDGE])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Monster_Disappear"), TEXT("Com_Texture_Bar_Hp"), (CComponent**)&m_pTextureCom[BAR_HP])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Monster_Disappear"), TEXT("Com_Texture_Bar_Disappear"), (CComponent**)&m_pTextureCom[BAR_DISAPPEAR])))
		return E_FAIL;

	return S_OK;
}
