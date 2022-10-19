#include "stdafx.h"
#include "..\Public\Magician.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"
#include "HierarchyNode.h"
#include "Weapon.h"
#include "CollisionMgr.h"
#include "Player.h"
#include "Status.h"

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
	
	m_eCurState = Magician_StunStart_Sword;
	return S_OK;
}

void CMagician::Tick( _float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, _Instance);
	if (_Instance->KeyDown(DIK_NUMPAD1))
	{
		m_eCurState = Walk_Disappear_F;
	}
	else if (_Instance->KeyDown(DIK_NUMPAD2))
	{
		m_eCurState = Magician_ParryAttack02;
	}
	else if (_Instance->KeyDown(DIK_NUMPAD3))
	{
		m_eCurState = Cane_Att3;
	}
	else if (_Instance->KeyDown(DIK_NUMPAD4))
	{
		m_eCurState = Magician_Stage2_Attakc01;
	}
	
	if (m_pModelCom != nullptr)
	{
		Check_Stun();
		CheckAnim();

		CheckState(fTimeDelta);
		PlayAnimation(fTimeDelta);
	}
	Update_Weapon();
	for (auto& pPart : m_pParts)
	{
		if (pPart != nullptr)
			pPart->Tick(fTimeDelta, this);
	}
	if (m_fAppear < 1.f)
	{
		m_bCollision[COLLIDERTYPE_BODY] = false;
	}
	Update_Collider();
}

void CMagician::LateTick( _float fTimeDelta)
{
	if (Collision(fTimeDelta))
	{
		CheckAnim();
		CheckState(fTimeDelta);
		PlayAnimation(fTimeDelta);
	}

	RenderGroup();
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
		


		if (FAILED(m_pModelCom->Render(m_pShaderCom, 1,i)))
			return E_FAIL;
	}

	/*for (_uint i = 0; i < COLLILDERTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}*/

	return S_OK;
}

