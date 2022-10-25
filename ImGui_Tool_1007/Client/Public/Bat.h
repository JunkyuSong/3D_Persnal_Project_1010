#pragma once

#include "Client_Defines.h"
#include "Monster.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CAnimModel;
class CHierarchyNode;
END

BEGIN(Client)

/*
플레이어가 룩의 일정 범위 안에 있으면 턴 안하고
일정 범위 안에 없으면 그 범위 안에 들어올때까지 턴한다
*/

class CBat final : public CMonster
{
public:
	enum MAGICIANCOLLIDER { COLLIDERTYPE_BODY, COLLIDERTYPE_HAND_R, COLLIDERTYPE_HAND_L, COLLILDERTYPE_END };
	enum STATE {
		BossBat_AttackL_01_1,
		BossBat_AttackL_01_2b,
		BossBat_AttackL_01_3a,
		BossBat_AttackR_01_1,
		BossBat_AttackR_01_2b,
		BossBat_Bite_2,
		BossBat_Dash,
		BossBat_FTurn_L,
		BossBat_FTurn_R,
		BossBat_HurtXL_R,
		BossBat_HurtXL_L,
		BossBat_Idle,
		BossBat_JumpSmash_Chest,
		BossBat_JumpSmashForwardL,
		BossBat_JumpSmashL,
		BossBat_FightStart,
		BossBat_Stun,
		BossBat_TakeExecution_End,
		BossBat_TakeExecution_DeadStart01,
		BossBat_TurnL90,
		BossBat_TurnR90,
		BossBat_WalkF,
		STATE_END
	};
private:
	enum DIRECT { DIR_F, DIR_B, DIR_R, DIR_L, DIR_FR, DIR_BR, DIR_FL, DIR_BL, DIR_END };

private:
	CBat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBat(const CBat& rhs);
	virtual ~CBat() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	void PlayAnimation(_float fTimeDelta);

public:
	void		Set_AnimState(STATE	_eState) { m_eCurState = _eState; }
	void		Set_AnimReserveState(STATE	_eState) { m_eReserveState = _eState; }
	STATE*		Get_AnimState() { return &m_eCurState; }
	STATE*		Get_AnimReserveState() { return &m_eReserveState; }

private:
	void CheckEndAnim();
	void Set_Anim(STATE _eState);
	void CheckAnim();
	void CheckState(_float fTimeDelta);
	
	void CheckLimit();

	void Get_AnimMat();

	void RenderGroup();
	_bool Collision(_float fTimeDelta);

	void On_Collider(MAGICIANCOLLIDER _eCollider, _bool _bCollision);

	void Look_Move_Player(_float _fPosX, _float _fPosZ);
	void Look_Player();

	void Turn(_float fTimeDelta);
	void CurrentRot();

	_bool	m_bAgainAnim = false;

private:
	STATE					m_eReserveState = STATE_END;
	STATE					m_eCurState = BossBat_Idle;
	STATE					m_ePreState = STATE_END;

	_float4					m_AnimPos;
	_float4					m_PreAnimPos;

	_bool					m_bAnimStop = false;
	_float					m_fPlayTime = 0.f;
	_float					m_fPlaySpeed = 1.f;
	vector<_float>			m_vecLimitTime[STATE_END];

	CCollider*				m_pColliderCom[COLLILDERTYPE_END] = { nullptr };
	_bool					m_bCollision[COLLILDERTYPE_END] = { false };

	DIRECT					m_eDir = DIR_END;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();

	void	Ready_LimitTime();

	void	Update_Collider();

	void	Check_Stun();

private:
	_float	m_fStay = 0.f;


public:
	static CBat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMonster* Clone(void* pArg);
	virtual void Free() override;
};

END