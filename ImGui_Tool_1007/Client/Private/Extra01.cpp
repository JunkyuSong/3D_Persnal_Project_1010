#include "stdafx.h"
#include "..\Public\Extra01.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"
#include "HierarchyNode.h"
#include "Weapon.h"
#include "CollisionMgr.h"
#include "Player.h"
#include "Status.h"
#include "Animation.h"

CExtra01::CExtra01(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CExtra01::CExtra01(const CExtra01 & rhs)
	: CMonster(rhs)
	, m_AnimPos(rhs.m_AnimPos)
	, m_PreAnimPos(rhs.m_PreAnimPos)
{
}

HRESULT CExtra01::Initialize_Prototype()
{
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;

	m_bAnimStop = true;

	return S_OK;
}

HRESULT CExtra01::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	Ready_LimitTime();


	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.672, 2.402, 50.622, 1.f));
	_bool		isMove = true;
	Get_AnimMat();
	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != m_pNavigationCom)
		isMove = m_pNavigationCom->isMove(XMVectorSet(30.672, 2.402, 50.622, 1.f), &vNormal);

	if (true == isMove)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.672, 2.402, 50.622, 1.f));
	m_eCurState = BossBat_Idle;


	return S_OK;
}

void CExtra01::Tick(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, _Instance);
	if (_Instance->KeyDown(DIK_NUMPAD1))
	{
		m_eCurState = BossBat_JumpSmash_Chest;
	}
	else if (_Instance->KeyDown(DIK_NUMPAD2))
	{
		m_eCurState = BossBat_JumpSmashForwardL;
	}
	else if (_Instance->KeyDown(DIK_NUMPAD3))
	{
		m_eCurState = BossBat_AttackL_01_3a;
	}
	else if (_Instance->KeyDown(DIK_NUMPAD4))
	{
		m_eCurState = BossBat_Dash;
	}

	if (m_pModelCom != nullptr)
	{
		Check_Stun();
		CheckAnim();

		
		PlayAnimation(fTimeDelta);
		CheckState(fTimeDelta);
	}

	Update_Collider();
}

void CExtra01::LateTick(_float fTimeDelta)
{
	if (Collision(fTimeDelta))
	{
		CheckAnim();
		CheckState(fTimeDelta);
		PlayAnimation(fTimeDelta);
	}

	RenderGroup();
}

HRESULT CExtra01::Render()
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



		if (FAILED(m_pModelCom->Render(m_pShaderCom, 0, i)))
			return E_FAIL;
	}

	for (_uint i = 0; i < COLLILDERTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
	m_pNavigationCom->Render();

	return S_OK;
}

