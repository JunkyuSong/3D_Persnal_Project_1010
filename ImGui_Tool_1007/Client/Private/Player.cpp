#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"
#include "HierarchyNode.h"

#include "Status.h"

#include "MotionTrail.h"
#include "Dummy.h"
#include "Weapon.h"

#include "Layer.h"

#include "Monster.h"

#include "CameraMgr.h"
#include "Camera_Player.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_ePass(PASS_NONPICK)
{
	for (auto& _pHand : m_pHands)
		_pHand = nullptr;
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
	, m_ePass(rhs.m_ePass)
	, m_AnimPos(rhs.m_AnimPos)
	, m_PreAnimPos(rhs.m_PreAnimPos)
{
	for (auto& _pHand : m_pHands)
		_pHand = nullptr;
}

HRESULT CPlayer::Initialize_Prototype()
{
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f,1.f));
	m_PreAnimPos = m_AnimPos;


	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{

	AUTOINSTANCE(CGameInstance, pGameInstance);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Hands()))
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


	pGameInstance->Set_Player(this);

	

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
		CDummy*		_pDummy = CDummy::Creat(m_pDevice, m_pContext, _Motion, _float4x4());
		m_listDeadMotion.push_back(_pDummy);
	}

	KeySetting();


	m_MonsterLayer = pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
	
	

	return S_OK;
}

void CPlayer::Tick( _float fTimeDelta)
{
	ImGuiTick();
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

	

	Update_Weapon(fTimeDelta);

	Update_Collider();

	Check_MotionTrail(fTimeDelta);
}

void CPlayer::LateTick( _float fTimeDelta)
{
	if (Collision(fTimeDelta))
	{
		CheckAnim();
		AfterAnim();
		PlayAnimation(fTimeDelta);
	}
	
	

	Add_Render();
}

HRESULT CPlayer::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	_uint		iNumMeshes;//메쉬 갯수를 알고 메쉬 갯수만큼 렌더를 할 것임. 여기서!
	
	SetUp_ShaderResources();

	iNumMeshes = m_pModelCom->Get_NumMesh();//메쉬 갯수를 알고 메쉬 갯수만큼 렌더를 할 것임. 여기서!

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, 0,i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	//for (_uint i = 0; i < COLLILDERTYPE_END; ++i)
	//{
	//	if (nullptr != m_pColliderCom[i])
	//		m_pColliderCom[i]->Render();
	//}
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

