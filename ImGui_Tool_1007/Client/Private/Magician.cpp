#include "stdafx.h"
#include "..\Public\Magician.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"
#include "HierarchyNode.h"
#include "Weapon.h"

CMagician::CMagician(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CMagician::CMagician(const CMagician & rhs)
	: CMonster(rhs)
	, m_AnimPos(rhs.m_AnimPos)
	, m_PreAnimPos(rhs.m_PreAnimPos)
	, m_pSockets(rhs.m_pSockets)
	, m_pParts(rhs.m_pParts)
{
}

HRESULT CMagician::Initialize_Prototype()
{
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f,1.f));
	m_PreAnimPos = m_AnimPos;
	//m_pModelCom->Set_AnimationIndex(STATE_END);
	m_bAnimStop = true;
	for (int i = 0; i < PART_END; ++i)
	{
		m_pSockets.push_back(nullptr);
		m_pParts.push_back(nullptr);
	}
	return S_OK;
}

HRESULT CMagician::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;

	Ready_LimitTime();

	/*if (m_pModelCom != nullptr)
		m_pModelCom->Set_AnimationIndex(STATE_END);*/

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 5.f, 1.f));
	m_eCurState = SP_Att2_Start;
	return S_OK;
}

void CMagician::Tick( _float fTimeDelta)
{
	if (m_pModelCom != nullptr)
	{
		if (!m_bAnimStop)
		{
		
			CheckAnim();

			CheckState(fTimeDelta);
			PlayAnimation(fTimeDelta);

			
		}
		else
		{
			CheckAnim();
			_float4 _vAnim;
			XMStoreFloat4(&_vAnim, XMVectorSet(0.f, 0.f, 0.f, 1.f));
			if (m_pModelCom->Play_Animation(fTimeDelta, &_vAnim, &m_fPlayTime))
			{
				//CheckEndAnim(fTimeDelta);
			}
			XMStoreFloat4(&m_AnimPos, (XMLoadFloat4(&_vAnim) - XMLoadFloat4(&m_PreAnimPos)));
			m_PreAnimPos = _vAnim;
		}
	}
	Update_Weapon();
	for (auto& pPart : m_pParts)
	{
		if (pPart != nullptr)
			pPart->Tick(fTimeDelta);
	}
}

void CMagician::LateTick( _float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	if (m_pModelCom != nullptr)
	{
		
	}
	for (auto& pPart : m_pParts)
	{
		if (pPart != nullptr)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, pPart);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMagician::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, 0,i)))
			return E_FAIL;
	}


	return S_OK;
}

