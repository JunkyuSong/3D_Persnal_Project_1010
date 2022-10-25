#include "stdafx.h"
#include "..\Public\Boss_Bat.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"
#include "HierarchyNode.h"
#include "Weapon.h"
#include "CollisionMgr.h"
#include "Player.h"
#include "Status.h"
#include "Animation.h"

CBoss_Bat::CBoss_Bat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CBoss_Bat::CBoss_Bat(const CBoss_Bat & rhs)
	: CMonster(rhs)
	, m_AnimPos(rhs.m_AnimPos)
	, m_PreAnimPos(rhs.m_PreAnimPos)
{
}

HRESULT CBoss_Bat::Initialize_Prototype()
{
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;

	m_bAnimStop = true;

	return S_OK;
}

HRESULT CBoss_Bat::Initialize(void * pArg)
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

void CBoss_Bat::Tick(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, _Instance);
	if (_Instance->KeyDown(DIK_NUMPAD1))
	{
		m_eCurState = BossBat_Idle;
	}
	else if (_Instance->KeyDown(DIK_NUMPAD2))
	{
		m_eCurState = BossBat_TurnL90;
	}
	else if (_Instance->KeyDown(DIK_NUMPAD3))
	{
		m_eCurState = BossBat_AttackL_01_3a;
	}
	else if (_Instance->KeyDown(DIK_NUMPAD4))
	{
		m_eCurState = BossBat_AttackR_01_1;
	}

	if (m_pModelCom != nullptr)
	{
		Check_Stun();
		CheckAnim();

		CheckState(fTimeDelta);
		PlayAnimation(fTimeDelta);
	}

	Update_Collider();
}

void CBoss_Bat::LateTick(_float fTimeDelta)
{
	if (Collision(fTimeDelta))
	{
		CheckAnim();
		CheckState(fTimeDelta);
		PlayAnimation(fTimeDelta);
	}

	RenderGroup();
}

HRESULT CBoss_Bat::Render()
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



		if (FAILED(m_pModelCom->Render(m_pShaderCom, 3, i)))
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

void CBoss_Bat::PlayAnimation(_float fTimeDelta)
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


void CBoss_Bat::CheckEndAnim()
{
	m_fPlaySpeed = 1.f;



	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
}

void CBoss_Bat::CheckState(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CBoss_Bat::BossBat_AttackL_01_1:
		break;
	case Client::CBoss_Bat::BossBat_AttackL_01_2b:
		break;
	case Client::CBoss_Bat::BossBat_AttackL_01_3a:
		break;
	case Client::CBoss_Bat::BossBat_AttackR_01_1:
		break;
	case Client::CBoss_Bat::BossBat_AttackR_01_2b:
		break;
	case Client::CBoss_Bat::BossBat_Bite_2:
		break;
	case Client::CBoss_Bat::BossBat_Dash:
		break;
	case Client::CBoss_Bat::BossBat_FTurn_L:
		break;
	case Client::CBoss_Bat::BossBat_FTurn_R:
		break;
	case Client::CBoss_Bat::BossBat_HurtXL_R:
		break;
	case Client::CBoss_Bat::BossBat_HurtXL_L:
		break;
	case Client::CBoss_Bat::BossBat_Idle:
		break;
	case Client::CBoss_Bat::BossBat_JumpSmash_Chest:
		break;
	case Client::CBoss_Bat::BossBat_JumpSmashForwardL:
		break;
	case Client::CBoss_Bat::BossBat_JumpSmashL:
		break;
	case Client::CBoss_Bat::BossBat_FightStart:
		break;
	case Client::CBoss_Bat::BossBat_Stun:
		break;
	case Client::CBoss_Bat::BossBat_TakeExecution_End:
		break;
	case Client::CBoss_Bat::BossBat_TakeExecution_DeadStart01:
		break;
	case Client::CBoss_Bat::BossBat_TurnL90:
		break;
	case Client::CBoss_Bat::BossBat_TurnR90:
		break;
	case Client::CBoss_Bat::BossBat_WalkF:
		break;
	case Client::CBoss_Bat::STATE_END:
		break;
	default:
		break;
	}
	Get_AnimMat();
}

void CBoss_Bat::CheckLimit()
{

}

void CBoss_Bat::Set_Anim(STATE _eState)
{
	m_eCurState = _eState;
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	m_eMonsterState = CMonster::ATTACK_IDLE;
	m_pModelCom->Set_AnimationIndex(m_eCurState);
	m_fPlayTime = 0.f;
}

void CBoss_Bat::CheckAnim()
{
	if (m_ePreState != m_eCurState)
	{
		if (m_ePreState == BossBat_TurnL90)
		{
			//아이들 상태로 바꿔주는 작업...? 아니지 들어온 애니메이션의 첫 상태로 싹 세팅해주는 작업이 필요함.

			// 싹 돌되
		}
		Set_Anim(m_eCurState);
		m_ePreState = m_eCurState;
	}
}

void CBoss_Bat::Get_AnimMat()
{
	if (m_bAnimStop)
		return;
	_fmatrix _World = m_pTransformCom->Get_WorldMatrix();
	_vector _vPos;
	_vPos = XMVector3TransformCoord(XMLoadFloat4(&m_AnimPos), _World);

	_bool		isMove = true;

	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != m_pNavigationCom)
		isMove = m_pNavigationCom->isMove(_vPos, &vNormal);

	if (true == isMove)
	{
		_vPos.m128_f32[1] = m_pNavigationCom->Get_PosY(_vPos);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
	}
}

