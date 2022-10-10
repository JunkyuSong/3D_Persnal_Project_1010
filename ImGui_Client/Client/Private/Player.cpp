#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"

#include "HierarchyNode.h"

#include "MotionTrail.h"
#include "Saber.h"

#include "Weapon.h"


CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_ePass(PASS_NONPICK)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
	, m_ePass(rhs.m_ePass)
	, m_AnimPos(rhs.m_AnimPos)
	, m_PreAnimPos(rhs.m_PreAnimPos)
	, m_pSockets(rhs.m_pSockets)
	, m_pParts(rhs.m_pParts)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f,1.f));
	m_PreAnimPos = m_AnimPos;

	for (int i = 0; i < PART_END; ++i)
	{
		m_pSockets.push_back(nullptr);
		m_pParts.push_back(nullptr);
	}
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	Ready_AnimLimit();


	ZeroMemory(&m_tInfo, sizeof(OBJ_DESC));

	if (pArg)
	{
		OBJ_DESC _tInfo = *static_cast<OBJ_DESC*>(pArg);
		Set_Info(_tInfo);
	}
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Set_Player(this);

	Safe_Release(pGameInstance);

	m_fAnimSpeed = 1.f;

	for (int i = 0; i < 10; ++i)
	{
		//m_listMotion
		CMotionTrail* _Motion = CMotionTrail::Create(m_pDevice, m_pContext, m_pModelCom);
		if (_Motion == nullptr)
		{
			MSG_BOX(TEXT("creat Motion"));
			return E_FAIL;
		}
		m_listDeadMotion.push_back(_Motion);
	}

	KeySetting();

	/*CAutoInstance<CGameInstance> GameInstance(CGameInstance::Get_Instance());
	GameInstance->Set_TimeSpeed(TEXT("Timer_Main"), 2.f);*/
	
	return S_OK;
}

void CPlayer::Tick( _float fTimeDelta)
{
	if (m_pModelCom != nullptr)
	{
		if (!m_bAnimStop)
		{
			KeyInputMain(fTimeDelta);
			CheckAnim();
			//CheckState();
			AfterAnim();
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
			CheckLimit();
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
	for (auto& pCollider : m_pColliderCom)
	{
		if (nullptr != pCollider)
			pCollider->Update(m_pTransformCom->Get_WorldMatrix());
	}

	Check_MotionTrail(fTimeDelta);
}

void CPlayer::LateTick( _float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	for (auto& pPart : m_pParts)
	{
		if (pPart != nullptr)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, pPart);
	}
		

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;
	SetUp_ShaderResources();
	_uint		iNumMeshes;//메쉬 갯수를 알고 메쉬 갯수만큼 렌더를 할 것임. 여기서!
	for (auto& _motion : m_listMotion)
	{
		
		iNumMeshes = _motion->Get_NumMesh();
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(_motion->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(_motion->Render(m_pShaderCom, 1, i)))
				return E_FAIL;
		}
	}

	SetUp_ShaderResources();

	iNumMeshes = m_pModelCom->Get_NumMesh();//메쉬 갯수를 알고 메쉬 갯수만큼 렌더를 할 것임. 여기서!

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, m_ePass,i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	for (_uint i = 0; i < COLLILDERTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
#endif


	return S_OK;
}

void CPlayer::PlayAnimation( _float fTimeDelta)
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

void CPlayer::Set_Info(OBJ_DESC _tInfo)
{
	m_tInfo.eLevel = _tInfo.eLevel;

	if (m_pModelCom == nullptr)
	{
		m_tInfo.szModelTag = _tInfo.szModelTag;
		__super::Add_Component(LEVEL_GAMEPLAY, m_tInfo.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom);

	}
	else if (lstrcmp( m_tInfo.szModelTag, _tInfo.szModelTag))
	{
		Safe_Delete_Array(m_tInfo.szModelTag);
		Safe_Release(m_pModelCom);

		auto& iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(TEXT("Com_Model")));
		Safe_Release(iter->second);
		m_Components.erase(iter);

		m_tInfo.szModelTag = _tInfo.szModelTag;
		__super::Add_Component(LEVEL_GAMEPLAY, m_tInfo.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom);
	}
	m_pModelCom->Set_AnimationIndex(STATE_AVOIDATTACK);
	m_tInfo.matWorld = _tInfo.matWorld;
	m_pTransformCom->Set_WorldFloat4x4(m_tInfo.matWorld);
}


