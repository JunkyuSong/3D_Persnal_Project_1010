#pragma once

#include "Client_Defines.h"
#include "Monster.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CAnimModel;
class CHierarchyNode;
END

BEGIN(Client)

class CMagician final : public CMonster
{
public:
	enum DIRECT { DIR_F, DIR_B, DIR_R, DIR_L, DIR_FR, DIR_BR, DIR_FL, DIR_BL, DIR_END };
	enum STATE {
		Magician_Idle, Magician_Idle2,
		Hurt_Short, Hurt_Long,
		Boss_Enter,
		Cane_Att1, Cane_Att2,
		SP_Att1_Start, SP_Att1_Suc,
		SP_Att2_Start, SP_Att2_Loop, SP_Att2_Suc,
		Appear_L, Appear_R, Appear_B, Appear_F,
		Cane_Att3,
		Kick_Combo,
		Walk_B, Walk_F, Walk_L, Walk_R,
		STATE_END
	};

	enum PART { PART_CANE, PART_CANESWORD, PART_END };

private:
	CMagician(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMagician(const CMagician& rhs);
	virtual ~CMagician() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick( _float fTimeDelta);
	virtual void LateTick( _float fTimeDelta);
	virtual HRESULT Render();
	void PlayAnimation( _float fTimeDelta);

public:
	void		Set_AnimState(STATE	_eState)			{ m_eCurState = _eState; }
	void		Set_AnimReserveState(STATE	_eState)	{ m_eReserveState = _eState; }
	STATE*		Get_AnimState()							{ return &m_eCurState; }
	STATE*		Get_AnimReserveState()					{ return &m_eReserveState; }

private:

	void CheckEndAnim();
	void Set_Anim(STATE _eState);
	void CheckAnim();
	void CheckState();

	void Get_AnimMat();

private:
	STATE					m_eReserveState = STATE_END;
	STATE					m_eCurState = Magician_Idle;
	STATE					m_ePreState = STATE_END;

	_float4					m_AnimPos;
	_float4					m_PreAnimPos;

	_bool					m_bAnimStop = false;
	_float					m_fPlayTime = 0.f;
	vector<_float>			m_vecLimitTime[STATE_END];

	_float					m_fAnimSpeed = 1.f;

	DIRECT					m_eDir = DIR_END;

	typedef vector<CGameObject*>		PARTS;
	PARTS								m_pParts;
	vector<class CHierarchyNode*>		m_pSockets;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();

	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();
	HRESULT Update_Weapon();

public:
	static CMagician* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMonster* Clone(void* pArg);
	virtual void Free() override;
};

END