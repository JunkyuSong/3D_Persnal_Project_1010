#include "stdafx.h"
#include "..\Public\Obj_Anim.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"

CObj_Anim::CObj_Anim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObj_Plus(pDevice, pContext)
{
}

CObj_Anim::CObj_Anim(const CObj_Anim & rhs)
	: CObj_Plus(rhs)
	, m_AnimPos(rhs.m_AnimPos)
	, m_PreAnimPos(rhs.m_PreAnimPos)
{
}

HRESULT CObj_Anim::Initialize_Prototype()
{
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	//m_pModelCom->Set_AnimationIndex(STATE_END);
	m_bAnimStop = true;
	return S_OK;
}

HRESULT CObj_Anim::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Initialize(pArg);
	/*if (m_pModelCom != nullptr)
	m_pModelCom->Set_AnimationIndex(STATE_END);*/
	m_fAnimSpeed = 1.f;
	return S_OK;
}

void CObj_Anim::Tick(_float fTimeDelta)
{
	//ImGuiTick();
	if (m_pModelCom != nullptr)
	{
		if (!m_bAnimStop)
		{

			CheckAnim();
			//CheckState();

			PlayAnimation(fTimeDelta);
			CheckState();
		}
		else
		{
			CheckAnim();
			_float4 _vAnim;
			XMStoreFloat4(&_vAnim, XMVectorSet(0.f, 0.f, 0.f, 1.f));
			if (m_pModelCom->Play_Animation(fTimeDelta, &_vAnim, &m_fPlayTime, m_bAgainAnim))
			{
				//CheckEndAnim(fTimeDelta);
			}
			XMStoreFloat4(&m_AnimPos, (XMLoadFloat4(&_vAnim) - XMLoadFloat4(&m_PreAnimPos)));
			m_PreAnimPos = _vAnim;
		}


	}
}

void CObj_Anim::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	if (m_pModelCom != nullptr)
	{

	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CObj_Anim::Render()
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

		//if (FAILED(m_pShaderCom->Begin(m_ePass)))
		//	return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, 0, i)))
			return E_FAIL;
	}


	return S_OK;
}

void CObj_Anim::PlayAnimation(_float fTimeDelta)
{
	if (m_bAnimStop)
		return;
	_float4 _vAnim;
	XMStoreFloat4(&_vAnim, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	if (m_pModelCom->Play_Animation(fTimeDelta, &_vAnim, &m_fPlayTime, m_bAgainAnim))
	{
		CheckEndAnim();
	}
	XMStoreFloat4(&m_AnimPos, (XMLoadFloat4(&_vAnim) - XMLoadFloat4(&m_PreAnimPos)));
	m_PreAnimPos = _vAnim;
}

void CObj_Anim::Set_Info(OBJ_DESC _tInfo)
{
	m_tInfo.eLevel = _tInfo.eLevel;

	if (m_pModelCom == nullptr)
	{
		lstrcpy(m_tInfo.szModelTag, _tInfo.szModelTag);
		__super::Add_Component(g_eCurLevel, m_tInfo.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom);

	}
	else if (lstrcmp(m_tInfo.szModelTag, _tInfo.szModelTag))
	{
		//Safe_Delete_Array(m_tInfo.szModelTag);
		Safe_Release(m_pModelCom);

		auto& iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(TEXT("Com_Model")));
		Safe_Release(iter->second);
		m_Components.erase(iter);

		//m_tInfo.szModelTag = _tInfo.szModelTag;
		lstrcpy(m_tInfo.szModelTag, _tInfo.szModelTag);
		__super::Add_Component(g_eCurLevel, m_tInfo.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom);
	}

	m_tInfo.matWorld = _tInfo.matWorld;
	m_pTransformCom->Set_WorldFloat4x4(m_tInfo.matWorld);
}

void CObj_Anim::ImGuiTick()
{

}