void CPlayer::KeySetting()
{
	//KeyInput.resize(STATE_END);
	KeyInput[STATE_ATT1] = &CPlayer::KP_ATT;
	KeyInput[STATE_ATT2] = &CPlayer::KP_ATT;
	KeyInput[STATE_ATT3] = &CPlayer::KP_ATT;
	KeyInput[STATE_ATT4] = &CPlayer::KP_ATT;
	KeyInput[STATE_ATT5] = &CPlayer::KP_ATT;
	KeyInput[STATE_IDLE] = &CPlayer::KeyInput_Idle;
	KeyInput[STATE_RUN_F] = &CPlayer::KeyInput_Idle;
	
}

void CPlayer::KeyInputMain( _float fTimeDelta)
{
	/*for (int i = 0; i< KeyInput[m_eCurState].size(); ++i)
		(this->*(KeyInput[m_eCurState][i]))(fTimeDelta);*/
	//KeyInput[m_eCurState](fTimeDelta);

	switch (m_eCurState)
	{
	case Client::CPlayer::STATE_ATT1:
		KP_ATT(fTimeDelta);
		break;
	case Client::CPlayer::STATE_ATT2:
		KP_ATT(fTimeDelta);
		break;
	case Client::CPlayer::STATE_ATT3:
		KP_ATT(fTimeDelta);
		break;
	case Client::CPlayer::STATE_ATT4:
		KP_ATT(fTimeDelta);
		break;
	case Client::CPlayer::STATE_ATT5:
		break;
	case Client::CPlayer::STATE_RUN_B:
		KeyInput_Idle(fTimeDelta);
		break;
	case Client::CPlayer::STATE_RUN_F:
		KeyInput_Idle(fTimeDelta);
		break;
	case Client::CPlayer::STATE_RUN_L:
		KeyInput_Idle(fTimeDelta);
		break;
	case Client::CPlayer::STATE_RUN_R:
		KeyInput_Idle(fTimeDelta);
		break;
	case Client::CPlayer::STATE_APPROACH:
		break;
	case Client::CPlayer::STATE_IDLE:
		KeyInput_Idle(fTimeDelta);
		break;
	case Client::CPlayer::STATE_WALK:
		KeyInput_Idle(fTimeDelta);
		break;
	case Client::CPlayer::STATE_AVOIDATTACK:
		KP_AVOIDATTACK(fTimeDelta);
		break;
	case Client::CPlayer::STATE_JUMPAVOID:
		
		break;
	case Client::CPlayer::STATE_AVOIDBACK:
		
		break;
	case Client::CPlayer::Corvus_PW_Axe:
		break;
	case Client::CPlayer::Tackle_F:
		break;
	case Client::CPlayer::ParryR:
		KP_Parry(fTimeDelta);
		break;
	case Client::CPlayer::ParryL:
		KP_Parry(fTimeDelta);
		break;
	case Client::CPlayer::DualKnife:
		break;
	case Client::CPlayer::GreatSword:
		break;
	case Client::CPlayer::PW_Halberds:
		break;
	case Client::CPlayer::PW_Hammer_A:
		break;
	case Client::CPlayer::PW_TwinSwords_1:
		break;
	case Client::CPlayer::PW_TwinSwords_2:
		break;
	case Client::CPlayer::PW_VargSword_A:
		break;
	case Client::CPlayer::PW_Bow_Start:
		break;
	case Client::CPlayer::PW_Bow_End:
		break;
	case Client::CPlayer::PW_Bow_B:
		break;
	case Client::CPlayer::PW_Bloodwhip:
		break;
	case Client::CPlayer::PW_CaneSword_SP01:
		break;
	case Client::CPlayer::PW_CaneSword_SP02:
		break;
	case Client::CPlayer::Healing_Little:
		break;
	case Client::CPlayer::Healing2_Blend:
		break;
	case Client::CPlayer::Raven_ClawCommonV2_ChargeStart:
		break;
	case Client::CPlayer::Raven_ClawLong_ChargeFull:
		break;
	case Client::CPlayer::Raven_ClawNear:
		KP_ClawNear(fTimeDelta);
		break;
	case Client::CPlayer::Strong_Jump:
		break;
	case Client::CPlayer::RavenAttack_Start:
		break;
	case Client::CPlayer::RavenAttack_End:
		break;
	case Client::CPlayer::SD_ParryToMob:
		break;
	case Client::CPlayer::SD_HurtIdle:
		break;
	case Client::CPlayer::SD_StrongHurt_Start:
		break;
	case Client::CPlayer::SD_StrongHurt_End:
		break;
	case Client::CPlayer::SD_Dead:
		break;
	case Client::CPlayer::STATE_END:
		break;
	default:
		//KeyInput_Idle(fTimeDelta);
		break;
	}
}