void CMagician::PlayAnimation( _float fTimeDelta)
{
	if (m_bAnimStop)
		return;
	_float4 _vAnim;
	XMStoreFloat4(&_vAnim, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	if (m_pModelCom->Play_Animation(fTimeDelta * m_fPlaySpeed, &_vAnim, &m_fPlayTime))
	{
		CheckEndAnim();
	}
	CheckLimit();
	XMStoreFloat4(&m_AnimPos, (XMLoadFloat4(&_vAnim) - XMLoadFloat4(&m_PreAnimPos)));
	m_PreAnimPos = _vAnim;
}


void CMagician::CheckEndAnim()
{
	m_fPlaySpeed = 1.f;
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
		{
			m_pParts[PART_CANE]->Set_CollisionOn(false);
			On_Collider(COLLIDERTYPE_BODY, true);
			if (m_pParts[PART_CANE]->Trail_GetOn())
				m_pParts[PART_CANE]->TrailOff();
		}
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::SP_Att1_Suc:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::SP_Att2_Start:
	{
		m_eCurState = SP_Att2_Loop;
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
		On_Collider(COLLIDERTYPE_FOOT_R, false);
		On_Collider(COLLIDERTYPE_FOOT_L, false);
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_B:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_Disappear_B:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_F:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_Disappear_F:
		m_eCurState = Walk_Disappear_F;
		break;
	case Client::CMagician::Walk_L:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_Disappear_L:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_R:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Walk_Disappear_R:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Parry01:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_ParryAttack01:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_ParryAttack02:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_ParryJump:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Shoot1:
		m_bCollision[COLLIDERTYPE_CARD] = false;
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Shoot2:
		m_bCollision[COLLIDERTYPE_CARD] = false;
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Shoot2_Slow:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_ShotJump:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Disappear:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Stage2_Attakc01:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Stage2_Attakc04:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Stage2_SwrodAttackCombo:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Stage2_JumpAppear:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_StunStart_Sword:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_StunLoop_Sword:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_StunEnd_Sword:
		break;
	case Client::CMagician::Magician_StunStart_Cane:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_StunLoop_Cane:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_StunEnd_Cane:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_SwordAttack1:
		ChangeCanesword(CANESWORD_R);
		m_pParts[PART_CANESWORD]->Set_CollisionOn(false);
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_SwordAttack2:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_SwordAttack2_V2:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_VSCorvus_TakeExecution:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_VSCorvus_TakeExecutionDisappear:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Walk2F:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Walk2L:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Walk2R:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_HurtFL:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_HurtFR:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_HurtSL:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_HurtSR:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_DisppearIdle:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_JumpAppear:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_Sprinkle:
		m_eCurState = Magician_Idle;
		break;
	case Client::CMagician::Magician_ParrySword:
		m_eCurState = Magician_Idle;
		break;
	}

	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
}

void CMagician::CheckState(_float fTimeDelta)
{
	m_bCollision[COLLIDERTYPE_PARRY] = false;
	
	switch (m_eCurState)
	{
	case Client::CMagician::Magician_Idle:
	{
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		//콜라이더 넣음
		On_Collider(COLLIDERTYPE_BODY, true);
		//플레이어 쳐다봄 => 서서히 쳐다보도록!
		CTransform* _vPlayerPos = static_cast<CTransform*>(CGameInstance::Get_Instance()->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));
		
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 
			_vPlayerPos->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)
			, 0.9f);
		Pattern(fTimeDelta);
	}		
		break;
	case Client::CMagician::Magician_Idle2:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		//m_eMonsterState = ATTACK::ATTACK_IDLE;
		break;
	case Client::CMagician::Hurt_Short:
		if (m_pParts[PART_CANESWORD]->Trail_GetOn())
			m_pParts[PART_CANESWORD]->TrailOff();
		m_pParts[PART_CANESWORD]->Set_CollisionOn(false);

		if (m_pParts[PART_CANE]->Trail_GetOn())
			m_pParts[PART_CANE]->TrailOff();
		m_pParts[PART_CANE]->Set_CollisionOn(false);

		break;
	case Client::CMagician::Hurt_Long:
		if (m_pParts[PART_CANESWORD]->Trail_GetOn())
			m_pParts[PART_CANESWORD]->TrailOff();
		m_pParts[PART_CANESWORD]->Set_CollisionOn(false);

		if (m_pParts[PART_CANE]->Trail_GetOn())
			m_pParts[PART_CANE]->TrailOff();
		m_pParts[PART_CANE]->Set_CollisionOn(false);

		break;
	case Client::CMagician::Boss_Enter:
		break;
	case Client::CMagician::Cane_Att1:
		
		break;
	case Client::CMagician::Cane_Att2:
		m_bCollision[COLLIDERTYPE_PARRY] = false;
	
		break;
	case Client::CMagician::SP_Att1_Start:
		m_bCollision[COLLIDERTYPE_PARRY] = false;
		break;
	case Client::CMagician::SP_Att1_Suc:
		m_bCollision[COLLIDERTYPE_PARRY] = false;
		break;
	case Client::CMagician::SP_Att2_Start:
		break;
	case Client::CMagician::SP_Att2_Loop:
		On_Collider(COLLIDERTYPE_BODY, false);
		break;
	case Client::CMagician::SP_Att2_Suc:
		break;
	case Client::CMagician::Appear_L:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Appear_R:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Appear_B:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Appear_F:
		m_bCollision[COLLIDERTYPE_PARRY] = true;			
		break;
	case Client::CMagician::Cane_Att3:
		break;
	case Client::CMagician::Kick_Combo:
		{
			On_Collider(COLLIDERTYPE_FOOT_R, true);
			On_Collider(COLLIDERTYPE_FOOT_L, true);
			//On_Collider(COLLIDERTYPE_BODY, true);
		}
		
		break;
	case Client::CMagician::Walk_B:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Walk_Disappear_B:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Walk_F:
		Look_Player();
		
		m_fAppear += 0.2f;
		//만약 1 이상 되면 공격패턴으로 바뀜.
		if (m_fAppear >= 1.0f)
		{
			m_fAppear = 1.f;
			m_eCurState = m_eReserveState;
			m_eReserveState = STATE_END;
			//랜덤매니져---- 공격패턴
			m_bCollision[COLLIDERTYPE_BODY] = true;
			m_eMonsterState = ATTACK_IDLE;
			m_bCollision[COLLIDERTYPE_PARRY] = true;
		}
		break;
	case Client::CMagician::Walk_Disappear_F:
		m_bCollision[COLLIDERTYPE_BODY] = false;
		m_eMonsterState = ATTACK_DISAPPEAR;
		//여기서 0.1씩 줄여서 사라지게끔
		m_fAppear -= 0.05f;
		//0 이하면 (중간텀) 플레이어 주변에 나타나서 다시 증가
		if (m_fAppear < -3.f)
		{
			m_eCurState = Walk_F;
			m_fAppear = 0.f;
			
			Pattern_Appear();			
		}
		break;
	case Client::CMagician::Walk_L:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Walk_Disappear_L:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Walk_R:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Walk_Disappear_R:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Magician_Parry01:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Magician_ParryAttack01:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Magician_ParryAttack02:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Magician_ParryJump:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Magician_Shoot1:
		break;
	case Client::CMagician::Magician_Shoot2:
		break;
	case Client::CMagician::Magician_Shoot2_Slow:
		break;
	case Client::CMagician::Magician_ShotJump:
		break;
	case Client::CMagician::Magician_Disappear:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Magician_Stage2_Attakc01:
		break;
	case Client::CMagician::Magician_Stage2_Attakc04:
		break;
	case Client::CMagician::Magician_Stage2_SwrodAttackCombo:
		break;
	case Client::CMagician::Magician_Stage2_JumpAppear:
		break;
	case Client::CMagician::Magician_StunStart_Sword:
		m_bCollision[COLLIDERTYPE_PARRY] = false;
		m_bCollision[COLLIDERTYPE_BODY] = true;
		break;
	case Client::CMagician::Magician_StunLoop_Sword:
		break;
	case Client::CMagician::Magician_StunEnd_Sword:
		break;
	case Client::CMagician::Magician_StunStart_Cane:
		m_pTransformCom->Go_Backward(fTimeDelta/* * 0.8f*/);
		break;
	case Client::CMagician::Magician_StunLoop_Cane:
		break;
	case Client::CMagician::Magician_StunEnd_Cane:
		break;
	case Client::CMagician::Magician_SwordAttack1:
		break;
	case Client::CMagician::Magician_SwordAttack2:
		break;
	case Client::CMagician::Magician_SwordAttack2_V2:
		break;
	case Client::CMagician::Magician_VSCorvus_TakeExecution:
		break;
	case Client::CMagician::Magician_VSCorvus_TakeExecutionDisappear:
		break;
	case Client::CMagician::Magician_Walk2F:
		break;
	case Client::CMagician::Magician_Walk2L:
		break;
	case Client::CMagician::Magician_Walk2R:
		break;
	case Client::CMagician::Magician_HurtFL:
		break;
	case Client::CMagician::Magician_HurtFR:
		break;
	case Client::CMagician::Magician_HurtSL:
		break;
	case Client::CMagician::Magician_HurtSR:
		break;
	case Client::CMagician::Magician_DisppearIdle:
		m_bCollision[COLLIDERTYPE_PARRY] = true;
		break;
	case Client::CMagician::Magician_JumpAppear:
		break;
	case Client::CMagician::Magician_Sprinkle:
		break;
	case Client::CMagician::Magician_ParrySword:
		break;
	}

	Get_AnimMat();
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
		if (15.f < m_fPlayTime)
		{
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
		}
		break;
	case Client::CMagician::Hurt_Long:
		if (15.f < m_fPlayTime)
		{
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
		}
		break;
	case Client::CMagician::Boss_Enter:
		break;
	case Client::CMagician::Cane_Att1:
		if (m_vecLimitTime[Cane_Att1][1] < m_fPlayTime)
		{
			m_pParts[PART_CANE]->Set_CollisionOn(false);
			On_Collider(COLLIDERTYPE_BODY, true);
			if (m_pParts[PART_CANESWORD]->Trail_GetOn())
				m_pParts[PART_CANESWORD]->TrailOff();
		}
		else if (m_vecLimitTime[Cane_Att1][0] < m_fPlayTime)
		{
			m_pParts[PART_CANE]->Set_CollisionOn(true);
			On_Collider(COLLIDERTYPE_BODY, false);
			if (!m_pParts[PART_CANESWORD]->Trail_GetOn())
				m_pParts[PART_CANESWORD]->TrailOn();
		}
		break;
	case Client::CMagician::Cane_Att2:

			if (m_vecLimitTime[Cane_Att2][1] < m_fPlayTime)
			{
				m_pParts[PART_CANE]->Set_CollisionOn(false);
				On_Collider(COLLIDERTYPE_BODY, true);
				if (m_pParts[PART_CANE]->Trail_GetOn())
					m_pParts[PART_CANE]->TrailOff();
			}
			else if (m_vecLimitTime[Cane_Att2][0] < m_fPlayTime)
			{
				m_pParts[PART_CANE]->Set_CollisionOn(true);
				On_Collider(COLLIDERTYPE_BODY, false);
				if (!m_pParts[PART_CANE]->Trail_GetOn())
					m_pParts[PART_CANE]->TrailOn();
			}
		break;
	case Client::CMagician::SP_Att1_Start:
		if (m_vecLimitTime[SP_Att1_Start][1] < m_fPlayTime)
		{
			m_pParts[PART_CANE]->Set_CollisionOn(false);
			On_Collider(COLLIDERTYPE_BODY, true);
			if (!m_pParts[PART_CANE]->Trail_GetOn())
				m_pParts[PART_CANE]->TrailOn();
		}
		else if (m_vecLimitTime[SP_Att1_Start][0] < m_fPlayTime)
		{
			m_pParts[PART_CANE]->Set_CollisionOn(true);
			On_Collider(COLLIDERTYPE_BODY, false);
			if (!m_pParts[PART_CANE]->Trail_GetOn())
				m_pParts[PART_CANE]->TrailOn();
		}
		break;
	case Client::CMagician::SP_Att1_Suc:
		if (m_vecLimitTime[SP_Att1_Suc][1] < m_fPlayTime)
		{
			m_pParts[PART_CANE]->Set_CollisionOn(false);
			On_Collider(COLLIDERTYPE_BODY, true);
			if (m_pParts[PART_CANE]->Trail_GetOn())
				m_pParts[PART_CANE]->TrailOff();
		}
		else if (m_vecLimitTime[SP_Att1_Suc][0] < m_fPlayTime)
		{
			m_pParts[PART_CANE]->Set_CollisionOn(true);
			On_Collider(COLLIDERTYPE_BODY, false);
			if (!m_pParts[PART_CANE]->Trail_GetOn())
				m_pParts[PART_CANE]->TrailOn();
		}
		break;
	case Client::CMagician::SP_Att2_Start:

		break;
	case Client::CMagician::SP_Att2_Loop:
		if (m_vecLimitTime[SP_Att2_Loop][2] < m_fPlayTime)
		{
			ChangeCanesword(CANESWORD_R);
			if (m_pParts[PART_CANESWORD]->Trail_GetOn())
				m_pParts[PART_CANESWORD]->TrailOff();
			m_pParts[PART_CANESWORD]->Set_CollisionOn(false);
			On_Collider(COLLIDERTYPE_BODY, true);
		}
		else if (m_vecLimitTime[SP_Att2_Loop][1] < m_fPlayTime)
		{
			ChangeCanesword(CANESWORD_L);
		}
		else if (m_vecLimitTime[SP_Att2_Loop][0] < m_fPlayTime)
		{
			On_Collider(COLLIDERTYPE_BODY, false);
			if (!m_pParts[PART_CANESWORD]->Trail_GetOn())
				m_pParts[PART_CANESWORD]->TrailOn();
			m_pParts[PART_CANESWORD]->Set_CollisionOn(true);
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
	case Client::CMagician::Walk_Disappear_R:
		break;
	case Client::CMagician::Magician_Parry01:
		if (m_vecLimitTime[Magician_Parry01][2] < m_fPlayTime) // 공격 끝
		{
			//충돌체 빼고, 여기서부터 공격당할 수 있고
			On_Collider(COLLIDERTYPE_BODY, true);
			m_pParts[PART_CANESWORD]->Set_CollisionOn(false);
			ChangeCanesword(CANESWORD_R);
			if (m_pParts[PART_CANESWORD]->Trail_GetOn())
			{
				m_pParts[PART_CANESWORD]->TrailOff();
			}
		}
		else if (m_vecLimitTime[Magician_Parry01][1] < m_fPlayTime) // 공격 시작
		{
			m_pParts[PART_CANESWORD]->Set_CollisionOn(true);
			ChangeCanesword(CANESWORD_L);
			if (!m_pParts[PART_CANESWORD]->Trail_GetOn())
			{
				m_pParts[PART_CANESWORD]->TrailOn();
			}
			Look_Player();
		}
		else if (m_vecLimitTime[Magician_Parry01][0] < m_fPlayTime) // 패링타이밍
		{
			On_Collider(COLLIDERTYPE_BODY, false);
			m_eMonsterState = ATTACK::ATTACK_PARRY;
		}
		break;
	case Client::CMagician::Magician_ParryAttack01:
		if (m_vecLimitTime[Magician_ParryAttack01][1] < m_fPlayTime) // 패링타이밍
		{
			On_Collider(COLLIDERTYPE_BODY, true);
			On_Collider(COLLIDERTYPE_FOOT_L, false);
		}
		else if (m_vecLimitTime[Magician_ParryAttack01][0] < m_fPlayTime) // 패링타이밍
		{
			On_Collider(COLLIDERTYPE_BODY, false);
			On_Collider(COLLIDERTYPE_FOOT_L, true);
			Look_Player();
		}
		break;
	case Client::CMagician::Magician_ParryAttack02:
		break;
	case Client::CMagician::Magician_ParryJump:
		break;
	case Client::CMagician::Magician_Shoot1:
		if (m_vecLimitTime[Magician_Shoot1][0] < m_fPlayTime)
		{
			if (m_bCollision[COLLIDERTYPE_CARD] == false)
			{
				Shoot();
				m_bCollision[COLLIDERTYPE_CARD] = true;
			}
		}
		break;
	case Client::CMagician::Magician_Shoot2:
		if (m_vecLimitTime[Magician_Shoot2][0] < m_fPlayTime)
		{
			if (m_bCollision[COLLIDERTYPE_CARD] == false)
			{
				Shoot();
				m_bCollision[COLLIDERTYPE_CARD] = true;
			}
		}
		break;
	case Client::CMagician::Magician_Shoot2_Slow:
		break;
	case Client::CMagician::Magician_ShotJump:
		break;
	case Client::CMagician::Magician_Disappear:
		break;
	case Client::CMagician::Magician_Stage2_Attakc01:
		break;
	case Client::CMagician::Magician_Stage2_Attakc04:
		break;
	case Client::CMagician::Magician_Stage2_SwrodAttackCombo:
		break;
	case Client::CMagician::Magician_Stage2_JumpAppear:
		break;
	case Client::CMagician::Magician_StunStart_Sword:
		break;
	case Client::CMagician::Magician_StunLoop_Sword:
		break;
	case Client::CMagician::Magician_StunEnd_Sword:
		break;
	case Client::CMagician::Magician_StunStart_Cane:
		if (m_vecLimitTime[Magician_StunStart_Cane][0] < m_fPlayTime)
		{
			m_eCurState = Magician_ParrySword;
		}
		break;
	case Client::CMagician::Magician_StunLoop_Cane:
		break;
	case Client::CMagician::Magician_StunEnd_Cane:
		break;
	case Client::CMagician::Magician_SwordAttack1:
		if (m_vecLimitTime[Magician_SwordAttack1][1] < m_fPlayTime)
		{
			//충돌체 빼고, 여기서부터 공격당할 수 있고
			On_Collider(COLLIDERTYPE_BODY, true);
			m_pParts[PART_CANESWORD]->Set_CollisionOn(false);
			if (m_pParts[PART_CANESWORD]->Trail_GetOn())
			{
				m_pParts[PART_CANESWORD]->TrailOff();
			}
			ChangeCanesword(CANESWORD_R);

		}
		else if (m_vecLimitTime[Magician_SwordAttack1][0] < m_fPlayTime)
		{
			On_Collider(COLLIDERTYPE_BODY, false);
			m_pParts[PART_CANESWORD]->Set_CollisionOn(true);
			if (!m_pParts[PART_CANESWORD]->Trail_GetOn())
			{
				m_pParts[PART_CANESWORD]->TrailOn();
			}
			ChangeCanesword(CANESWORD_L);
		}
		break;
	case Client::CMagician::Magician_SwordAttack2:
		break;
	case Client::CMagician::Magician_SwordAttack2_V2:
		break;
	case Client::CMagician::Magician_VSCorvus_TakeExecution:
		break;
	case Client::CMagician::Magician_VSCorvus_TakeExecutionDisappear:
		break;
	case Client::CMagician::Magician_Walk2F:
		break;
	case Client::CMagician::Magician_Walk2L:
		break;
	case Client::CMagician::Magician_Walk2R:
		break;
	case Client::CMagician::Magician_HurtFL:
		break;
	case Client::CMagician::Magician_HurtFR:
		break;
	case Client::CMagician::Magician_HurtSL:
		break;
	case Client::CMagician::Magician_HurtSR:
		break;
	case Client::CMagician::Magician_DisppearIdle:
		break;
	case Client::CMagician::Magician_JumpAppear:
		break;
	case Client::CMagician::Magician_Sprinkle:
		break;
	case Client::CMagician::Magician_ParrySword:
		if (m_vecLimitTime[Magician_ParrySword][1] < m_fPlayTime)
		{
			//충돌체 빼고, 여기서부터 공격당할 수 있고
			On_Collider(COLLIDERTYPE_BODY, true);
			m_pParts[PART_CANESWORD]->Set_CollisionOn(false);
			if (m_pParts[PART_CANESWORD]->Trail_GetOn())
			{
				m_pParts[PART_CANESWORD]->TrailOff();
			}
			ChangeCanesword(CANESWORD_R);

		}
		else if (m_vecLimitTime[Magician_ParrySword][0] < m_fPlayTime)
		{
			On_Collider(COLLIDERTYPE_BODY, false);
			m_pParts[PART_CANESWORD]->Set_CollisionOn(true);
			if (!m_pParts[PART_CANESWORD]->Trail_GetOn())
			{
				m_pParts[PART_CANESWORD]->TrailOn();
			}
			ChangeCanesword(CANESWORD_L);
		}
		break;
	}
}

