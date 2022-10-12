#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CAnimModel;
class CMotionTrail;
class CHierarchyNode;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:	
	enum STATE {
		STATE_ATT1, STATE_ATT2, STATE_ATT3, STATE_ATT4, STATE_ATT5,
		STATE_RUN_B, STATE_RUN_F, STATE_RUN_L, STATE_RUN_R,
		STATE_APPROACH, STATE_IDLE, STATE_WALK, 
		STATE_AVOIDATTACK, STATE_JUMPAVOID, STATE_AVOIDBACK,
		Corvus_PW_Axe,
		Tackle_F,
		ParryR,
		ParryL,
		DualKnife,
		GreatSword,
		PW_Halberds,
		PW_Hammer_A,
		PW_TwinSwords_1, PW_TwinSwords_2, 
		PW_VargSword_A,
		PW_Bow_Start, PW_Bow_End,
		PW_Bow_B,
		PW_Bloodwhip,
		PW_CaneSword_SP01, PW_CaneSword_SP02,
		Healing_Little, Healing2_Blend,
		Raven_ClawCommonV2_ChargeStart,
		Raven_ClawLong_ChargeFull,
		Raven_ClawNear,
		Strong_Jump,
		RavenAttack_Start, RavenAttack_End,
		SD_ParryToMob,
		SD_HurtIdle,
		SD_StrongHurt_Start, SD_StrongHurt_End,
		SD_Dead,
		STATE_END
	};

private:
	enum DIRECT { DIR_F, DIR_B, DIR_R, DIR_L, DIR_FR, DIR_BR, DIR_FL, DIR_BL, DIR_END };
	enum COLLIDERTYPE { COLLIDERTYPE_AABB, COLLIDERTYPE_OBB_MONSTER, COLLIDERTYPE_SPHERE_ITEM, COLLILDERTYPE_END };
	enum ATTACKLIMIT { ATTACKLIMIT_CHANGE, ATTACKLIMIT_TRAILON, ATTACKLIMIT_TRAILEND, ATTACKLIMIT_COLLIDERON, ATTACKLIMIT_COLLIDEREND, ATTACKLIMIT_END };

public:
	struct OBJ_DESC
	{
		LEVEL		eLevel;
		_float4x4	matWorld;
		_tchar*		szModelTag;
	};
	enum PASS { PASS_NONPICK, PASS_PICK, PASS_END };
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick( _float fTimeDelta);
	virtual void LateTick( _float fTimeDelta);
	virtual HRESULT Render();
	void PlayAnimation( _float fTimeDelta);

public:
	void		Set_Info(OBJ_DESC _tInfo);
	OBJ_DESC	Get_Info(OBJ_DESC _tInfo) { return m_tInfo; }

	void Set_Pass(PASS _ePass) { m_ePass = _ePass; }

	void		ImGuiTick();
	void		Set_AnimState(STATE	_eState)			{ m_eCurState = _eState; }
	void		Set_AnimReserveState(STATE	_eState)	{ m_eReserveState = _eState; }
	STATE*		Get_AnimState()							{ return &m_eCurState; }
	STATE*		Get_AnimReserveState()					{ return &m_eReserveState; }
	_int		Get_MaxState()							{ return (int)STATE_END - 1; }
	void		Set_Stop(_bool _bStop)					{ m_bAnimStop = _bStop; }

private:
	HRESULT Ready_AnimLimit();
	void	CheckEndAnim();

	void	Set_Anim(STATE _eState);
	void	CheckAnim();
	void	CheckState();
	void	AfterAnim();

	void	CheckLimit();
	_bool	CheckLimit_Att(STATE _eAtt);
	void	Change_Anim();

	void	Get_AnimMat();



private:
	enum BASE { BASE_SABER, BASE_DAGGER, BASE_END };
	enum SKILL {SKILL_AXE, SKILL_DUAL, SKILL_END};
	enum HAND	{HAND_RIGHT, HAND_LEFT, HAND_END};

	typedef vector<class CWeapon*>		PARTS;
	class CHierarchyNode*				m_pHands[HAND_END];
	_float4x4							m_matHands[HAND_END];
	
	PARTS								m_pBaseParts;
	vector<_uint>						m_pBaseHands;

	PARTS								m_pSkillParts[SKILL_END];	
	vector<_uint>						m_pSkillHands[SKILL_END];

	_bool								m_bSkill = false;
	SKILL								m_eCurSkill = SKILL_END;

private:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CAnimModel*				m_pModelCom = nullptr;

	OBJ_DESC				m_tInfo;

	PASS					m_ePass;

	STATE					m_eReserveState = STATE_END;
	STATE					m_eCurState = STATE_IDLE;
	STATE					m_ePreState = STATE_END;

	_float4					m_AnimPos;
	_float4					m_PreAnimPos;

	_bool					m_bAnimStop = false;
	_float					m_fPlayTime = 0.f;
	vector<_float>			m_vecLimitTime[STATE_END];

	_float					m_fAnimSpeed = 1.f;

	DIRECT					m_eDir = DIR_END;

	list<class CDummy*>		m_listMotion;
	list<class CDummy*>		m_listDeadMotion;
	_bool					m_bMotionPlay = false;
	_float					m_MaxMotionTrail = 0.07f;
	_float					m_CurMotionTrail = 0.f;

	
	void (CPlayer::*KeyInput[STATE_END])(_float);

	CCollider*				m_pColliderCom[COLLILDERTYPE_END] = { nullptr };
	_bool					m_bTrail = false;

private:
	HRESULT	Check_MotionTrail(_float fTimeDelta);

	HRESULT Ready_Components();
	
	HRESULT Ready_Collider();
	HRESULT SetUp_ShaderResources();

	HRESULT Ready_Hands();
	HRESULT Ready_PlayerParts();
	HRESULT Ready_PlayerParts_Skill();
	HRESULT Ready_PlayerParts_Base();

	HRESULT	Update_Weapon(_float fTimeDelta);
	HRESULT Update_Hands_Matrix();
	HRESULT Update_Weapon_Base();
	HRESULT Update_Weapon_Skill();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	void KeySetting();

	void KeyInputMain(_float fTimeDelta);

	void KeyInput_Idle(_float fTimeDelta);

	void Move(_float fTimeDelta);

	void KP_ATT(_float fTimeDelta);

	void KP_Parry(_float fTimeDelta);
	void KP_AVOIDATTACK(_float fTimeDelta);
	void KP_ClawNear(_float fTimeDelta);

};

END