void CExtra01::PlayAnimation(_float fTimeDelta)
{
	if (m_bAnimStop)
		return;
	_float4 _vAnim;
	XMStoreFloat4(&_vAnim, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	if (m_pModelCom->Play_Animation(fTimeDelta * m_fPlaySpeed, &_vAnim, &m_fPlayTime, m_bAgainAnim))
	{
		CheckEndAnim();
	}
	CheckLimit();
	XMStoreFloat4(&m_AnimPos, (XMLoadFloat4(&_vAnim) - XMLoadFloat4(&m_PreAnimPos)));
	m_PreAnimPos = _vAnim;
}


void CExtra01::CheckEndAnim()
{
	m_fPlaySpeed = 1.f;

	switch (m_eCurState)
	{
	case Client::CExtra01::BossBat_AttackL_01_1:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_AttackL_01_2b:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_AttackL_01_3a:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_AttackR_01_1:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_AttackR_01_2b:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_Bite_2:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_Dash:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_FTurn_L:
		m_pModelCom->DirectAnim(BossBat_Idle);
		m_pTransformCom->Turn_Angle(XMVectorSet(0.f,1.f,0.f,0.f),XMConvertToRadians(180.f));
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_FTurn_R:
		m_pModelCom->DirectAnim(BossBat_Idle);
		m_pTransformCom->Turn_Angle(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_HurtXL_R:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_HurtXL_L:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_Idle:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_JumpSmash_Chest:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_JumpSmashForwardL:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_JumpSmashL:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_FightStart:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_Stun:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_TakeExecution_End:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_TakeExecution_DeadStart01:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_TurnL90:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_TurnR90:
		m_eCurState = BossBat_Idle;
		break;
	case Client::CExtra01::BossBat_WalkF:
		break;
	}

	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
}

void CExtra01::CheckState(_float fTimeDelta)
{
	
	Get_AnimMat();
}

void CExtra01::CheckLimit()
{
	
}

void CExtra01::Set_Anim(STATE _eState)
{
	m_eCurState = _eState;
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	m_eMonsterState = CMonster::ATTACK_IDLE;
	m_pModelCom->Set_AnimationIndex(m_eCurState);
	m_fPlayTime = 0.f;
}

void CExtra01::CheckAnim()
{
	if (m_ePreState != m_eCurState)
	{
		if (m_ePreState == BossBat_TurnL90)
		{
			//아이들 상태로 바꿔주는 작업...? 아니지 들어온 애니메이션의 첫 상태로 싹 세팅해주는 작업이 필요함.

			// 싹 돌되

			//조금 빡센데...
		}
		Set_Anim(m_eCurState);
		m_ePreState = m_eCurState;
	}
}

void CExtra01::Get_AnimMat()
{
	if (m_bAnimStop)
		return;
	_fmatrix _World = m_pTransformCom->Get_WorldMatrix();
	_vector _vPos;
	_vPos = XMVector3TransformCoord(XMLoadFloat4(&m_AnimPos), _World);

	_bool		isMove = true;

	if (nullptr != m_pNavigationCom)
		isMove = m_pNavigationCom->isMove(_vPos, nullptr);

	if (true == isMove)
	{
		if (m_eCurState == BossBat_WalkF || m_eCurState == BossBat_Idle)
			_vPos.m128_f32[1] = m_pNavigationCom->Get_PosY(_vPos);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
	}
}

void CExtra01::RenderGroup()
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

_bool CExtra01::Collision(_float fTimeDelta)
{
	CGameObject* _pTarget = nullptr;

	AUTOINSTANCE(CGameInstance, _instance);

	return false;
}

void CExtra01::On_Collider(EXTRA01COLLIDER _eCollider, _bool _bCollision)
{
	switch (_eCollider)
	{
	case Client::CExtra01::COLLIDERTYPE_BODY:
		if (m_bCollision[COLLIDERTYPE_BODY] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
		break;
	
	}
}

void CExtra01::Look_Move_Player(_float _fPosX, _float _fPosZ)
{
	// 애니메이션 진행 중에 턴하는 함수

	AUTOINSTANCE(CGameInstance, _Intance);
	CTransform* _Trans =
		static_cast<CTransform*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));

	CNavigation* _pNavi =
		static_cast<CNavigation*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Navigation")));
	m_pNavigationCom->Set_Index(_pNavi->Get_Index());
	_vector _vTargetPos = _Trans->Get_State(CTransform::STATE_POSITION);
	_vTargetPos.m128_f32[0] += _fPosX;
	_vTargetPos.m128_f32[2] += _fPosZ;

	_bool		isMove = true;
	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != m_pNavigationCom)
		isMove = m_pNavigationCom->isMove(_vTargetPos, &vNormal);

	if (true == isMove)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vTargetPos);

	m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));
}

void CExtra01::Look_Player()
{
	//한번에 바라보는거
	AUTOINSTANCE(CGameInstance, _Intance);
	CTransform* _Trans =
		static_cast<CTransform*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));

	m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));
}