void CMagician::Set_Anim(STATE _eState)
{
	m_eCurState = _eState;
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	m_eMonsterState = CMonster::ATTACK_IDLE;
	m_pModelCom->Set_AnimationIndex(m_eCurState);
	m_fPlayTime = 0.f;
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

void CMagician::RenderGroup()
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

_bool CMagician::Collision(_float fTimeDelta)
{
	CGameObject* _pTarget = nullptr;

	if (_pTarget = m_pColliderCom[COLLIDERTYPE_PUSH]->Get_Target())
	{
		_vector _vDir =	XMLoadFloat3(&(static_cast<CCapsule*>(m_pColliderCom[COLLIDERTYPE_PUSH])->Get_Dir()));
		_float	_vDis = (static_cast<CCapsule*>(m_pColliderCom[COLLIDERTYPE_PUSH])->Get_Dis());
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize( _vDir) * _vDis);
	}

	AUTOINSTANCE(CGameInstance, _instance);
		
	if (_pTarget = m_pColliderCom[COLLIDERTYPE_PARRY]->Get_Target())
	{
		CPlayer* _pPlayer = static_cast<CPlayer*>(_pTarget);
		if (_instance->Rand_Int(1, 10) > 2 && *(_pPlayer->Get_AnimState()) == CPlayer::STATE_ATT1)//임시 확률
		{
			AUTOINSTANCE(CGameInstance, _pInstance);
			if (_pInstance->Rand_Int(0, 1) == 0)
			{
				m_eCurState = Magician_ParryAttack01;
			}
			else
			{
				m_eCurState = Magician_Parry01;
			}
			m_eMonsterState = ATTACK_PARRY;
			
			Look_Player();
		}
	}
	

	if ((_pTarget = m_pColliderCom[COLLIDERTYPE_BODY]->Get_Target()) && (CPlayer::ParryL != *static_cast<CPlayer*>(_pTarget)->Get_AnimState()))
	{
		CPlayer* _pPlayer = static_cast<CPlayer*>(_pTarget);
		if (m_eMonsterState == ATTACK_PARRY)
		{
			
			//m_eCurState = Magician_Parry01;
			
			_pPlayer->Set_AnimState(CPlayer::SD_HurtIdle);
			_pPlayer->Cancle();
			
			m_eMonsterState = ATTACK_IDLE;
			return true;
		}
		
		if (m_eCurState == Hurt_Long)
		{

			if (_instance->Rand_Int(0, 4) == 0)
			{
				m_eCurState = Walk_Disappear_F;
			}
			else
			{
				m_eCurState = Hurt_Short;
				m_fAppear = 1.f;
			}
			
		}
		else if (m_eCurState == Hurt_Short)
		{
			if (_instance->Rand_Int(0, 4) == 0)
			{
				m_eCurState = Walk_Disappear_F;
			}
			else
			{
				m_eCurState = Hurt_Long;
				m_fAppear = 1.f;
			}
		}
		else
		{
			if (_instance->Rand_Int(0, 4) == 0)
			{
				m_eCurState = Walk_Disappear_F;
			}
			else
			{
				m_eCurState = Hurt_Short;
				m_fAppear = 1.f;
			}
		}
		m_pTransformCom->LookAt_ForLandObject(
			static_cast<CTransform*>(_pTarget->Get_ComponentPtr(TEXT("Com_Transform")))
			->Get_State(CTransform::STATE_POSITION));
		return true;
	}
	return false;
}

