#include "stdafx.h"
#include "Puppet.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"
#include "HierarchyNode.h"
#include "Weapon.h"
#include "CollisionMgr.h"
#include "Player.h"
#include "Status.h"
#include "Animation.h"
#include "ImGuiMgr.h"

CPuppet::CPuppet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CPuppet::CPuppet(const CPuppet & rhs)
	: CMonster(rhs)
	, m_AnimPos(rhs.m_AnimPos)
	, m_PreAnimPos(rhs.m_PreAnimPos)
{
}

HRESULT CPuppet::Initialize_Prototype()
{
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;

	m_bAnimStop = true;

	return S_OK;
}

HRESULT CPuppet::Initialize(void * pArg)
{
	m_eMonsterType = MONSTER_EXTRA01;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Ready_LimitTime();
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f, -25.f, 45.f, 1.f));
	m_vLocalPos = _float3(45.f, -25.f, 45.f);
	
	for (int i = 0; i < STATE_END; ++i)
	{
		m_ListNextLook[i] = _float3(45.f,-25.f,45.f);
	}
	//플레이어 로컬좌표에서 빼보자 그러면 원하는 좌표가 나오지 않을까 하는데?
	
	/*m_ListNextLook[Puppet_AttackF_A] = _float3(34.201f - 45.f, -36.2f + 25.f, 22.8f - 45.f);
	m_ListNextLook[Puppet_Combo_F_A_Start] = _float3(33.1f- 45.f, -26.2f+ 25.f, 19.f - 45.f);*/

	m_ListNextLook[Puppet_AttackF_A] = _float3(34.201f - 41.614f, -36.2f - 29.997f, 22.8f - 79.284f);

	m_ListNextLook[Puppet_Combo_F_A_Start] = _float3(29.9f- 41.614f, -26.2f - 29.997f, 20.f - 79.284f);

	//플레이어로부터 떨어진 만큼임
	//즉 이만큼을 플레이어 월드좌표에 더하면 원하는 좌표가 나옴
	//그걸로 타겟 좌표 삼아서 
	
	//로컬의 룩방향 -> 이걸 현재의 회전행렬과 곱해서 월드방향벡터를 만들어준다.
	//쿠드로 하면 현재 가야할 좌표가 나오고

	m_eCurState = Puppet_Idle_F;

	return S_OK;
}

void CPuppet::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		m_eMonsterState = ATTACK_DEAD;
		return;
	}
	AUTOINSTANCE(CGameInstance, _Instance);

	
	if (m_bDead)
		return;

	if (_Instance->KeyDown(DIK_NUMPAD1))
	{
		//증가
		if (m_eCurState + 1 < STATE_END)
			m_eCurState = STATE(m_eCurState + 1);
	}
	else if (_Instance->KeyDown(DIK_NUMPAD2))
	{
		if (m_eCurState - 1 >= 0)
		m_eCurState = STATE(m_eCurState - 1);
	}

	Tick_Imgui();

	if (m_pModelCom != nullptr)
	{
		//Check_Stun();
		CheckAnim();

		
		PlayAnimation(fTimeDelta);
		CheckState(fTimeDelta);
	}

	//Update_Weapon();

	if (m_pParts != nullptr)
		m_pParts->Tick(fTimeDelta, this);
	
	//Update_Collider();
}

void CPuppet::LateTick(_float fTimeDelta)
{
	if (m_bDead)
	{
		RenderGroup();
		return;
	}
	AUTOINSTANCE(CGameInstance, _pInstance);
	if (Collision(fTimeDelta))
	{
		CheckAnim();
		CheckState(fTimeDelta);
		PlayAnimation(fTimeDelta);
	}

	RenderGroup();
}

HRESULT CPuppet::Render()
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
	
	return S_OK;
}

void CPuppet::PlayAnimation(_float fTimeDelta)
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