void CExtra01::Turn()
{
	/*
	플레이어가 룩의 일정 범위 안에 있으면 턴 안하고
	일정 범위 안에 없으면 그 범위 안에 들어올때까지 턴한다
	*/

	AUTOINSTANCE(CGameInstance, _pInstance);
	CPlayer* _pPlayer = static_cast<CPlayer*>(_pInstance->Get_Player());

	_vector _vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	CTransform* _pTransform = static_cast<CTransform*>(_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")));
	_vector _vDestLook = XMVector3Normalize(_pTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_float _fAngle = XMVector3Dot(_vLook, _vDestLook).m128_f32[0];


	m_eCurState = BossBat_WalkF;

	m_pTransformCom->Turn(_vLook,
		_vDestLook
		, 0.8f);

	if (_fAngle > ((sqrtf(6.f) + sqrtf(2.f)) / 4.f))
		Pattern();

}

void CExtra01::Pattern()
{
	//거리에 따라 확률적으로 공격하고 날라댕기고
	// 거리 짧으면 또 그러고 어키어키
	AUTOINSTANCE(CGameInstance, _pInstance);
	_uint chance = _pInstance->Rand_Int(1,100);

	CTransform* _pPlayerTransform = static_cast<CTransform*>(_pInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));
	_float _fDistance = XMVector3Length(_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)).m128_f32[0];

	//공격인지 아이들인지 먼저 체크
	if (chance > 5 || _fDistance > 30.f)
	{
		return;
	}

	// 공격 패턴 짜기 시작
	//플레이어와의 거리
	
	if (_fDistance < 10.f)
	{
		_uint ChanceShort = _pInstance->Rand_Int(0, 7);
		switch (ChanceShort)
		{
		case 0:
			m_eCurState = BossBat_AttackL_01_1;
			break;
		case 1:
			m_eCurState = BossBat_AttackL_01_2b;
			break;
		case 2:
			m_eCurState = BossBat_AttackL_01_3a;
			break;
		case 3:
			m_eCurState = BossBat_AttackR_01_1;
			break;
		case 4:
			m_eCurState = BossBat_AttackR_01_2b;
			break;
		case 5:
			m_eCurState = BossBat_Bite_2;
			break;
		case 6:
			m_eCurState = BossBat_Dash;
			break;
		case 7:
			m_eCurState = BossBat_JumpSmashL;
			break;
		}
	}
	else
	{
		_uint ChanceShort = _pInstance->Rand_Int(0, 4);
		switch (ChanceShort)
		{
		case 0:
			m_eCurState = BossBat_JumpSmash_Chest;
			break;
		case 1:
			m_eCurState = BossBat_JumpSmashForwardL;
			break;
		case 2:
			m_eCurState = BossBat_Dash;
			break;
		case 3:
			m_eCurState = BossBat_FTurn_L;
			break;
		case 4:
			m_eCurState = BossBat_FTurn_R;
			break;
		}

	}
}

void CExtra01::CurrentRot()
{
	// 그 순간의 회전값을 얻어와서 다시 행렬을 세팅.
	KEYFRAME _tOldKeyFrame = (*(m_pModelCom->Get_AllAnimation()))[BossBat_TurnL90]->Get_OldKeyFrame(m_pModelCom->Get_HierarchyNode("root")->Get_Index());
	_tOldKeyFrame.vScale = m_pTransformCom->Get_Scale();

	XMStoreFloat3(&_tOldKeyFrame.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_tOldKeyFrame.vRotation; // 쿼터니언으로 받아옴 - 새로운 회전 행렬을 만들어준다

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&_tOldKeyFrame.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&_tOldKeyFrame.vRotation), XMVectorSetW(XMLoadFloat3(&_tOldKeyFrame.vPosition), 1.f));
	_float4x4	_World;
	XMStoreFloat4x4(&_World, TransformationMatrix);
	m_pTransformCom->Set_WorldFloat4x4(_World);
}

