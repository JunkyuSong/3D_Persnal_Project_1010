#include "stdafx.h"
#include "..\Public\Obj_Plus.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"

CObj_Plus::CObj_Plus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_ePass(PASS_NONPICK)
{
}

CObj_Plus::CObj_Plus(const CObj_Plus & rhs)
	: CGameObject(rhs)
	, m_ePass(rhs.m_ePass)
{
}

HRESULT CObj_Plus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CObj_Plus::Initialize(void * pArg)
{
	ZeroMemory(&m_tInfo, sizeof(OBJ_DESC));

	if (pArg)
	{
		OBJ_DESC _tInfo = *static_cast<OBJ_DESC*>(pArg);
		Set_Info(_tInfo);
	}
	return S_OK;
}

void CObj_Plus::ImGuiTick()
{
}

void CObj_Plus::Free()
{
	__super::Free();

	if (m_tInfo.szModelTag != nullptr)
	{
		//Safe_Delete_Array(m_tInfo.szModelTag);
	}
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}