void CMagician::PlayAnimation( _float fTimeDelta)
{
	if (m_bAnimStop)
		return;
	_float4 _vAnim;
	XMStoreFloat4(&_vAnim, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	if (m_pModelCom->Play_Animation(fTimeDelta, &_vAnim, &m_fPlayTime))
	{
		CheckEndAnim();
	}
	CheckLimit();
	XMStoreFloat4(&m_AnimPos, (XMLoadFloat4(&_vAnim) - XMLoadFloat4(&m_PreAnimPos)));
	m_PreAnimPos = _vAnim;
}


void CMagician::CheckEndAnim()
{
	switch (m_eCurState)
	{
	case Client::CMagician::Magician_Idle:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Idle2:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Hurt_Short:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Hurt_Long:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Boss_Enter:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Cane_Att1:
		
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Cane_Att2:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::SP_Att1_Start:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::SP_Att1_Suc:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::SP_Att2_Start:
	{
		m_eCurState = SP_Att2_Suc;
		ChangeCanesword(CANESWORD_L); //패링 안되었을때??
	}
	{
		//m_eCurState = SP_Att2_Loop; 패링되었을때??
	}
		
		break;
	case Client::CMagician::SP_Att2_Loop:
		ChangeCanesword(CANESWORD_R);
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::SP_Att2_Suc:
		
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Appear_L:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Appear_R:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Appear_B:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Appear_F:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Cane_Att3:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Kick_Combo:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_B:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_F:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_L:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_R:
		m_eCurState = Magician_Idle;
		break;
	}
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
}

void CMagician::CheckState(_float fTimeDelta)
{
	//아이들 상태일때 플레이어쪽으로 회전
	// 
	switch (m_eCurState)
	{
	case Client::CMagician::Magician_Idle:
		break;
	case Client::CMagician::Magician_Idle2:
		break;
	case Client::CMagician::Hurt_Short:
		break;
	case Client::CMagician::Hurt_Long:
		break;
	case Client::CMagician::Boss_Enter:
		break;
	case Client::CMagician::Cane_Att1:
		
		break;
	case Client::CMagician::Cane_Att2:
		break;
	case Client::CMagician::SP_Att1_Start:
		break;
	case Client::CMagician::SP_Att1_Suc:
		break;
	case Client::CMagician::SP_Att2_Start:
		break;
	case Client::CMagician::SP_Att2_Loop:
		break;
	case Client::CMagician::SP_Att2_Suc:		
		break;
	case Client::CMagician::Appear_L:
		break;
	case Client::CMagician::Appear_R:
		break;
	case Client::CMagician::Appear_B:
		break;
	case Client::CMagician::Appear_F:
		break;
	case Client::CMagician::Cane_Att3:
		break;
	case Client::CMagician::Kick_Combo:
		break;
	case Client::CMagician::Walk_B:
		break;
	case Client::CMagician::Walk_F:
		break;
	case Client::CMagician::Walk_L:
		break;
	case Client::CMagician::Walk_R:
		break;
	}

	Get_AnimMat();
}

void CMagician::AnimTick(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CMagician::Magician_Idle:
		break;
	case Client::CMagician::Magician_Idle2:
		break;
	case Client::CMagician::Hurt_Short:
		break;
	case Client::CMagician::Hurt_Long:
		break;
	case Client::CMagician::Boss_Enter:
		break;
	case Client::CMagician::Cane_Att1:
		break;
	case Client::CMagician::Cane_Att2:
		break;
	case Client::CMagician::SP_Att1_Start:
		break;
	case Client::CMagician::SP_Att1_Suc:
		break;
	case Client::CMagician::SP_Att2_Start:
		break;
	case Client::CMagician::SP_Att2_Loop:
		break;
	case Client::CMagician::SP_Att2_Suc:
		break;
	case Client::CMagician::Appear_L:
		break;
	case Client::CMagician::Appear_R:
		break;
	case Client::CMagician::Appear_B:
		break;
	case Client::CMagician::Appear_F:
		break;
	case Client::CMagician::Cane_Att3:
		break;
	case Client::CMagician::Kick_Combo:
		break;
	case Client::CMagician::Walk_B:
		break;
	case Client::CMagician::Walk_F:
		break;
	case Client::CMagician::Walk_L:
		break;
	case Client::CMagician::Walk_R:
		break;
	case Client::CMagician::STATE_END:
		break;
	default:
		break;
	}
}

void CMagician::ChangeCanesword(CANESWORD _eCanesword)
{
	Safe_Release(m_pSockets[PART_CANESWORD]);
	m_pSockets[PART_CANESWORD] = m_pCanesword[_eCanesword];
	Safe_AddRef(m_pSockets[PART_CANESWORD]);
}

void CMagician::CheckLimit()
{
	switch (m_eCurState)
	{
	case Client::CMagician::Magician_Idle:
		break;
	case Client::CMagician::Magician_Idle2:
		break;
	case Client::CMagician::Hurt_Short:
		break;
	case Client::CMagician::Hurt_Long:
		break;
	case Client::CMagician::Boss_Enter:
		break;
	case Client::CMagician::Cane_Att1:
		break;
	case Client::CMagician::Cane_Att2:
		break;
	case Client::CMagician::SP_Att1_Start:
		break;
	case Client::CMagician::SP_Att1_Suc:
		break;
	case Client::CMagician::SP_Att2_Start:
		break;
	case Client::CMagician::SP_Att2_Loop:
		if (m_vecLimitTime[SP_Att2_Loop][1] < m_fPlayTime)
		{
			ChangeCanesword(CANESWORD_R);
		}
		else if (m_vecLimitTime[SP_Att2_Loop][0] < m_fPlayTime)
		{
			ChangeCanesword(CANESWORD_L);
		}
		break;
	case Client::CMagician::SP_Att2_Suc:
		if (m_vecLimitTime[SP_Att2_Suc][0] < m_fPlayTime)
		{
			ChangeCanesword(CANESWORD_R);
		}
		break;
	case Client::CMagician::Appear_L:
		break;
	case Client::CMagician::Appear_R:
		break;
	case Client::CMagician::Appear_B:
		break;
	case Client::CMagician::Appear_F:
		break;
	case Client::CMagician::Cane_Att3:
		break;
	case Client::CMagician::Kick_Combo:
		break;
	case Client::CMagician::Walk_B:
		break;
	case Client::CMagician::Walk_F:
		break;
	case Client::CMagician::Walk_L:
		break;
	case Client::CMagician::Walk_R:
		break;
	}
}