void CPlayer::KeyInput_Idle( _float fTimeDelta)
{
	Move(fTimeDelta);
	//if (CGameInstance::Get_Instance()->KeyDown(DIK_SPACE))
	if (CGameInstance::Get_Instance()->MouseDown(DIMK_LBUTTON))
	{
		m_eCurState = STATE_ATT1;
	}

	if (CGameInstance::Get_Instance()->KeyDown(DIK_F))
	{
		m_eCurState = ParryL;
	}

	if (CGameInstance::Get_Instance()->KeyDown(DIK_SPACE))
	{
		m_eCurState = STATE_AVOIDATTACK;
		if (CGameInstance::Get_Instance()->KeyPressing(DIK_S))
		{
			m_eCurState = STATE_AVOIDBACK;
		}
	}

	if (CGameInstance::Get_Instance()->MouseDown(DIMK_RBUTTON))
	{
		m_eCurState = Raven_ClawNear;
	}
}

void CPlayer::Move(_float fTimeDelta)
{
	if (CGameInstance::Get_Instance()->KeyPressing(DIK_A))
	{
		if (CGameInstance::Get_Instance()->KeyPressing(DIK_W))
		{
			m_eDir = DIR_FL;
		}
		else if (CGameInstance::Get_Instance()->KeyPressing(DIK_S))
		{
			m_eDir = DIR_BL;
		}
		else
		{
			m_eDir = DIR_L;
		}
	}
	else if (CGameInstance::Get_Instance()->KeyPressing(DIK_D))
	{
		if (CGameInstance::Get_Instance()->KeyPressing(DIK_W))
		{
			m_eDir = DIR_FR;
		}
		else if (CGameInstance::Get_Instance()->KeyPressing(DIK_S))
		{
			m_eDir = DIR_BR;
		}
		else
		{
			m_eDir = DIR_R;
		}
	}
	else
	{
		if (CGameInstance::Get_Instance()->KeyPressing(DIK_W))
		{
			m_eDir = DIR_F;
		}
		else if (CGameInstance::Get_Instance()->KeyPressing(DIK_S))
		{
			m_eDir = DIR_B;
		}
		else
		{
			m_eDir = DIR_END;
		}
	}

	_float4x4 _CamMatix;
	XMStoreFloat4x4(&_CamMatix, XMMatrixInverse(nullptr, CGameInstance::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));

	_vector	_vCamLook, _vCamRight, _vLook, _vRight;
	_float _fRatio = 0.8f;
	_vCamLook = XMLoadFloat4((_float4*)&_CamMatix.m[2][0]);
	_vCamRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), _vCamLook));
	_vCamLook = XMVector3Normalize(XMVector3Cross(_vCamRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	switch (m_eDir)
	{
	case Client::CPlayer::DIR_F:
		m_eCurState = STATE_RUN_F;

		m_pTransformCom->Turn(_vLook, _vCamLook, _fRatio);
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	case Client::CPlayer::DIR_B:
		m_eCurState = STATE_RUN_F;

		m_pTransformCom->Turn(_vLook, _vCamLook * (-1.f), _fRatio);
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	case Client::CPlayer::DIR_R:
		m_eCurState = STATE_RUN_F;

		m_pTransformCom->Turn(_vLook, _vCamRight, _fRatio);
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	case Client::CPlayer::DIR_L:
		m_eCurState = STATE_RUN_F;

		m_pTransformCom->Turn(_vLook, _vCamRight * (-1.f), _fRatio);
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	case Client::CPlayer::DIR_FR:
		m_eCurState = STATE_RUN_F;
		m_pTransformCom->Turn(_vLook, _vCamLook * (0.5f) + _vCamRight * 0.5f, _fRatio);
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	case Client::CPlayer::DIR_BR:
		m_eCurState = STATE_RUN_F;
		m_pTransformCom->Turn(_vLook, _vCamLook * (-0.5f) + _vCamRight * 0.5f, _fRatio);
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	case Client::CPlayer::DIR_FL:
		m_eCurState = STATE_RUN_F;
		m_pTransformCom->Turn(_vLook, _vCamLook * (0.5f) + _vCamRight * (-0.5f), _fRatio);
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	case Client::CPlayer::DIR_BL:
		m_eCurState = STATE_RUN_F;
		m_pTransformCom->Turn(_vLook, _vCamLook * (-0.5f) + _vCamRight * (-0.5f), _fRatio);
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	case Client::CPlayer::DIR_END:
		m_eCurState = STATE_IDLE;
		break;
	}
}

void CPlayer::KP_ATT(_float fTimeDelta)
{
	//1. 리미트 시간 전에 눌렀을때 반응하는 상태 : 다음 공격키(예약)
	//2. 리미트 시간 후에 눌렀을때 반응하는 상태 : 패리(즉각)
	//3. 아무때나 눌렀을때 반응하는 상태 : 클로(예약), 회피(즉각)
	switch (m_eCurState)
	{
	case Client::CPlayer::STATE_ATT1:
		if (m_fPlayTime <= m_vecLimitTime[STATE_ATT1][ATTACKLIMIT_CHANGE])
		{
			//if (CGameInstance::Get_Instance()->KeyDown(DIK_SPACE))
			if (CGameInstance::Get_Instance()->MouseDown(DIMK_LBUTTON))
			{
				m_eReserveState = STATE_ATT2;
			}
		}
		if (m_fPlayTime > m_vecLimitTime[STATE_ATT1][ATTACKLIMIT_CHANGE])
		{
			if (CGameInstance::Get_Instance()->KeyDown(DIK_F))
			{
				m_eCurState = ParryL;
			}
		}
		break;
	case Client::CPlayer::STATE_ATT2:
		if (m_fPlayTime <= m_vecLimitTime[STATE_ATT2][ATTACKLIMIT_CHANGE])
		{
			//if (CGameInstance::Get_Instance()->KeyDown(DIK_SPACE))
			if (CGameInstance::Get_Instance()->MouseDown(DIMK_LBUTTON))
			{
				m_eReserveState = STATE_ATT3;
			}
			
		}
		if (m_fPlayTime > m_vecLimitTime[STATE_ATT2][ATTACKLIMIT_CHANGE])
		{
			if (CGameInstance::Get_Instance()->KeyDown(DIK_F))
			{
				m_eReserveState = ParryL;
			}
			
		}
		break;
	case Client::CPlayer::STATE_ATT3:
		if (m_fPlayTime <= m_vecLimitTime[STATE_ATT3][ATTACKLIMIT_CHANGE])
		{
			//if (CGameInstance::Get_Instance()->KeyDown(DIK_SPACE))
			if (CGameInstance::Get_Instance()->MouseDown(DIMK_LBUTTON))
			{
				m_eReserveState = STATE_ATT4;
			}
		}
		if (m_fPlayTime > m_vecLimitTime[STATE_ATT3][ATTACKLIMIT_CHANGE])
		{
			if (CGameInstance::Get_Instance()->KeyDown(DIK_F))
			{
				m_eReserveState = ParryL;
			}
			
		}
		break;
	case Client::CPlayer::STATE_ATT4:
		if (m_fPlayTime <= m_vecLimitTime[STATE_ATT4][ATTACKLIMIT_CHANGE])
		{
			//if (CGameInstance::Get_Instance()->KeyDown(DIK_SPACE))
			if (CGameInstance::Get_Instance()->MouseDown(DIMK_LBUTTON))
			{
				m_eReserveState = STATE_ATT5;
			}
		}
		if (m_fPlayTime > m_vecLimitTime[STATE_ATT4][ATTACKLIMIT_CHANGE])
		{
			if (CGameInstance::Get_Instance()->KeyDown(DIK_F))
			{
				m_eReserveState = ParryL;
			}
		}
		break;
	case Client::CPlayer::STATE_ATT5:
		if (m_fPlayTime > m_vecLimitTime[STATE_ATT5][ATTACKLIMIT_CHANGE])
		{
			if (CGameInstance::Get_Instance()->KeyDown(DIK_F))
			{
				m_eReserveState = ParryL;
			}
		}
		break;
	}
	if (CGameInstance::Get_Instance()->MouseDown(DIMK_RBUTTON))
	{
		m_eReserveState = Raven_ClawNear;
	}
	if (CGameInstance::Get_Instance()->KeyDown(DIK_SPACE))
	{
		m_eCurState = STATE_AVOIDATTACK;
		if (CGameInstance::Get_Instance()->KeyPressing(DIK_S))
		{
			m_eCurState = STATE_AVOIDBACK;
		}
	}
}

void CPlayer::KP_Parry(_float fTimeDelta)
{
	
}

void CPlayer::KP_AVOIDATTACK(_float fTimeDelta)
{

}

void CPlayer::KP_ClawNear(_float fTimeDelta)
{
	if (CGameInstance::Get_Instance()->KeyDown(DIK_SPACE))
	{
		m_eCurState = STATE_AVOIDATTACK;
	}
}

void CPlayer::CheckEndAnim()
{
	switch (m_eCurState)
	{
	case Client::CPlayer::STATE_ATT1:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_ATT2:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_ATT3:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_ATT4:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_ATT5:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_RUN_B:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_RUN_F:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_RUN_L:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_RUN_R:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_APPROACH:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_WALK:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_IDLE:
		break;
	case Client::CPlayer::STATE_AVOIDATTACK:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_JUMPAVOID:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_AVOIDBACK:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::Corvus_PW_Axe:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::Tackle_F:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::ParryR:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::ParryL:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::DualKnife:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::GreatSword:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::PW_Halberds:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::PW_Hammer_A:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::PW_TwinSwords_1:
		m_eCurState = PW_TwinSwords_2;
		break;
	case Client::CPlayer::PW_TwinSwords_2:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::PW_VargSword_A:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::PW_Bow_Start:
		m_eCurState = PW_Bow_End;
		break;
	case Client::CPlayer::PW_Bow_End:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::PW_Bow_B:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::PW_Bloodwhip:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::PW_CaneSword_SP01:		
		m_eCurState = PW_CaneSword_SP02;
		break;
	case Client::CPlayer::PW_CaneSword_SP02:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::Healing_Little:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::Healing2_Blend:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::Raven_ClawCommonV2_ChargeStart:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::Raven_ClawLong_ChargeFull:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::Raven_ClawNear:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::Strong_Jump:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::RavenAttack_Start:
		m_eCurState = RavenAttack_End;
		break;
	case Client::CPlayer::RavenAttack_End:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::SD_ParryToMob:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::SD_HurtIdle:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::SD_StrongHurt_Start:
		m_eCurState = SD_StrongHurt_End;
		break;
	case Client::CPlayer::SD_StrongHurt_End:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::SD_Dead:
		m_eCurState = STATE_IDLE;
		break;
	default:
		break;
	}

	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
}

void CPlayer::CheckLimit()
{
	switch (m_eCurState)
	{
	case Client::CPlayer::STATE_ATT1:
		if (CheckLimit_Att(STATE_ATT1))
			return;
		break;
	case Client::CPlayer::STATE_ATT2:
		if (CheckLimit_Att(STATE_ATT2))
			return;
		break;
	case Client::CPlayer::STATE_ATT3:
		if (CheckLimit_Att(STATE_ATT3))
			return;
		break;
	case Client::CPlayer::STATE_ATT4:
		if (CheckLimit_Att(STATE_ATT4))
			return;
		break;
	case Client::CPlayer::STATE_ATT5:
		if (CheckLimit_Att(STATE_ATT5))
			return;
		break;
	case Client::CPlayer::STATE_RUN_B:
		break;
	case Client::CPlayer::STATE_RUN_F:
		break;
	case Client::CPlayer::STATE_RUN_L:
		break;
	case Client::CPlayer::STATE_RUN_R:
		break;
	case Client::CPlayer::STATE_APPROACH:
		break;
	case Client::CPlayer::STATE_IDLE:
		break;
	case Client::CPlayer::STATE_WALK:
		break;
	case Client::CPlayer::STATE_AVOIDATTACK:
		break;
	case Client::CPlayer::Corvus_PW_Axe:
		break;
	case Client::CPlayer::Tackle_F:
		break;
	case Client::CPlayer::ParryR:
		break;
	case Client::CPlayer::ParryL:
		break;
	case Client::CPlayer::DualKnife:
		break;
	case Client::CPlayer::GreatSword:
		break;
	case Client::CPlayer::PW_Halberds:
		break;
	case Client::CPlayer::PW_Hammer_A:
		break;
	case Client::CPlayer::PW_TwinSwords_1:
		break;
	case Client::CPlayer::PW_TwinSwords_2:
		break;
	case Client::CPlayer::PW_VargSword_A:
		break;
	case Client::CPlayer::PW_Bow_Start:
		break;
	case Client::CPlayer::PW_Bow_End:
		break;
	case Client::CPlayer::PW_Bow_B:
		break;
	case Client::CPlayer::PW_Bloodwhip:
		break;
	case Client::CPlayer::PW_CaneSword_SP01:
		break;
	case Client::CPlayer::PW_CaneSword_SP02:
		break;
	case Client::CPlayer::Healing_Little:
		break;
	case Client::CPlayer::Healing2_Blend:
		break;
	case Client::CPlayer::Raven_ClawCommonV2_ChargeStart:
		break;
	case Client::CPlayer::Raven_ClawLong_ChargeFull:
		break;
	case Client::CPlayer::Raven_ClawNear:
		break;
	case Client::CPlayer::Strong_Jump:
		break;
	case Client::CPlayer::RavenAttack_Start:
		break;
	case Client::CPlayer::RavenAttack_End:
		break;
	case Client::CPlayer::SD_ParryToMob:
		break;
	case Client::CPlayer::SD_HurtIdle:
		break;
	case Client::CPlayer::SD_StrongHurt_Start:
		break;
	case Client::CPlayer::SD_StrongHurt_End:
		break;
	case Client::CPlayer::SD_Dead:
		break;
	case Client::CPlayer::STATE_END:
		break;
	default:
		break;
	}

}

_bool CPlayer::CheckLimit_Att(STATE _eAtt)
{
	if (m_fPlayTime > m_vecLimitTime[_eAtt][ATTACKLIMIT_CHANGE])
	{
		if (m_eReserveState != STATE_END)
		{
			Change_Anim();
			return true;
		}
	}

	if (m_fPlayTime > m_vecLimitTime[_eAtt][ATTACKLIMIT_TRAILEND])
	{
		if (m_pParts[PART_SABER]->Trail_GetOn() == true)
			m_pParts[PART_SABER]->TrailOff();
	}
	else if (m_fPlayTime > m_vecLimitTime[_eAtt][ATTACKLIMIT_TRAILON])
	{
		//트레일 온
		if (m_pParts[PART_SABER]->Trail_GetOn() == false)
		{
			m_pParts[PART_SABER]->TrailOn();
		}
	}

	if (m_fPlayTime > m_vecLimitTime[_eAtt][ATTACKLIMIT_COLLIDEREND])
	{
		m_pParts[PART_SABER]->Set_CollisionOn(false);
	}
	else if (m_fPlayTime > m_vecLimitTime[_eAtt][ATTACKLIMIT_COLLIDERON])
	{
		m_pParts[PART_SABER]->Set_CollisionOn(true);
	}
	return false;
}

void CPlayer::Change_Anim()
{
	m_eCurState = m_eReserveState;
	m_eReserveState = STATE_END;
	Set_Anim(m_eCurState);
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
}

void CPlayer::CheckState()
{
	

	switch (m_eCurState)
	{
	case Client::CPlayer::STATE_ATT1:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_ATT2:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_ATT3:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_ATT4:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_ATT5:
		m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_RUN_B:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_RUN_F:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_RUN_L:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_RUN_R:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_APPROACH:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_WALK:
		m_fAnimSpeed = 1.5f;
		break;
	case Client::CPlayer::STATE_IDLE:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_AVOIDATTACK:
		m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::Corvus_PW_Axe:
		break;
	case Client::CPlayer::Tackle_F:
		break;
	case Client::CPlayer::ParryR:
		break;
	case Client::CPlayer::ParryL:
		break;
	case Client::CPlayer::DualKnife:
		break;
	case Client::CPlayer::GreatSword:
		break;
	case Client::CPlayer::PW_Halberds:
		break;
	case Client::CPlayer::PW_Hammer_A:
		break;
	case Client::CPlayer::PW_TwinSwords_1:
		break;
	case Client::CPlayer::PW_TwinSwords_2:
		break;
	case Client::CPlayer::PW_VargSword_A:
		break;
	case Client::CPlayer::PW_Bow_Start:
		break;
	case Client::CPlayer::PW_Bow_End:
		break;
	case Client::CPlayer::PW_Bow_B:
		break;
	case Client::CPlayer::PW_Bloodwhip:
		break;
	case Client::CPlayer::PW_CaneSword_SP01:
		break;
	case Client::CPlayer::PW_CaneSword_SP02:
		break;
	case Client::CPlayer::Healing_Little:
		break;
	case Client::CPlayer::Healing2_Blend:
		break;
	case Client::CPlayer::Raven_ClawCommonV2_ChargeStart:
		break;
	case Client::CPlayer::Raven_ClawLong_ChargeFull:
		break;
	case Client::CPlayer::Raven_ClawNear:
		break;
	case Client::CPlayer::Strong_Jump:
		break;
	case Client::CPlayer::RavenAttack_Start:
		break;
	case Client::CPlayer::RavenAttack_End:
		break;
	case Client::CPlayer::SD_ParryToMob:
		break;
	case Client::CPlayer::SD_HurtIdle:
		break;
	case Client::CPlayer::SD_StrongHurt_Start:
		break;
	case Client::CPlayer::SD_StrongHurt_End:
		break;
	case Client::CPlayer::SD_Dead:
		break;
	case Client::CPlayer::STATE_END:
		break;
	default:
		break;
	}
}

void CPlayer::AfterAnim()
{
	switch (m_eCurState)
	{
	case Client::CPlayer::STATE_ATT1:
		//m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_ATT2:
		//m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_ATT3:
		//m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_ATT4:
		//m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_ATT5:
		//m_fAnimSpeed = 2.f;
		break;
	case Client::CPlayer::STATE_RUN_B:
		//m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_RUN_F:
		//m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_RUN_L:
		//m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_RUN_R:
		//m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_APPROACH:
		//m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_WALK:
		//m_fAnimSpeed = 1.5f;
		break;
	case Client::CPlayer::STATE_IDLE:
		//m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_AVOIDATTACK:
		//m_fAnimSpeed = 1.f;
		break;
	case Client::CPlayer::STATE_JUMPAVOID:
		m_bMotionPlay = true;
		break;
	case Client::CPlayer::STATE_AVOIDBACK:
		m_bMotionPlay = true;
		break;
	case Client::CPlayer::Corvus_PW_Axe:
		break;
	case Client::CPlayer::Tackle_F:
		break;
	case Client::CPlayer::ParryR:
		break;
	case Client::CPlayer::ParryL:
		break;
	case Client::CPlayer::DualKnife:
		break;
	case Client::CPlayer::GreatSword:
		break;
	case Client::CPlayer::PW_Halberds:
		break;
	case Client::CPlayer::PW_Hammer_A:
		break;
	case Client::CPlayer::PW_TwinSwords_1:
break;
	case Client::CPlayer::PW_TwinSwords_2:
		break;
	case Client::CPlayer::PW_VargSword_A:
		break;
	case Client::CPlayer::PW_Bow_Start:
		break;
	case Client::CPlayer::PW_Bow_End:
		break;
	case Client::CPlayer::PW_Bow_B:
		break;
	case Client::CPlayer::PW_Bloodwhip:
		break;
	case Client::CPlayer::PW_CaneSword_SP01:
		break;
	case Client::CPlayer::PW_CaneSword_SP02:
		break;
	case Client::CPlayer::Healing_Little:
		break;
	case Client::CPlayer::Healing2_Blend:
		break;
	case Client::CPlayer::Raven_ClawCommonV2_ChargeStart:
		break;
	case Client::CPlayer::Raven_ClawLong_ChargeFull:
		break;
	case Client::CPlayer::Raven_ClawNear:
		break;
	case Client::CPlayer::Strong_Jump:
		break;
	case Client::CPlayer::RavenAttack_Start:
		break;
	case Client::CPlayer::RavenAttack_End:
		break;
	case Client::CPlayer::SD_ParryToMob:
		break;
	case Client::CPlayer::SD_HurtIdle:
		break;
	case Client::CPlayer::SD_StrongHurt_Start:
		break;
	case Client::CPlayer::SD_StrongHurt_End:
		break;
	case Client::CPlayer::SD_Dead:
		break;
	case Client::CPlayer::STATE_END:
		break;
	default:
		break;
	}
	Get_AnimMat();
}

void CPlayer::Set_Anim(STATE _eState)
{
	m_eCurState = _eState;
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	m_pModelCom->Set_AnimationIndex(m_eCurState);
	m_fPlayTime = 0.f;

	m_pParts[PART_SABER]->Set_CollisionOn(false);
	m_pParts[PART_SABER]->TrailOff();
	m_bMotionPlay = false;
}

void CPlayer::CheckAnim()
{
	if (m_ePreState != m_eCurState)
	{
		Set_Anim(m_eCurState);
		m_ePreState = m_eCurState;
	}
}

void CPlayer::Get_AnimMat()
{
	if (m_bAnimStop)
		return;
	_fmatrix _World = m_pTransformCom->Get_WorldMatrix();
	_vector _vPos;
	_vPos = XMVector3TransformCoord(XMLoadFloat4(&m_AnimPos), _World);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
}

HRESULT CPlayer::Check_MotionTrail(_float fTimeDelta)
{
	m_CurMotionTrail += fTimeDelta;
	for (auto iter = m_listMotion.begin(); iter != m_listMotion.end();)
	{
		(*iter)->Tick(fTimeDelta);
		if ((*iter)->Get_Tick() > 0.3f)
		{
			(*iter)->Set_Tick(0.f);
			CMotionTrail* _Motion = (*iter);
			iter = m_listMotion.erase(iter);
			m_listDeadMotion.push_back(_Motion);
		}
		else
			++iter;
	}
	if (!m_bMotionPlay)
	{
		return S_OK;
	}

	if (m_MaxMotionTrail < m_CurMotionTrail)
	{
		m_CurMotionTrail = 0.f;
		if (m_listDeadMotion.empty())
		{
			CMotionTrail* _Motion = CMotionTrail::Create(m_pDevice, m_pContext, m_pModelCom);
			if (_Motion == nullptr)
			{
				MSG_BOX(TEXT("creat Motion"));
				return E_FAIL;
			}
			_Motion->Set_CombinedMat(m_pModelCom->Get_HierarchyNodeVector(), m_pTransformCom->Get_WorldFloat4x4());
			m_listMotion.push_back(_Motion);
		}
		else
		{
			CMotionTrail* _Motion = m_listDeadMotion.back();
			_Motion->Set_CombinedMat(m_pModelCom->Get_HierarchyNodeVector(), m_pTransformCom->Get_WorldFloat4x4());
			m_listMotion.push_back(_Motion);
			m_listDeadMotion.pop_back();
		}
	}
	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC	_Desc;
	_Desc.fRotationPerSec = XMConvertToRadians(90.f);
	//_Desc.fSpeedPerSec = 1.5f;
	_Desc.fSpeedPerSec =4.f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &_Desc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CPlayer::Ready_AnimLimit()
{
	m_vecLimitTime[STATE_ATT1].push_back(30.f);
	m_vecLimitTime[STATE_ATT1].push_back(10.f);
	m_vecLimitTime[STATE_ATT1].push_back(25.f);
	m_vecLimitTime[STATE_ATT1].push_back(10.f);
	m_vecLimitTime[STATE_ATT1].push_back(25.f);

	m_vecLimitTime[STATE_ATT2].push_back(30.f);
	m_vecLimitTime[STATE_ATT2].push_back(10.f);
	m_vecLimitTime[STATE_ATT2].push_back(25.f);
	m_vecLimitTime[STATE_ATT2].push_back(10.f);
	m_vecLimitTime[STATE_ATT2].push_back(25.f);

	m_vecLimitTime[STATE_ATT3].push_back(30.f);
	m_vecLimitTime[STATE_ATT3].push_back(10.f);
	m_vecLimitTime[STATE_ATT3].push_back(25.f);
	m_vecLimitTime[STATE_ATT3].push_back(10.f);
	m_vecLimitTime[STATE_ATT3].push_back(25.f);

	m_vecLimitTime[STATE_ATT4].push_back(45.f);
	m_vecLimitTime[STATE_ATT4].push_back(10.f);
	m_vecLimitTime[STATE_ATT4].push_back(40.f);
	m_vecLimitTime[STATE_ATT4].push_back(10.f);
	m_vecLimitTime[STATE_ATT4].push_back(40.f);

	m_vecLimitTime[STATE_ATT5].push_back(55.f);
	m_vecLimitTime[STATE_ATT5].push_back(20.f);
	m_vecLimitTime[STATE_ATT5].push_back(50.f);
	m_vecLimitTime[STATE_ATT5].push_back(20.f);
	m_vecLimitTime[STATE_ATT5].push_back(50.f);

	return S_OK;
}

HRESULT CPlayer::Ready_Collider()
{/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.f, 2.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.3f, 1.3f, 1.3f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_OBB_MONSTER], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_SPHERE_ITEM], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	/*//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
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

HRESULT CPlayer::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("ik_hand_gun");
	if (nullptr == pWeaponSocket)
		return E_FAIL;
	m_pSockets[PART_SABER] = pWeaponSocket;

	pWeaponSocket = m_pModelCom->Get_HierarchyNode("ik_hand_l");
	if (nullptr == pWeaponSocket)
		return E_FAIL;
	m_pSockets[PART_DAGGER] = pWeaponSocket;

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerParts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Sword */
	CWeapon*		pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Saber")));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pParts[PART_SABER] = pGameObject;
	
	
	pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Dagger")));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pParts[PART_DAGGER] = pGameObject;

	return S_OK;
}