void CMagician::On_Collider(MAGICIANCOLLIDER _eCollider, _bool _bCollision)
{
	switch (_eCollider)
	{
	case Client::CMagician::COLLIDERTYPE_BODY:		
		if (m_bCollision[COLLIDERTYPE_BODY] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
		break;
	case Client::CMagician::COLLIDERTYPE_PARRY:
		if (m_bCollision[COLLIDERTYPE_PARRY] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_PARRY], this);
		break;
	case Client::CMagician::COLLIDERTYPE_FOOT_R:
		if (m_bCollision[COLLIDERTYPE_FOOT_R] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_FOOT_R], this);
		break;
	case Client::CMagician::COLLIDERTYPE_FOOT_L:
		if (m_bCollision[COLLIDERTYPE_FOOT_L] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_FOOT_L], this);
		break;
	case Client::CMagician::COLLILDERTYPE_END:
		break;
	}
}

void CMagician::Look_Move_Player(_float _fPosX, _float _fPosZ)
{
	AUTOINSTANCE(CGameInstance, _Intance);
	CTransform* _Trans =
		static_cast<CTransform*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));

	_vector _vTargetPos = _Trans->Get_State(CTransform::STATE_POSITION);
	_vTargetPos.m128_f32[0] += _fPosX;
	_vTargetPos.m128_f32[2] += _fPosZ;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vTargetPos);
	m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));
}