void CMagician::Set_Anim(STATE _eState)
{
	m_eCurState = _eState;
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	m_pModelCom->Set_AnimationIndex(m_eCurState);
}

void CMagician::CheckAnim()
{
	if (m_ePreState != m_eCurState)
	{
		Set_Anim(m_eCurState);
		m_ePreState = m_eCurState;
	}
}

void CMagician::Get_AnimMat()
{
	if (m_bAnimStop)
		return;
	_fmatrix _World = m_pTransformCom->Get_WorldMatrix();
	_vector _vPos;
	_vPos = XMVector3TransformCoord(XMLoadFloat4(&m_AnimPos), _World);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
}

HRESULT CMagician::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Magician"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CMagician::SetUp_ShaderResources()
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

void CMagician::Ready_LimitTime()
{
	m_vecLimitTime[SP_Att2_Loop].push_back(20.f);
	m_vecLimitTime[SP_Att2_Loop].push_back(150.f);
	m_vecLimitTime[SP_Att2_Suc].push_back(220.f);
}

CMagician * CMagician::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMagician*		pInstance = new CMagician(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMagician"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMonster * CMagician::Clone(void * pArg)
{
	CMagician*		pInstance = new CMagician(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMagician"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagician::Free()
{
	__super::Free();
	for (auto& _Socket : m_pSockets)
	{
		if (_Socket)
			Safe_Release(_Socket);
	}
	for (auto& _Part : m_pParts)
	{
		if (_Part)
			Safe_Release(_Part);
	}
	for (auto& _Socket : m_pCanesword)
	{
		if (_Socket)
			Safe_Release(_Socket);
	}
}


HRESULT CMagician::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;


	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("weapon_Cane");
	if (nullptr == pWeaponSocket)
		return E_FAIL;
	m_pSockets[PART_CANE] = pWeaponSocket;
	Safe_AddRef(m_pSockets[PART_CANE]);

	pWeaponSocket = m_pModelCom->Get_HierarchyNode("weapon_r_Sword");
	if (nullptr == pWeaponSocket)
		return E_FAIL;
	m_pSockets[PART_CANESWORD] = pWeaponSocket;
	Safe_AddRef(m_pSockets[PART_CANESWORD]);
	m_pCanesword.push_back(m_pSockets[PART_CANESWORD]);
	Safe_AddRef(m_pCanesword[CANESWORD_R]);

	pWeaponSocket = m_pModelCom->Get_HierarchyNode("weapon_l_Sword");
	if (nullptr == pWeaponSocket)
		return E_FAIL;
	m_pCanesword.push_back(pWeaponSocket);
	Safe_AddRef(m_pCanesword[CANESWORD_L]);

	return S_OK;
}

HRESULT CMagician::Ready_PlayerParts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Sword */
	CWeapon*		pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Cane")));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pParts[PART_CANE] = pGameObject;


	pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Cane_Sword")));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pParts[PART_CANESWORD] = pGameObject;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMagician::Update_Weapon()
{
	if (nullptr == m_pSockets[PART_CANE])
		return E_FAIL;

	/* 행렬. */
	/*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
	* 뼈의 행렬(CombinedTransformation)
	* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/
	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Player */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	_matrix WeaponMatrix = /*m_pSockets[PART_CANE]->Get_OffSetMatrix()**/
		m_pSockets[PART_CANE]->Get_CombinedTransformation()
		* XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())
		* m_pTransformCom->Get_WorldMatrix();

	m_pParts[PART_CANE]->SetUp_State(WeaponMatrix);


	if (nullptr == m_pSockets[PART_CANESWORD])
		return E_FAIL;
	WeaponMatrix = /*m_pSockets[PART_CANESWORD]->Get_OffSetMatrix()**/
		m_pSockets[PART_CANESWORD]->Get_CombinedTransformation()
		* XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())
		* m_pTransformCom->Get_WorldMatrix();

	m_pParts[PART_CANESWORD]->SetUp_State(WeaponMatrix);

	return S_OK;
}