void CPlayer::ImGuiTick()
{
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
	case Client::CPlayer::STATE_RUN_BL:
		KeyInput_Idle(fTimeDelta);
		break;
	case Client::CPlayer::STATE_RUN_BR:
		KeyInput_Idle(fTimeDelta);
		break;
	case Client::CPlayer::STATE_RUN_FL:
		KeyInput_Idle(fTimeDelta);
		break;
	case Client::CPlayer::STATE_RUN_FR:
		KeyInput_Idle(fTimeDelta);
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
	if (m_pTarget)
	{
		TargetingMove(fTimeDelta);
	}
	else
	{
		Move(fTimeDelta);
	}
	
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

	if (CGameInstance::Get_Instance()->KeyDown(DIK_R))
	{
		m_eCurState = Healing_Little;
	}
	if (CGameInstance::Get_Instance()->KeyDown(DIK_1))
	{
		m_eCurState = DualKnife;
		m_eWeapon = WEAPON::WEAPON_SKILL;
		m_eCurSkill = SKILL_DUAL;
	}
	if (CGameInstance::Get_Instance()->KeyDown(DIK_2))
	{
		
		m_eCurState = Corvus_PW_Axe;
	}
	if (CGameInstance::Get_Instance()->MouseDown(DIMK_WHEEL))
	{
		Targeting();
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

void CPlayer::TargetingMove(_float fTimeDelta)
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
		m_eCurState = STATE_RUN_B;

		m_pTransformCom->Turn(_vLook, _vCamLook, _fRatio);
		m_pTransformCom->Go_Straight(-fTimeDelta);
		break;
	case Client::CPlayer::DIR_R:
		m_eCurState = STATE_RUN_R;

		m_pTransformCom->Turn(_vLook, _vCamLook, _fRatio);
		m_pTransformCom->Go_Right(fTimeDelta);
		break;
	case Client::CPlayer::DIR_L:
		m_eCurState = STATE_RUN_L;

		m_pTransformCom->Turn(_vLook, _vCamLook, _fRatio);
		m_pTransformCom->Go_Right(-fTimeDelta);
		break;
	case Client::CPlayer::DIR_FR:
		m_eCurState = STATE_RUN_FR;
		m_pTransformCom->Turn(_vLook, _vCamLook, _fRatio);
		m_pTransformCom->Go_Straight((fTimeDelta) / (sqrtf(2.f)));
		m_pTransformCom->Go_Right((fTimeDelta) / (sqrtf(2.f)));
		break;
	case Client::CPlayer::DIR_BR:
		m_eCurState = STATE_RUN_BR;
		m_pTransformCom->Turn(_vLook, _vCamLook, _fRatio);
		m_pTransformCom->Go_Straight((-fTimeDelta) / (sqrtf(2.f)));
		m_pTransformCom->Go_Right((fTimeDelta) / (sqrtf(2.f)));
		break;
	case Client::CPlayer::DIR_FL:
		m_eCurState = STATE_RUN_FL;
		m_pTransformCom->Turn(_vLook, _vCamLook, _fRatio);
		m_pTransformCom->Go_Straight((fTimeDelta) / (sqrtf(2.f)));
		m_pTransformCom->Go_Right((-fTimeDelta) / (sqrtf(2.f)));
		break;
	case Client::CPlayer::DIR_BL:
		m_eCurState = STATE_RUN_BL;
		m_pTransformCom->Turn(_vLook, _vCamLook, _fRatio);
		m_pTransformCom->Go_Straight((-fTimeDelta) / (sqrtf(2.f)));
		m_pTransformCom->Go_Right((-fTimeDelta) / (sqrtf(2.f)));
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

void CPlayer::Targeting()
{
	/*if (m_pTarget != nullptr)
	{
	_vector _vTargetPos = static_cast<CTransform*>(m_pTarget->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt_ForLandObject(_vTargetPos);
	}*/
	if (m_pTarget == nullptr)
	{
		list<CGameObject*> Monsters = *m_MonsterLayer->Get_ListFromLayer();

		_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float _fClosedDis(15.f);
		for (auto iter : Monsters)
		{
			_float _fDis = fabs(XMVectorGetX(XMVector3Length(static_cast<CTransform*>(iter->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION)
				- _vPos)));
			if (_fDis < 15.f)
			{
				if (_fClosedDis > _fDis)
				{
					m_pTarget = iter;
					_fClosedDis = _fDis;
				}
			}
		}
		if (m_pTarget != nullptr)
		{
			Safe_AddRef(m_pTarget);
			static_cast<CCamera_Player*>(CCameraMgr::Get_Instance()->Get_Cam(CCameraMgr::CAMERA_PLAYER))->Get_Target(static_cast<CTransform*>(m_pTarget->Get_ComponentPtr(TEXT("Com_Transform"))));
		}
	}
	else
	{
		Safe_Release(m_pTarget);
		static_cast<CCamera_Player*>(CCameraMgr::Get_Instance()->Get_Cam(CCameraMgr::CAMERA_PLAYER))->Get_Target(nullptr);
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
		m_bCollision[COLLIDERTYPE_BODY] = true;
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_JUMPAVOID:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_AVOIDBACK:
		m_bCollision[COLLIDERTYPE_BODY] = true;
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::Corvus_PW_Axe:
		m_bCollision[COLLIDERTYPE_BODY] = true;
		m_eCurState = STATE_IDLE;
		m_bMotionPlay = false;
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
		m_eWeapon = WEAPON_BASE;
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
		m_eWeapon = WEAPON_BASE;
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
	case Client::CPlayer::STATE_RUN_BL:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_RUN_BR:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_RUN_FL:
		m_eCurState = STATE_IDLE;
		break;
	case Client::CPlayer::STATE_RUN_FR:
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
		if (m_fPlayTime > m_vecLimitTime[Corvus_PW_Axe][4])//다시 무기 스왑 및 타이머 정상화
		{
			m_eWeapon = WEAPON_BASE;
			m_eCurSkill = SKILL_END;
			AUTOINSTANCE(CGameInstance, pGame);
			pGame->Set_TimeSpeed(TEXT("Timer_Main"), 1.f);
		}
		else if (m_fPlayTime > m_vecLimitTime[Corvus_PW_Axe][3])//다시 무기 스왑 및 타이머 정상화
		{
			AUTOINSTANCE(CGameInstance, pGame);
			pGame->Set_TimeSpeed(TEXT("Timer_Main"), 2.f);
		}
		else if (m_fPlayTime > m_vecLimitTime[Corvus_PW_Axe][2])//모션트레일 off
		{
			m_pSkillParts[SKILL_AXE][0]->Set_CollisionOn(false);
			m_bMotionPlay = false;
		}
		else if (m_fPlayTime > m_vecLimitTime[Corvus_PW_Axe][1])//타이머 및 모션트레일
		{
			m_pSkillParts[SKILL_AXE][0]->Set_CollisionOn(true);
			m_bMotionPlay = true;
			AUTOINSTANCE(CGameInstance, pGame);
			pGame->Set_TimeSpeed(TEXT("Timer_Main"), 0.5f);
		}
		else if (m_fPlayTime > m_vecLimitTime[Corvus_PW_Axe][0])//무기 스왑
		{
			m_eCurSkill = SKILL_AXE;
			m_eWeapon = WEAPON_SKILL;
		}
		break;
	case Client::CPlayer::Tackle_F:
		break;
	case Client::CPlayer::ParryR:
		if (m_fPlayTime > m_vecLimitTime[ParryL][1])//무기 스왑
		{
			m_bCollision[COLLIDERTYPE_PARRY] = false;
			//m_pBaseParts[BASE_DAGGER]->Set_CollisionOn(false);
		}
		else if (m_fPlayTime > m_vecLimitTime[ParryL][0])//무기 스왑
		{
			m_bCollision[COLLIDERTYPE_PARRY] = true;
			//m_pBaseParts[BASE_DAGGER]->Set_CollisionOn(true);
		}
		break;
	case Client::CPlayer::ParryL:
		if (m_fPlayTime > m_vecLimitTime[ParryL][1])//무기 스왑
		{
			m_bCollision[COLLIDERTYPE_PARRY] = false;
			//m_pBaseParts[BASE_DAGGER]->Set_CollisionOn(false);
		}
		else if (m_fPlayTime > m_vecLimitTime[ParryL][0])//무기 스왑
		{
			m_bCollision[COLLIDERTYPE_PARRY] = true;
			//m_pBaseParts[BASE_DAGGER]->Set_CollisionOn(true);
		}
		break;
	case Client::CPlayer::DualKnife:
		if (m_fPlayTime > m_vecLimitTime[DualKnife][1])//무기 스왑
		{
			m_pSkillParts[SKILL_DUAL][HAND_LEFT]->Set_CollisionOn(false);
			m_pSkillParts[SKILL_DUAL][HAND_RIGHT]->Set_CollisionOn(false);
			if(m_pSkillParts[SKILL_DUAL][HAND_LEFT]->Trail_GetOn())
				m_pSkillParts[SKILL_DUAL][HAND_LEFT]->TrailOff();
			if (m_pSkillParts[SKILL_DUAL][HAND_RIGHT]->Trail_GetOn())
				m_pSkillParts[SKILL_DUAL][HAND_RIGHT]->TrailOff();
		}
		else if (m_fPlayTime > m_vecLimitTime[DualKnife][0])
		{
			if (!m_pSkillParts[SKILL_DUAL][HAND_LEFT]->Trail_GetOn())
				m_pSkillParts[SKILL_DUAL][HAND_LEFT]->TrailOn();
			if (!m_pSkillParts[SKILL_DUAL][HAND_RIGHT]->Trail_GetOn())
				m_pSkillParts[SKILL_DUAL][HAND_RIGHT]->TrailOn();
			m_pSkillParts[SKILL_DUAL][HAND_LEFT]->Set_CollisionOn(true);
			m_pSkillParts[SKILL_DUAL][HAND_RIGHT]->Set_CollisionOn(true);
		}
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
		if (m_fPlayTime > m_vecLimitTime[Raven_ClawNear][1])
		{
			m_bCollision[COLLIDERTYPE_CLAW] = false;
		}
		else if (m_fPlayTime > m_vecLimitTime[Raven_ClawNear][0])
		{
			m_bCollision[COLLIDERTYPE_CLAW] = true;
		}
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
		if (m_fPlayTime > 5.f)
		{
			m_bCollision[COLLIDERTYPE_BODY] = true;
			//m_eWeapon = WEAPON_BASE;
		}
		else if (m_fPlayTime > 0.f)
		{
			//m_eWeapon = WEAPON_NONE;
		}
		break;
	case Client::CPlayer::SD_StrongHurt_Start:
		if (m_fPlayTime > 5.f)
		{
			m_bCollision[COLLIDERTYPE_BODY] = true;
			//m_eWeapon = WEAPON_BASE;
		}
		else if (m_fPlayTime > 0.f)
		{
			//m_eWeapon = WEAPON_NONE;
		}
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
		if (m_pBaseParts[BASE_SABER]->Trail_GetOn() == true && m_eReserveState == STATE_END)
		{
			m_pBaseParts[BASE_SABER]->TrailOff();
		}
	}
	else if (m_fPlayTime > m_vecLimitTime[_eAtt][ATTACKLIMIT_TRAILON])
	{
		//트레일 온
		if (m_pBaseParts[BASE_SABER]->Trail_GetOn() == false)
		{
			m_pBaseParts[BASE_SABER]->TrailOn();
		}
	}

	if (m_fPlayTime > m_vecLimitTime[_eAtt][ATTACKLIMIT_COLLIDEREND])
	{
		m_pBaseParts[BASE_SABER]->Set_CollisionOn(false);
	}
	else if (m_fPlayTime > m_vecLimitTime[_eAtt][ATTACKLIMIT_COLLIDERON])
	{
		m_pBaseParts[BASE_SABER]->Set_CollisionOn(true);
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

void CPlayer::AfterAnim()
{
	switch (m_eCurState)
	{
	case Client::CPlayer::STATE_ATT1:
		//CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::GAMEOBJ_TYPE::TYPE_PLAYER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
		m_bCollision[COLLIDERTYPE_BODY] = true;
		break;
	case Client::CPlayer::STATE_ATT2:
		//CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::GAMEOBJ_TYPE::TYPE_PLAYER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
		m_bCollision[COLLIDERTYPE_BODY] = true;
		break;
	case Client::CPlayer::STATE_ATT3:
		m_bCollision[COLLIDERTYPE_BODY] = true;
		break;
	case Client::CPlayer::STATE_ATT4:
		m_bCollision[COLLIDERTYPE_BODY] = true;
		break;
	case Client::CPlayer::STATE_ATT5:
		m_bCollision[COLLIDERTYPE_BODY] = true;
		break;
	case Client::CPlayer::STATE_RUN_B:
		break;
	case Client::CPlayer::STATE_RUN_F:
		m_bCollision[COLLIDERTYPE_BODY] = true;
		break;
	case Client::CPlayer::STATE_RUN_L:
		break;
	case Client::CPlayer::STATE_RUN_R:
		break;
	case Client::CPlayer::STATE_APPROACH:
		break;
	case Client::CPlayer::STATE_WALK:
		break;
	case Client::CPlayer::STATE_IDLE:
		m_bCollision[COLLIDERTYPE_BODY] = true;
		break;
	case Client::CPlayer::STATE_AVOIDATTACK:
		if (m_pBaseParts[BASE_SABER]->Trail_GetOn())
			m_pBaseParts[BASE_SABER]->TrailOff();
		m_bCollision[COLLIDERTYPE_BODY] = false;
		break;
	case Client::CPlayer::STATE_JUMPAVOID:
		m_bMotionPlay = true;
		break;
	case Client::CPlayer::STATE_AVOIDBACK:
		m_bMotionPlay = true;
		if (m_pBaseParts[BASE_SABER]->Trail_GetOn())
			m_pBaseParts[BASE_SABER]->TrailOff();
		m_bCollision[COLLIDERTYPE_BODY] = false;
		break;
	case Client::CPlayer::Corvus_PW_Axe:
		break;
	case Client::CPlayer::Tackle_F:
		break;
	case Client::CPlayer::ParryR:
		m_bCollision[COLLIDERTYPE_BODY] = true;
		
		break;
	case Client::CPlayer::ParryL:
		m_bCollision[COLLIDERTYPE_BODY] = true;
		
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
		m_eWeapon = WEAPON_NONE;
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

	m_pBaseParts[BASE_SABER]->Set_CollisionOn(false);
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

void CPlayer::Cancle()
{
	//무기들 전부 끄고, 근데 스킬 중엔 충돌 자체가 없을거고, 패링이 없을텐데...

	m_bCollision[COLLIDERTYPE_BODY] = true;
	m_bCollision[COLLIDERTYPE_CLAW] = false;
	m_bCollision[COLLIDERTYPE_PARRY] = false;

	for (auto& _pParts : m_pBaseParts)
	{
		if (_pParts->Trail_GetOn())
			_pParts->TrailOff();
		
		_pParts->Set_CollisionOn(false);
	}

}

void CPlayer::Update_Collider()
{
	m_pColliderCom[COLLIDERTYPE_PUSH]->Update(m_pTransformCom->Get_WorldMatrix());
	CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_PLAYER_PUSH, m_pColliderCom[COLLIDERTYPE_PUSH], this);
	if (m_bCollision[COLLIDERTYPE_CLAW])
	{
		m_pColliderCom[COLLIDERTYPE_CLAW]->Update(m_pHands[HAND_RIGHT]->Get_CombinedTransformation()*XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())*m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_PLAYER_WEAPON, m_pColliderCom[COLLIDERTYPE_CLAW], this);
	}
	if (m_bCollision[COLLIDERTYPE_BODY])
	{
		m_pColliderCom[COLLIDERTYPE_BODY]->Update(m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_PLAYER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
	}
	if (m_bCollision[COLLIDERTYPE_PARRY])
	{
		m_pColliderCom[COLLIDERTYPE_PARRY]->Update(m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_PLAYER_PARRY, m_pColliderCom[COLLIDERTYPE_PARRY], this);
	}
}

_bool CPlayer::Collision(_float fTimeDelta)
{
	CGameObject* _pTarget = //static_cast<CCollider*>(m_pBaseParts[BASE_DAGGER]->Get_ComponentPtr(TEXT("Com_OBB")))->Get_Target();
		m_pColliderCom[COLLIDERTYPE_PARRY]->Get_Target();
	if (_pTarget)
	{
		//MSG_BOX(TEXT("Parry"));
		static_cast<CMonster*>(_pTarget)->Set_MonsterState(CMonster::ATTACK_STUN);
	}
	else
	{
		_pTarget = (m_pColliderCom[COLLIDERTYPE_BODY])->Get_Target();
		if (_pTarget)
		{
			//패링 안되고 몸 충돌되었을때
			
			if (m_eCurState == SD_HurtIdle)
			{
				m_eCurState = SD_StrongHurt_Start;
			}
			else
			{
				m_eCurState = SD_HurtIdle;
			}

			for (auto& _Part : m_pBaseParts)
			{
				if (_Part->Trail_GetOn())
				{
					_Part->TrailOff();
				}
				
				_Part->Set_CollisionOn(false);
			}
			m_pTransformCom->LookAt_ForLandObject(
				static_cast<CTransform*>(_pTarget->Get_ComponentPtr(TEXT("Com_Transform")))
				->Get_State(CTransform::STATE_POSITION));

			m_pStatusCom->Damage(static_cast<CStatus*>(_pTarget->Get_ComponentPtr(TEXT("Com_Status")))->Get_Attack());
			m_bCollision[COLLIDERTYPE_BODY] = false;
			return true;
		}
	}
	return false;
}

HRESULT CPlayer::Check_MotionTrail(_float fTimeDelta)
{
	m_CurMotionTrail += fTimeDelta;
	for (auto iter = m_listMotion.begin(); iter != m_listMotion.end();)
	{
		(*iter)->Tick(fTimeDelta);
		if ((*iter)->Get_Tick() > 0.5f)
		{
			(*iter)->Set_Tick(0.f);
			CDummy* _Motion = (*iter);
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
			CDummy*		_pDummy = CDummy::Creat(m_pDevice, m_pContext, _Motion, m_pTransformCom->Get_WorldFloat4x4());
			m_listMotion.push_back(_pDummy);
		}
		else
		{
			CDummy*		_pDummy = m_listDeadMotion.back();
			_pDummy->Set_CombinedMat(m_pModelCom->Get_HierarchyNodeVector(), m_pTransformCom->Get_WorldFloat4x4());
			m_listMotion.push_back(_pDummy);
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

	/* For.Com_Status */
	CStatus::STATUS _tStatus;
	_tStatus.fMaxHp = 200.f;
	_tStatus.fAttack = 40.f;
	_tStatus.fHp = _tStatus.fMaxHp;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &_tStatus)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Add_Render()
{
	if (nullptr == m_pRendererCom)
		return;

	switch (m_eWeapon)
	{
	case Client::CPlayer::WEAPON_NONE:
		break;
	case Client::CPlayer::WEAPON_BASE:
		for (auto& _pPart : m_pBaseParts)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, _pPart);
		}
		break;
	case Client::CPlayer::WEAPON_SKILL:
		for (auto& _pPart : m_pSkillParts[m_eCurSkill])
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, _pPart);
		}
		break;
	}

	for (auto& _motion : m_listMotion)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, _motion);
	}
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Ready_AnimLimit()
{
	//공격
	m_vecLimitTime[STATE_ATT1].push_back(20.f);
	m_vecLimitTime[STATE_ATT1].push_back(10.f);
	m_vecLimitTime[STATE_ATT1].push_back(25.f);
	m_vecLimitTime[STATE_ATT1].push_back(10.f);
	m_vecLimitTime[STATE_ATT1].push_back(25.f);

	m_vecLimitTime[STATE_ATT2].push_back(20.f);
	m_vecLimitTime[STATE_ATT2].push_back(10.f);
	m_vecLimitTime[STATE_ATT2].push_back(25.f);
	m_vecLimitTime[STATE_ATT2].push_back(10.f);
	m_vecLimitTime[STATE_ATT2].push_back(25.f);

	m_vecLimitTime[STATE_ATT3].push_back(25.f);
	m_vecLimitTime[STATE_ATT3].push_back(10.f);
	m_vecLimitTime[STATE_ATT3].push_back(25.f);
	m_vecLimitTime[STATE_ATT3].push_back(10.f);
	m_vecLimitTime[STATE_ATT3].push_back(25.f);

	m_vecLimitTime[STATE_ATT4].push_back(40.f);
	m_vecLimitTime[STATE_ATT4].push_back(10.f);
	m_vecLimitTime[STATE_ATT4].push_back(40.f);
	m_vecLimitTime[STATE_ATT4].push_back(10.f);
	m_vecLimitTime[STATE_ATT4].push_back(40.f);

	m_vecLimitTime[STATE_ATT5].push_back(50.f);
	m_vecLimitTime[STATE_ATT5].push_back(20.f);
	m_vecLimitTime[STATE_ATT5].push_back(50.f);
	m_vecLimitTime[STATE_ATT5].push_back(20.f);
	m_vecLimitTime[STATE_ATT5].push_back(50.f);
	
	//도끼스킬
	m_vecLimitTime[Corvus_PW_Axe].push_back(20.f);
	m_vecLimitTime[Corvus_PW_Axe].push_back(40.f);
	m_vecLimitTime[Corvus_PW_Axe].push_back(65.f);
	m_vecLimitTime[Corvus_PW_Axe].push_back(120.f);
	m_vecLimitTime[Corvus_PW_Axe].push_back(160.f);

	//쌍검
	m_vecLimitTime[DualKnife].push_back(5.f);
	m_vecLimitTime[DualKnife].push_back(40.f);

	//클로
	m_vecLimitTime[Raven_ClawNear].push_back(5.f);
	m_vecLimitTime[Raven_ClawNear].push_back(50.f);

	//패리
	m_vecLimitTime[ParryL].push_back(0.f);
	m_vecLimitTime[ParryL].push_back(50.f);
	

	return S_OK;
}

HRESULT CPlayer::Ready_Collider()
{
	/* For.Com_OBB */
	CCollider::COLLIDERDESC		ColliderDesc;	
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(0.7f, 1.4f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_BODY], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.vSize = _float3(0.7f, 1.4f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Capsule"), TEXT("Com_Capsule"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_PUSH], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(0.5f, 0.2f, 0.5f);
	_float3 vCenter = _float3(m_pModelCom->Get_HierarchyNode("ik_hand_gun")->Get_Trans()._41, 
		m_pModelCom->Get_HierarchyNode("ik_hand_gun")->Get_Trans()._42,
		m_pModelCom->Get_HierarchyNode("ik_hand_gun")->Get_Trans()._43);
	ColliderDesc.vCenter = vCenter;
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Claw"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_CLAW], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(0.7f, 1.6f, 0.3f);
	ColliderDesc.vCenter = _float3(0.f, 0.8f/*ColliderDesc.vSize.y * 0.5f*/, 0.5f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Parry"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_PARRY], &ColliderDesc)))
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

HRESULT CPlayer::Ready_Hands()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("ik_hand_gun");
	if (nullptr == pWeaponSocket)
		return E_FAIL;
	m_pHands[HAND_RIGHT] = pWeaponSocket;

	pWeaponSocket = m_pModelCom->Get_HierarchyNode("ik_hand_l");
	if (nullptr == pWeaponSocket)
		return E_FAIL;
	m_pHands[HAND_LEFT] = pWeaponSocket;

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerParts()
{
	if (FAILED(Ready_PlayerParts_Base()))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_PlayerParts_Skill()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer::Ready_PlayerParts_Skill()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	CWeapon*		pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Axe")));

	if (nullptr == pGameObject)
		return E_FAIL;
	m_pSkillParts[SKILL_AXE].push_back(pGameObject);
	m_pSkillHands[SKILL_AXE].push_back(HAND_LEFT);

	pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Knife")));

	if (nullptr == pGameObject)
		return E_FAIL;
	m_pSkillParts[SKILL_DUAL].push_back(pGameObject);
	m_pSkillHands[SKILL_DUAL].push_back(HAND_RIGHT);

	pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Knife")));

	if (nullptr == pGameObject)
		return E_FAIL;
	m_pSkillParts[SKILL_DUAL].push_back(pGameObject);
	m_pSkillHands[SKILL_DUAL].push_back(HAND_LEFT);

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerParts_Base()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	/* For.Sword */
	CWeapon*		pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Saber")));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pBaseParts.push_back(pGameObject);
	m_pBaseHands.push_back(HAND_RIGHT);

	pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Dagger")));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pBaseParts.push_back(pGameObject);
	m_pBaseHands.push_back(HAND_LEFT);

	return S_OK;
}

HRESULT CPlayer::Update_Hands_Matrix()
{
	if (nullptr == m_pHands[HAND_RIGHT])
		return E_FAIL;

	_matrix WeaponMatrix = /*m_pSockets[PART_SABER]->Get_OffSetMatrix()**/
		m_pHands[HAND_RIGHT]->Get_CombinedTransformation()
		* XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())
		* m_pTransformCom->Get_WorldMatrix();

	XMStoreFloat4x4(&(m_matHands[HAND_RIGHT]), WeaponMatrix);

	if (nullptr == m_pHands[HAND_LEFT])
		return E_FAIL;

	WeaponMatrix = /*m_pSockets[PART_SABER]->Get_OffSetMatrix()**/
		m_pHands[HAND_LEFT]->Get_CombinedTransformation()
		* XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())
		* m_pTransformCom->Get_WorldMatrix();

	XMStoreFloat4x4(&(m_matHands[HAND_LEFT]), WeaponMatrix);

	return S_OK;
}

HRESULT CPlayer::Update_Weapon(_float fTimeDelta)
{
	if (FAILED(Update_Hands_Matrix()))
	{
		MSG_BOX(TEXT("failed to update Hands"));
		return E_FAIL;
	}

	switch (m_eWeapon)
	{
	case Client::CPlayer::WEAPON_NONE:
		return S_OK;
	case Client::CPlayer::WEAPON_BASE:
		if (FAILED(Update_Weapon_Base()))
		{
			MSG_BOX(TEXT("failed to update BaseWeapon"));
			return E_FAIL;
		}
		for (auto& pPart : m_pBaseParts)
		{
			if (pPart != nullptr)
				pPart->Tick(fTimeDelta, this);
		}
		return S_OK;
	case Client::CPlayer::WEAPON_SKILL:
		if (FAILED(Update_Weapon_Skill()))
		{
			MSG_BOX(TEXT("failed to update SkillWeapon"));
			return E_FAIL;
		}
		for (auto& pPart : m_pSkillParts[m_eCurSkill])
		{
			if (pPart != nullptr)
				pPart->Tick(fTimeDelta, this);
		}
		return S_OK;
	}

	return S_OK;
}

HRESULT CPlayer::Update_Weapon_Base()
{
	_uint i = 0;
	for (auto& _pPart : m_pBaseParts)
	{
		_pPart->SetUp_State(XMLoadFloat4x4(&(m_matHands[i])));
		++i;
	}

	return S_OK;
}

HRESULT CPlayer::Update_Weapon_Skill()
{
	_uint i = 0;
	for (auto& _pPart : m_pSkillParts[m_eCurSkill])
	{
		_pPart->SetUp_State(XMLoadFloat4x4(&(m_matHands[m_pSkillHands[m_eCurSkill][i]])));
		++i;
	}

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


	for (auto& _pHand : m_pHands)
	{
		if (_pHand)
			Safe_Release(_pHand);
	}
	for (auto& _Part : m_pBaseParts)
	{
		if (_Part)
			Safe_Release(_Part);
	}
	for (_int i = 0; i < SKILL_END; ++i)
	{
		for (auto& _pPart : m_pSkillParts[i])
		{
			if (_pPart)
				Safe_Release(_pPart);
		}		
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
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pTarget);
}