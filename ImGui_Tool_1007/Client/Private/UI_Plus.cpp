#include "stdafx.h"
#include "..\Public\UI_Plus.h"
#include "GameInstance.h"

CUI_Plus::CUI_Plus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_tInfo, sizeof(m_tInfo));
}

CUI_Plus::CUI_Plus(const CUI_Plus & rhs)
	: CGameObject(rhs)
	, m_ePass(PASS_NONPICK)
{
	ZeroMemory(&m_tInfo, sizeof(m_tInfo));
}

HRESULT CUI_Plus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Plus::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg == nullptr)
	{
		MSG_BOX(TEXT("Not UI_DESC"));
		return E_FAIL;
	}
	
	UI_DESC _tUI_DESC = *static_cast<UI_DESC*>(pArg);

	Set_Info(_tUI_DESC);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	return S_OK;
}

void CUI_Plus::Tick(_float fTimeDelta)
{
	//m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX/* - g_iWinSizeX * 0.5f*/, -m_fY/* + g_iWinSizeY * 0.5f*/, 0.0f, 1.f));

	/*POINT		ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);

	RECT		rcUI;
	SetRect(&rcUI, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	if (PtInRect(&rcUI, ptMouse))
	{
		MSG_BOX(TEXT("충돌"));
	}*/
}

void CUI_Plus::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Plus::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;



	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_ePass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Plus::Set_Info(UI_DESC _tInfo)
{
	if (m_tInfo.szUIName == nullptr)
	{
		m_tInfo.szUIName = _tInfo.szUIName;
	}
	else
	{
		//Safe_Delete_Array(_tInfo.szUIName);
	}

	if (lstrcmp(m_tInfo.szTextureTag, _tInfo.szTextureTag))
	{
		Safe_Delete_Array(m_tInfo.szTextureTag);
		Safe_Release(m_pTextureCom);
		
		
		auto& iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(TEXT("Com_Texture")));
		if (iter != m_Components.end())
		{
			Safe_Release(iter->second);
			m_Components.erase(iter);
		}
		
		
		m_tInfo.szTextureTag = _tInfo.szTextureTag;
		__super::Add_Component(LEVEL_STATIC, m_tInfo.szTextureTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom);
	}

	//if (m_pTextureCom != nullptr)
	//{
	//	
	//}
	m_tInfo.vPos = _tInfo.vPos;
	m_tInfo.vScale = _tInfo.vScale;

	m_fX = _tInfo.vPos.x;
	m_fY = _tInfo.vPos.y;
	//포지션 받을때 1.f... ㅅㅂ
	_vector _vTemp = XMLoadFloat3(&(_tInfo.vPos));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_tInfo.vPos.x, _tInfo.vPos.y, _tInfo.vPos.z, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(_tInfo.vScale.x, _tInfo.vScale.y, 1.f, 0.f));
	

}

HRESULT CUI_Plus::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
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

	return S_OK;
}

CUI_Plus * CUI_Plus::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Plus*		pInstance = new CUI_Plus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Plus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Plus::Clone(void * pArg)
{
	CUI_Plus*		pInstance = new CUI_Plus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Plus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Plus::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	Safe_Delete_Array(m_tInfo.szTextureTag);
	Safe_Delete_Array(m_tInfo.szUIName);
}