void CMagician::Look_Player()
{
	AUTOINSTANCE(CGameInstance, _Intance);
	CTransform* _Trans =
		static_cast<CTransform*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));

	m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));
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

	/* For.Com_Status */
	CStatus::STATUS _tStatus;
	_tStatus.fMaxHp = 300.f;
	_tStatus.fAttack = 30.f;
	_tStatus.fHp = _tStatus.fMaxHp;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &_tStatus)))
		return E_FAIL;


	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 1.8f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_BODY], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 1.8f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Capsule"), TEXT("Com_Capsule"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_PUSH], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.2f, 0.2f, 0.2f);
	_float3 vCenter = _float3(m_pModelCom->Get_HierarchyNode("ball_r")->Get_Trans()._41,
		m_pModelCom->Get_HierarchyNode("ball_r")->Get_Trans()._42,
		m_pModelCom->Get_HierarchyNode("ball_r")->Get_Trans()._43);
	ColliderDesc.vCenter = vCenter;
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB_FootR"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_FOOT_R], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.2f, 0.2f, 0.2f);
	vCenter = _float3(m_pModelCom->Get_HierarchyNode("ball_l")->Get_Trans()._41,
		m_pModelCom->Get_HierarchyNode("ball_l")->Get_Trans()._42,
		m_pModelCom->Get_HierarchyNode("ball_l")->Get_Trans()._43);
	ColliderDesc.vCenter = vCenter;
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB_FootL"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_FOOT_L], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 1.8f, 0.3f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.5f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB_Parry"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_PARRY], &ColliderDesc)))
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAppear, sizeof(_float))))
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
	m_vecLimitTime[SP_Att2_Loop].push_back(0.f);
	m_vecLimitTime[SP_Att2_Loop].push_back(20.f);
	m_vecLimitTime[SP_Att2_Loop].push_back(150.f);//10까지
	m_vecLimitTime[SP_Att2_Suc].push_back(220.f);//10.1

	m_vecLimitTime[Hurt_Long].push_back(25.f);

	m_vecLimitTime[Magician_SwordAttack1].push_back(160.f);
	m_vecLimitTime[Magician_SwordAttack1].push_back(430.f);//안움직임

	m_vecLimitTime[Cane_Att1].push_back(50.f);
	m_vecLimitTime[Cane_Att1].push_back(200.f);//5.7

	m_vecLimitTime[Cane_Att2].push_back(10.f);
	m_vecLimitTime[Cane_Att2].push_back(45.f);//5.5까지 감

	m_vecLimitTime[SP_Att1_Start].push_back(10.f);
	m_vecLimitTime[SP_Att1_Start].push_back(40.f);//5.5

	m_vecLimitTime[SP_Att1_Suc].push_back(10.f);
	m_vecLimitTime[SP_Att1_Suc].push_back(120.f);

	m_vecLimitTime[Magician_Parry01].push_back(10.f);
	m_vecLimitTime[Magician_Parry01].push_back(130.f);
	m_vecLimitTime[Magician_Parry01].push_back(200.f);

	
	m_vecLimitTime[Magician_ParryAttack01].push_back(150.f);
	m_vecLimitTime[Magician_ParryAttack01].push_back(250.f);

	m_vecLimitTime[Magician_Shoot1].push_back(45.f);

	m_vecLimitTime[Magician_Shoot2].push_back(20.f);
	
	m_vecLimitTime[Magician_StunStart_Cane].push_back(45.f);

	m_vecLimitTime[Magician_ParrySword].push_back(70.f);
	m_vecLimitTime[Magician_ParrySword].push_back(150.f);//7.6
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
	for (auto& _Collider : m_pColliderCom)
	{
		if (_Collider)
			Safe_Release(_Collider);
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
	CWeapon*		pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Cane"),this));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pParts[PART_CANE] = pGameObject;


	pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Cane_Sword"),this));

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

void CMagician::Update_Collider()
{
	//if (m_bCollision[COLLIDERTYPE_BODY])
	m_pColliderCom[COLLIDERTYPE_PUSH]->Update(m_pTransformCom->Get_WorldMatrix());
	CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_PUSH, m_pColliderCom[COLLIDERTYPE_PUSH], this);

	if (m_bCollision[COLLIDERTYPE_BODY])
	{
		m_pColliderCom[COLLIDERTYPE_BODY]->Update(m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
	}
	if (m_bCollision[COLLIDERTYPE_PARRY])
	{
		m_pColliderCom[COLLIDERTYPE_PARRY]->Update(m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_PARRY], this);
	}
	if (m_bCollision[COLLIDERTYPE_FOOT_R])
	{
		m_pColliderCom[COLLIDERTYPE_FOOT_R]->Update(m_pModelCom->Get_HierarchyNode("ball_r")->Get_CombinedTransformation()*XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())*m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_FOOT_R], this);
	}

	if (m_bCollision[COLLIDERTYPE_FOOT_L])
	{
		m_pColliderCom[COLLIDERTYPE_FOOT_L]->Update(m_pModelCom->Get_HierarchyNode("ball_l")->Get_CombinedTransformation()*XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())*m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_FOOT_L], this);
	}
		
}