void CPuppet::CheckEndAnim()
{
	m_fPlaySpeed = 1.f;

	switch (m_eCurState)
	{
	case Client::CPuppet::Puppet_AttackF_A:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Combo_F2R:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Combo_F_A_Start:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Combo_R_Attack01:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Combo_R_Attack02:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Idle_F:
		break;
	case Client::CPuppet::Puppet_IdleGeneral:
		break;
	case Client::CPuppet::Puppet_Step0_FightStart:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step0_OffLineIdle:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step1_1F_Attack01:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step1_1F_Pose:
		break;
	case Client::CPuppet::Puppet_Step2_1FTO1R:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step2_1R_Attack01:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step3_2R_Attack01:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step3_2R_Attack02:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step3_2R_Attack03:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step3_2R_Attack04:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step3_2R_Pose1:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step4_2L_Attack01:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step4_2L_Attack02:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step4_2L_Pose:
		break;
	case Client::CPuppet::Puppet_Step4_2RTO2L:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step5_2B_Attack01:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_Step5_2LTO2B:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_VS_TakeExecution_Attack:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	case Client::CPuppet::Puppet_VS_TakeExecution_End:
		m_eMonsterState = CMonster::ATTACK_IDLE;
		m_eCurState = Puppet_Idle_F;
		break;
	}

	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
}

void CPuppet::CheckState(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	
	switch (m_eCurState)
	{
	case Client::CPuppet::Puppet_AttackF_A:
	
		break;
	case Client::CPuppet::Puppet_Combo_F2R:

		break;
	case Client::CPuppet::Puppet_Combo_F_A_Start:

		break;
	case Client::CPuppet::Puppet_Combo_R_Attack01:

		break;
	case Client::CPuppet::Puppet_Combo_R_Attack02:

		break;
	case Client::CPuppet::Puppet_Idle_F:
		if (m_eMonsterState == CMonster::ATTACK_ATTACK)
		{
			_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (XMVector3Equal(XMLoadFloat3(&m_vAttPos), _vPos))
			{
				// 그 이후 상태를 바꿔줌
				m_eCurState = m_eReserveState;
			}
			else
			{
				// 공격 시 거기에 맞는 좌표로 이동함 : 좌표로 점점 가? 룩을 만들어주고 빠르게 이동
				// 회전해서는 안된다
				m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vNextLook), (fTimeDelta * 5.f));
				
				if (fabs(XMVector3Length(XMLoadFloat3(&m_vAttPos) - _vPos).m128_f32[0]) < 1.5f)
				{
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vAttPos),1.f) );
				}
			}
		}
		else
		{
			_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (XMVector3Equal(_vPos, XMVectorSet(45.f, -25.f, 45.f, 1.f)))
			{
				////도착하면 플레이어 쪽 바라봄
				CTransform* _pPlayerTrans = static_cast<CTransform*>(_pInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));
				m_pTransformCom->LookAt_ForLandObject(_pPlayerTrans->Get_State(CTransform::STATE_POSITION));
			}
			else
			{
				//45,-25,45로 서서히 이동
				m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vNextLook), -(fTimeDelta * 5.f));

				if (fabs(XMVector3Length(XMVectorSet(45.f, -25.f, 45.f, 1.f) - _vPos).m128_f32[0]) < 1.5f)
				{
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f, -25.f, 45.f, 1.f));
				}
			}
		}
		
		
		break;
	case Client::CPuppet::Puppet_IdleGeneral:

		break;
	case Client::CPuppet::Puppet_Step0_FightStart:

		break;
	case Client::CPuppet::Puppet_Step0_OffLineIdle:

		break;
	case Client::CPuppet::Puppet_Step1_1F_Attack01:

		break;
	case Client::CPuppet::Puppet_Step1_1F_Pose:

		break;
	case Client::CPuppet::Puppet_Step2_1FTO1R:

		break;
	case Client::CPuppet::Puppet_Step2_1R_Attack01:

		break;
	case Client::CPuppet::Puppet_Step3_2R_Attack01:

		break;
	case Client::CPuppet::Puppet_Step3_2R_Attack02:

		break;
	case Client::CPuppet::Puppet_Step3_2R_Attack03:

		break;
	case Client::CPuppet::Puppet_Step3_2R_Attack04:

		break;
	case Client::CPuppet::Puppet_Step3_2R_Pose1:

		break;
	case Client::CPuppet::Puppet_Step4_2L_Attack01:

		break;
	case Client::CPuppet::Puppet_Step4_2L_Attack02:

		break;
	case Client::CPuppet::Puppet_Step4_2L_Pose:

		break;
	case Client::CPuppet::Puppet_Step4_2RTO2L:

		break;
	case Client::CPuppet::Puppet_Step5_2B_Attack01:

		break;
	case Client::CPuppet::Puppet_Step5_2LTO2B:

		break;
	case Client::CPuppet::Puppet_VS_TakeExecution_Attack:

		break;
	case Client::CPuppet::Puppet_VS_TakeExecution_End:

		break;
	}

	Get_AnimMat();
}