HRESULT CExtra01::Ready_Components()
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE_02, TEXT("Prototype_Component_Model_Monster_bat"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUS _tStatus;
	_tStatus.fMaxHp = 500.f;
	_tStatus.fAttack = 20.f;
	_tStatus.fHp = _tStatus.fMaxHp;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &_tStatus)))
		return E_FAIL;


	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(6.f, 5.f, 6.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 2.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_BODY], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(6.f, 5.f, 6.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 2.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB_AttBody"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_ATT], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Navigation_Stage_02_1"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CExtra01::SetUp_ShaderResources()
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

void CExtra01::Ready_LimitTime()
{
	//BossBat_AttackL_01_1
	m_vecLimitTime[BossBat_AttackL_01_1].push_back(120.f); // 왼손 on
	m_vecLimitTime[BossBat_AttackL_01_1].push_back(240.f); // 왼손 off

	//BossBat_AttackL_01_2b
	m_vecLimitTime[BossBat_AttackL_01_2b].push_back(60.f); // 왼손 on
	m_vecLimitTime[BossBat_AttackL_01_2b].push_back(230.f); // 왼손 off

	//BossBat_AttackL_01_2b
	m_vecLimitTime[BossBat_AttackL_01_3a].push_back(70.f); // 오른손 on
	m_vecLimitTime[BossBat_AttackL_01_3a].push_back(130.f); // 오른손 off

	//BossBat_AttackL_01_3a
	m_vecLimitTime[BossBat_AttackL_01_3a].push_back(70.f); // 오른손 on
	m_vecLimitTime[BossBat_AttackL_01_3a].push_back(130.f); // 오른손 off

	//BossBat_AttackR_01_1
	m_vecLimitTime[BossBat_AttackR_01_1].push_back(40.f); // 오른손 on
	m_vecLimitTime[BossBat_AttackR_01_1].push_back(120.f); // 오른손 off

	//BossBat_AttackR_01_2b
	m_vecLimitTime[BossBat_AttackR_01_2b].push_back(50.f); // 오른손 on
	m_vecLimitTime[BossBat_AttackR_01_2b].push_back(230.f); // 오른손 off

	//BossBat_Bite_2 -> 여기부터
	m_vecLimitTime[BossBat_Bite_2].push_back(120.f); // 입 on
	m_vecLimitTime[BossBat_Bite_2].push_back(240.f); // 입 off

	//BossBat_Dash -> 여기부터
	m_vecLimitTime[BossBat_Dash].push_back(170.f); // 바디 무기로
	m_vecLimitTime[BossBat_Dash].push_back(400.f); // 바디 다시 피격으로

	//BossBat_HurtXL_R -> 여기부터
	m_vecLimitTime[BossBat_HurtXL_R].push_back(0.f); // 바디 무기로
	m_vecLimitTime[BossBat_HurtXL_R].push_back(10.f); // 바디 다시 피격으로

	//BossBat_HurtXL_L -> 여기부터
	m_vecLimitTime[BossBat_HurtXL_L].push_back(0.f); // 바디 무기로
	m_vecLimitTime[BossBat_HurtXL_L].push_back(10.f); // 바디 다시 피격으로

	//BossBat_JumpSmash_Chest  -> 여기부터
	m_vecLimitTime[BossBat_JumpSmash_Chest].push_back(60.f); // 바디 무기로
	m_vecLimitTime[BossBat_JumpSmash_Chest].push_back(100.f); // 바디 다시 피격으로

	//BossBat_JumpSmashForwardL  -> 여기부터
	m_vecLimitTime[BossBat_JumpSmashForwardL].push_back(150.f); // 바디 무기로
	m_vecLimitTime[BossBat_JumpSmashForwardL].push_back(200.f); // 바디 다시 피격으로
	m_vecLimitTime[BossBat_JumpSmashForwardL].push_back(400.f); // 아이들 상태로

	//BossBat_JumpSmashL  -> 여기부터
	m_vecLimitTime[BossBat_JumpSmashL].push_back(150.f); // 바디 무기로
	m_vecLimitTime[BossBat_JumpSmashL].push_back(200.f); // 바디 다시 피격으로
	m_vecLimitTime[BossBat_JumpSmashL].push_back(400.f); // 아이들 상태로
}

CExtra01 * CExtra01::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CExtra01*		pInstance = new CExtra01(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CExtra01"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMonster * CExtra01::Clone(void * pArg)
{
	CExtra01*		pInstance = new CExtra01(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CExtra01"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExtra01::Free()
{
	__super::Free();

	for (auto& _Collider : m_pColliderCom)
	{
		if (_Collider)
			Safe_Release(_Collider);
	}
}

void CExtra01::Update_Collider()
{
	//if (m_bCollision[COLLIDERTYPE_BODY])

	if (m_bCollision[COLLIDERTYPE_BODY])
	{
		m_pColliderCom[COLLIDERTYPE_BODY]->Update(m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
	}

}

void CExtra01::Check_Stun()
{
	if (m_eMonsterState == CMonster::ATTACK_STUN)
	{
		m_eCurState = BossBat_Stun;
		m_eMonsterState = CMonster::ATTACK_IDLE;
	}
}