void CBoss_Bat::RenderGroup()
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

_bool CBoss_Bat::Collision(_float fTimeDelta)
{
	CGameObject* _pTarget = nullptr;

	AUTOINSTANCE(CGameInstance, _instance);

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
		//문제 : 어느 각도에서 맞았냐에 따라!
		// 라이트와 내적하자.

		_vector _vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

		CTransform* _pTransform = static_cast<CTransform*>(_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")));
		_vector _vHitLook = XMVector3Normalize(_pTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		_float _fAngle = XMVector3Dot(_vRight, _vHitLook).m128_f32[0];

		if (_fAngle > 0.f)
		{
			//오른쪽에서 피격
			if (m_eCurState == BossBat_HurtXL_R)
			{
				if (m_bAgainAnim == false)
				{
					m_bAgainAnim = true;
					Set_Anim(m_eCurState);
				}
			}
			else
			{
				m_eCurState = BossBat_HurtXL_R;
			}
		}
		else
		{
			//왼쪽에서 피격
			if (m_eCurState == BossBat_HurtXL_L)
			{
				if (m_bAgainAnim == false)
				{
					m_bAgainAnim = true;
					Set_Anim(m_eCurState);
				}
			}
			else
			{
				m_eCurState = BossBat_HurtXL_L;
			}
		}

		return true;
	}
	return false;
}

void CBoss_Bat::On_Collider(MAGICIANCOLLIDER _eCollider, _bool _bCollision)
{
	switch (_eCollider)
	{
	case Client::CBoss_Bat::COLLIDERTYPE_BODY:
		if (m_bCollision[COLLIDERTYPE_BODY] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
		break;
	case Client::CBoss_Bat::COLLIDERTYPE_HAND_R:
		if (m_bCollision[COLLIDERTYPE_HAND_R] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_HAND_R], this);
		break;
	case Client::CBoss_Bat::COLLIDERTYPE_HAND_L:
		if (m_bCollision[COLLIDERTYPE_HAND_L] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_HAND_L], this);
		break;
	}
}

void CBoss_Bat::Look_Move_Player(_float _fPosX, _float _fPosZ)
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

void CBoss_Bat::Look_Player()
{
	//한번에 바라보는거
	AUTOINSTANCE(CGameInstance, _Intance);
	CTransform* _Trans =
		static_cast<CTransform*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));

	m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));
}

void CBoss_Bat::Turn(_float fTimeDelta)
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

	if (fabs(_fAngle) < sqrtf(3.f) / 2.f)
	{
		//패턴
	}
	else
	{
		//회전 - 뭐가 회전하고 있는지 알아야하니까


	}
}

void CBoss_Bat::CurrentRot()
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

HRESULT CBoss_Bat::Ready_Components()
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
	ColliderDesc.vSize = _float3(0.7f, 1.8f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_BODY], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	_float3 vCenter = _float3(m_pModelCom->Get_HierarchyNode("middle_01_r")->Get_Trans()._41,
		m_pModelCom->Get_HierarchyNode("middle_01_r")->Get_Trans()._42,
		m_pModelCom->Get_HierarchyNode("middle_01_r")->Get_Trans()._43);
	ColliderDesc.vCenter = vCenter;
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB_Hand_R"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_HAND_R], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	vCenter = _float3(m_pModelCom->Get_HierarchyNode("middle_01_l")->Get_Trans()._41,
		m_pModelCom->Get_HierarchyNode("middle_01_l")->Get_Trans()._42,
		m_pModelCom->Get_HierarchyNode("middle_01_l")->Get_Trans()._43);
	ColliderDesc.vCenter = vCenter;
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB_Hand_L"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_HAND_L], &ColliderDesc)))
		return E_FAIL;


	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_STAGE_02, TEXT("Prototype_Component_Navigation_Stage_02"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBoss_Bat::SetUp_ShaderResources()
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

void CBoss_Bat::Ready_LimitTime()
{

}

CBoss_Bat * CBoss_Bat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Bat*		pInstance = new CBoss_Bat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBoss_Bat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMonster * CBoss_Bat::Clone(void * pArg)
{
	CBoss_Bat*		pInstance = new CBoss_Bat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBoss_Bat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Bat::Free()
{
	__super::Free();

	for (auto& _Collider : m_pColliderCom)
	{
		if (_Collider)
			Safe_Release(_Collider);
	}
}

void CBoss_Bat::Update_Collider()
{
	//if (m_bCollision[COLLIDERTYPE_BODY])

	if (m_bCollision[COLLIDERTYPE_BODY])
	{
		m_pColliderCom[COLLIDERTYPE_BODY]->Update(m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
	}

	//if (m_bCollision[COLLIDERTYPE_HAND_R])
	{
		m_pColliderCom[COLLIDERTYPE_HAND_R]->Update(m_pModelCom->Get_HierarchyNode("middle_01_r")->Get_CombinedTransformation()*XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())*m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_HAND_R], this);
	}

	//if (m_bCollision[COLLIDERTYPE_HAND_L])
	{
		m_pColliderCom[COLLIDERTYPE_HAND_L]->Update(m_pModelCom->Get_HierarchyNode("middle_01_l")->Get_CombinedTransformation()*XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())*m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_HAND_L], this);
	}
}

void CBoss_Bat::Check_Stun()
{
	if (m_eMonsterState == CMonster::ATTACK_STUN)
	{
		m_eCurState = BossBat_Stun;
		m_eMonsterState = CMonster::ATTACK_IDLE;
	}
}