void CPuppet::CheckLimit()
{
	
}

void CPuppet::Set_Anim(STATE _eState)
{
	m_eCurState = _eState;
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	m_eMonsterState = CMonster::ATTACK_IDLE;
	m_pModelCom->Set_AnimationIndex(m_eCurState);
	m_fPlayTime = 0.f;
}

void CPuppet::CheckAnim()
{
	if (m_ePreState != m_eCurState)
	{
		Set_Anim(m_eCurState);
		m_ePreState = m_eCurState;
	}
}

void CPuppet::Get_AnimMat()
{
	if (m_bAnimStop)
		return;
	_fmatrix _World = m_pTransformCom->Get_WorldMatrix();
	_vector _vPos;
	_vPos = XMVector3TransformCoord(XMLoadFloat4(&m_AnimPos), _World);

	_bool		isMove = true;


	if (true == isMove)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
	}
}

void CPuppet::RenderGroup()
{
	if (nullptr == m_pRendererCom)
		return;
	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

_bool CPuppet::Collision(_float fTimeDelta)
{
	CGameObject* _pTarget = nullptr;


	AUTOINSTANCE(CGameInstance, _instance);



	

	return false;
}

void CPuppet::On_Collider(EXTRA01COLLIDER _eCollider, _bool _bCollision)
{
	/*switch (_eCollider)
	{
	case Client::CPuppet::COLLIDERTYPE_BODY:
		if (m_bCollision[COLLIDERTYPE_BODY] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
		break;
	}*/
}

void CPuppet::Look_Move_Player(_float _fPosX, _float _fPosZ)
{
	// 애니메이션 진행 중에 턴하는 함수

	AUTOINSTANCE(CGameInstance, _Intance);
	CTransform* _Trans =
		static_cast<CTransform*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));

	CNavigation* _pNavi =
		static_cast<CNavigation*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Navigation")));

	_vector _vTargetPos = _Trans->Get_State(CTransform::STATE_POSITION);
	_vTargetPos.m128_f32[0] += _fPosX;
	_vTargetPos.m128_f32[2] += _fPosZ;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vTargetPos);

	m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));
}

void CPuppet::Look_Player()
{
	//한번에 바라보는거
	AUTOINSTANCE(CGameInstance, _Intance);
	CTransform* _Trans =
		static_cast<CTransform*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));

	m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));
}

_bool CPuppet::InRange()
{

	AUTOINSTANCE(CGameInstance, _pInstance);
	CPlayer* _pPlayer = static_cast<CPlayer*>(_pInstance->Get_Player());
	CTransform* _pTransform = static_cast<CTransform*>(_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")));
	_vector _vToPlayer = _pTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_float _fLimitAngle = sqrtf(3.f) / 2.f;
	_float _fLimitDis = 7.f;

	if (m_bPreStateAtt)
	{
		_fLimitDis = 20.f;
		if (XMVector3Length(_vToPlayer).m128_f32[0] > _fLimitDis)
		{
			return false;
		}
		_vector _vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_vector _vDestLook = XMVector3Normalize(_vToPlayer);

		_float _fAngle = XMVector3Dot(_vLook, _vDestLook).m128_f32[0];

		Pattern();

		return true;
	}
	else
	{
		if (XMVector3Length(_vToPlayer).m128_f32[0] > _fLimitDis)
		{
			return false;
		}

		_vector _vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_vector _vDestLook = XMVector3Normalize(_vToPlayer);

		_float _fAngle = XMVector3Dot(_vLook, _vDestLook).m128_f32[0];


		if (_fAngle > _fLimitAngle)//안에 들어오면 그리로 ... 뛰어가야되는데?
		{
			Pattern();
			return true;
		}

	}
	return false;
}

void CPuppet::Pattern()
{
	//거리에 따라 확률적으로 공격하고 날라댕기고
	// 거리 짧으면 또 그러고 어키어키
	AUTOINSTANCE(CGameInstance, _pInstance);

	CTransform* _pPlayerTransform = static_cast<CTransform*>(_pInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));
	_vector		_pLook = _pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float _fDistance = XMVector3Length(_pLook).m128_f32[0];

	

	// 공격 패턴 짜기 시작
	//플레이어와의 거리
	
}

HRESULT CPuppet::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Monster_Puppet"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUS _tStatus;
	_tStatus.fMaxHp = 80.f;
	_tStatus.fAttack = 7.f;
	_tStatus.fHp = _tStatus.fMaxHp;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &_tStatus)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPuppet::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CPuppet::Ready_LimitTime()
{
	
}