HRESULT CPlayer::Update_Weapon()
{
	if (nullptr == m_pSockets[PART_SABER])
		return E_FAIL;

	/* 행렬. */
	/*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
	* 뼈의 행렬(CombinedTransformation)
	* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/
	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Player */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	_matrix WeaponMatrix = /*m_pSockets[PART_SABER]->Get_OffSetMatrix()**/
			m_pSockets[PART_SABER]->Get_CombinedTransformation()
		 * XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())
		* m_pTransformCom->Get_WorldMatrix();

	m_pParts[PART_SABER]->SetUp_State(WeaponMatrix);


	if (nullptr == m_pSockets[PART_DAGGER])
		return E_FAIL;
	WeaponMatrix = /*m_pSockets[PART_SABER]->Get_OffSetMatrix()**/
		m_pSockets[PART_DAGGER]->Get_CombinedTransformation()
		* XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())
		* m_pTransformCom->Get_WorldMatrix();

	m_pParts[PART_DAGGER]->SetUp_State(WeaponMatrix);

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& _Motion : m_listMotion)
	{
		Safe_Release(_Motion);
	}
	
	for (auto& _Motion : m_listDeadMotion)
	{
		Safe_Release(_Motion);
	}


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
	for (auto& _pCollider : m_pColliderCom)
	{
		if (_pCollider)
			Safe_Release(_pCollider);
	}

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}