HRESULT CMagician::Shoot()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);
	//pos값 넣어주는데 z랑 y를 좀 올려서...
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Weapon_Card"), LEVEL_GAMEPLAY, TEXT("Layer_Bullet"), m_pTransformCom)))
		return E_FAIL;
	return S_OK;
}

void CMagician::Check_Stun()
{
	if (m_eMonsterState == CMonster::ATTACK_STUN)
	{
		AUTOINSTANCE(CGameInstance, _pInstance);
		if (m_pParts[PART_CANESWORD]->Trail_GetOn())
			m_pParts[PART_CANESWORD]->TrailOff();
		m_pParts[PART_CANESWORD]->Set_CollisionOn(false);

		if (m_pParts[PART_CANE]->Trail_GetOn())
			m_pParts[PART_CANE]->TrailOff();
		m_pParts[PART_CANE]->Set_CollisionOn(false);
		Look_Player();
		_int _iTemp = _pInstance->Rand_Int(0, 3);
		if (_iTemp == 0)
		{
			m_eCurState = Magician_StunStart_Cane;			
		}
		else
		{
			m_eCurState = Magician_StunStart_Sword;
		}
		
		m_eMonsterState = CMonster::ATTACK_IDLE;
	}
}

void CMagician::Pattern(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	m_fStay += fTimeDelta;
	_float _fStayTime = _pInstance->Rand_Float(1.5f, 3.5f);
	if (m_fStay < _fStayTime)
		return;
	m_fStay = 0.f;
	
	CTransform* _pPlayer_Trans = static_cast<CTransform*>(_pInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));
	_float _fDis = fabs(XMVectorGetX(XMVector3Length(_pPlayer_Trans->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))));
	if (_fDis > 6.f)
	{
		AUTOINSTANCE(CGameInstance, _pInstance);
		_int iPattern = _pInstance->Rand_Int(0, 4);
		if (iPattern < 3)
			Pattern_Long(_fDis);
		else
			m_eCurState = Walk_Disappear_F;
	}
	else
	{
		Pattern_Short(_fDis);
	}
}