CPuppet * CPuppet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPuppet*		pInstance = new CPuppet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPuppet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMonster * CPuppet::Clone(void * pArg)
{
	CPuppet*		pInstance = new CPuppet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPuppet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPuppet::Free()
{
	__super::Free();

	for (auto& _Collider : m_pColliderCom)
	{
		if (_Collider)
			Safe_Release(_Collider);
	}
	Safe_Release(m_pParts);
	Safe_Release(m_pSockets);
}

void CPuppet::Tick_Imgui()
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	CImGui::Get_Instance()->Begin("Puppet");
	if (_pInstance->KeyPressing(DIK_NUMPAD8))
		_vPos.m128_f32[2] += 0.1f;
	if (_pInstance->KeyPressing(DIK_NUMPAD5))
		_vPos.m128_f32[2] -= 0.1f;

	if (_pInstance->KeyPressing(DIK_NUMPAD4))
		_vPos.m128_f32[0] -= 0.1f;
	if (_pInstance->KeyPressing(DIK_NUMPAD6))
		_vPos.m128_f32[0] += 0.1f;

	if (_pInstance->KeyPressing(DIK_NUMPAD9))
		_vPos.m128_f32[1] += 0.1f;
	if (_pInstance->KeyPressing(DIK_NUMPAD7))
		_vPos.m128_f32[1] -= 0.1f;

	int _iState = int(m_eCurState);
	CImGui::Get_Instance()->Intcheck(&(_iState), "state");
	m_eCurState = (STATE)_iState;



	CImGui::Get_Instance()->floatcheck(&(_vPos.m128_f32[0]), "X");
	CImGui::Get_Instance()->floatcheck(&(_vPos.m128_f32[1]), "Y");
	CImGui::Get_Instance()->floatcheck(&(_vPos.m128_f32[2]), "Z");

	_float3 vRot = m_pTransformCom->Get_Rotation();

	CImGui::Get_Instance()->floatcheck(&(vRot.y), "Angle");

	if (CImGui::Get_Instance()->Button("Start"))
	{
		_matrix _RotMatrix = m_pTransformCom->Get_WorldMatrix();
		_RotMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		XMStoreFloat3(&m_vAttPos, XMVector3TransformCoord(XMLoadFloat3(&m_ListNextLook[Puppet_Combo_F_A_Start]), _RotMatrix)); //좌표
		
		m_eReserveState = Puppet_Combo_F_A_Start;
		m_eMonsterState = CMonster::ATTACK_ATTACK;

		_vector _vPlayerPos = static_cast<CTransform*>(_pInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
		
		XMStoreFloat3(&m_vAttPos, _vPlayerPos + XMLoadFloat3(&m_vAttPos));
		XMStoreFloat3(&m_vNextLook, XMVector3Normalize( XMLoadFloat3(&m_vAttPos) - XMLoadFloat3(&m_vLocalPos)));
	}

	m_pTransformCom->Set_Rotation(XMLoadFloat3(& vRot));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(vRot.y));
	CImGui::Get_Instance()->End();
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
}

void CPuppet::Update_Collider()
{
	//if (m_bCollision[COLLIDERTYPE_BODY])
	

}

void CPuppet::Check_Stun()
{
	if (m_eMonsterState == CMonster::ATTACK_STUN)
	{
		//m_eCurState = LV1Villager_M_HurtCounter;
		m_eMonsterState = CMonster::ATTACK_IDLE;
	}
}

HRESULT CPuppet::Ready_Weapon()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("weapon_r_IK");
	if (nullptr == pWeaponSocket)
		return E_FAIL;
	m_pSockets = pWeaponSocket;
	Safe_AddRef(m_pSockets);



	/* For.Sword */
	CWeapon*		pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_MonsterAxe"), this));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pParts = pGameObject;

	return S_OK;
}

HRESULT CPuppet::Update_Weapon()
{
	if (nullptr == m_pSockets)
		return E_FAIL;

	/* 행렬. */
	/*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
	* 뼈의 행렬(CombinedTransformation)
	* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/
	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Player */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	_matrix WeaponMatrix = /*m_pSockets[PART_CANE]->Get_OffSetMatrix()**/
		m_pSockets->Get_CombinedTransformation()
		* XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())
		* m_pTransformCom->Get_WorldMatrix();

	m_pParts->SetUp_State(WeaponMatrix);

	return S_OK;
}