void CObj_Anim::CheckEndAnim()
{
	/*if (m_bLimit)
	{
	m_eCurState = m_eReserveState;
	m_eReserveState = STATE_END;
	Set_Anim(m_eCurState);
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	m_bLimit = false;
	return;
	}*/
	switch (m_eCurState)
	{
	case Client::CObj_Anim::STATE_ATT1:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_ATT2:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_ATT3:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_ATT4:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_ATT5:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_RUN_B:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_RUN_F:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_RUN_L:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_RUN_R:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_APPROACH:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_WALK:
		m_eCurState = STATE_ATT1;
		break;
	case Client::CObj_Anim::STATE_IDLE:
		break;
	case Client::CObj_Anim::STATE_AVOIDATTACK:
		m_eCurState = STATE_ATT1;
		break;
	default:
		break;
	}
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
}

void CObj_Anim::CheckState()
{
	switch (m_eCurState)
	{
	case Client::CObj_Anim::STATE_ATT1:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CObj_Anim::STATE_ATT2:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CObj_Anim::STATE_ATT3:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CObj_Anim::STATE_ATT4:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CObj_Anim::STATE_ATT5:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CObj_Anim::STATE_RUN_B:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CObj_Anim::STATE_RUN_F:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CObj_Anim::STATE_RUN_L:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CObj_Anim::STATE_RUN_R:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CObj_Anim::STATE_APPROACH:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CObj_Anim::STATE_WALK:
		m_fAnimSpeed = 1.5f;
		break;
	case Client::CObj_Anim::STATE_IDLE:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CObj_Anim::STATE_AVOIDATTACK:
		m_fAnimSpeed = 1.f;
		break;
	default:
		break;
	}

	//switch (m_eCurState)
	//{
	//case Client::CObj_Plus::STATE_ATT1:
	//	m_fAnimSpeed = 2.f;
	//	break;
	//case Client::CObj_Plus::STATE_ATT2:
	//	m_fAnimSpeed = 2.f;
	//	break;
	//case Client::CObj_Plus::STATE_ATT3:
	//	m_fAnimSpeed = 2.f;
	//	break;
	//case Client::CObj_Plus::STATE_ATT4:
	//	m_fAnimSpeed = 2.f;
	//	break;
	//case Client::CObj_Plus::STATE_ATT5:
	//	m_fAnimSpeed = 2.f;
	//	break;
	//case Client::CObj_Plus::STATE_APPROACH:
	//	m_fAnimSpeed = 1.f;
	//	break;
	//case Client::CObj_Plus::STATE_WALK:
	//	m_fAnimSpeed = 1.5f;
	//	break;
	//case Client::CObj_Plus::STATE_IDLE:
	//	m_fAnimSpeed = 1.f;
	//	break;
	//case Client::CObj_Plus::STATE_AVOIDATTACK:
	//	m_fAnimSpeed = 1.f;
	//	break;
	//default:
	//	break;
	//}
	Get_AnimMat();
}

void CObj_Anim::Set_Anim(STATE _eState)
{
	m_eCurState = _eState;
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	m_pModelCom->Set_AnimationIndex(m_eCurState);
}

void CObj_Anim::CheckAnim()
{
	if (m_ePreState != m_eCurState)
	{
		Set_Anim(m_eCurState);
		m_ePreState = m_eCurState;
	}
}

void CObj_Anim::Get_AnimMat()
{
	if (m_bAnimStop)
		return;
	_fmatrix _World = m_pTransformCom->Get_WorldMatrix();
	_vector _vPos;
	_vPos = XMVector3TransformCoord(XMLoadFloat4(&m_AnimPos), _World);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
}

HRESULT CObj_Anim::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;



	return S_OK;
}

HRESULT CObj_Anim::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;


	//const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;

	//if (LIGHTDESC::TYPE_DIRECTIONAL == pLightDesc->eType)
	//{
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//		return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;

	///*if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CObj_Anim * CObj_Anim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CObj_Anim*		pInstance = new CObj_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CObj_Anim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CObj_Anim::Clone(void * pArg)
{
	CObj_Anim*		pInstance = new CObj_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CObj_Anim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObj_Anim::Free()
{
	__super::Free();

	if (m_tInfo.szModelTag != nullptr)
	{
		//Safe_Delete_Array(m_tInfo.szModelTag);
	}
	Safe_Release(m_pModelCom);
}