void CMagician::Pattern_Appear()
{
	//한대 맞고 사라지면?
	AUTOINSTANCE(CGameInstance, _pInstance);
	_int iPattern = _pInstance->Rand_Int(0,10);
	_int iDir = _pInstance->Rand_Int(0, 3);
	_float	_fZ(0.f), _fX(0.f);
	switch (iDir)
	{
	case 0://Z
		_fZ = 0.7f;
		break;
	case 1://-Z
		_fZ = -0.7f;
		break;
	case 2://X
		_fX = 0.7f;
		break;
	case 3://-X
		_fX = -0.7f;
		break;
	}
	m_fPlaySpeed = 3.f;
	switch (iPattern)
	{
	case 0:
		m_eReserveState = Kick_Combo;
		Look_Move_Player(3.f * _fX, 3.f*_fZ);
		break;
	case 1:
		m_eReserveState = SP_Att2_Start;
		Look_Move_Player(10.f * _fX, 10.f*_fZ);
		break;
	case 2:
		m_eReserveState = SP_Att1_Start;
		Look_Move_Player(5.5f * _fX, 5.5f*_fZ);
		break;
	case 3:
		m_eReserveState = Cane_Att1;
		Look_Move_Player(5.5f * _fX, 5.5f*_fZ);
		break;
	case 4:
		m_eReserveState = Cane_Att2;
		Look_Move_Player(5.5f * _fX, 5.5f*_fZ);
		break;
	case 5:
		m_eReserveState = Magician_SwordAttack1;
		Look_Move_Player(2.f * _fX, 2.f*_fZ);
		break;
	case 6:
		m_eReserveState = Magician_Shoot1;
		Look_Move_Player(10.f * _fX, 10.f*_fZ);
		m_fPlaySpeed = 1.f;
		break;
	case 7:
		m_eReserveState = Magician_Shoot2;
		Look_Move_Player(10.f * _fX, 10.f*_fZ);
		m_fPlaySpeed = 1.f;
		break;
	default:
		m_eReserveState = Magician_Idle;
		Look_Move_Player(3.f * _fX, 3.f*_fZ);
		break;
	}

}

void CMagician::Pattern_Short(_float fDis)
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	_int iPattern = _pInstance->Rand_Int(0, 5);
	//거리에 따라서 어느정도는 패턴 줄까

	if (fDis < 2.f)
	{
		if (iPattern < 3)
			m_eCurState = Magician_SwordAttack1;
		else
			m_eCurState = Walk_Disappear_F;
		return;
	}

	switch (iPattern)
	{
	case 0:
		m_eCurState = Kick_Combo;
		break;
	case 1:
		m_eCurState = SP_Att1_Start;
		break;
	case 2:
		m_eCurState = Cane_Att1;
		break;
	case 3:
		m_eCurState = Cane_Att2;
		break;
	default:
		m_eCurState = Walk_Disappear_F;
		break;
	}
}

void CMagician::Pattern_Long(_float fDis)
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	_int iPattern = _pInstance->Rand_Int(0, 3);

	if (fDis < 7.f)
	{
		switch (iPattern)
		{
		case 0:
			m_eCurState = SP_Att2_Start;
			m_fPlaySpeed = 3.f;
			break;
		case 1:
			m_eCurState = Magician_ParrySword;
			break;
		case 2:
			m_eCurState = Magician_Shoot1;
			break;
		case 3:
			m_eCurState = Magician_Shoot2;
			break;
		}
	}
	else
	{
		if (iPattern < 2)
		{
			m_eCurState = Magician_Shoot1;
		}
		else
		{
			m_eCurState = Magician_Shoot2;
		}
	}
}
