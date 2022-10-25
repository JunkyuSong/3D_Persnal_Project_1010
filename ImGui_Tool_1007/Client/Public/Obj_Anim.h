#pragma once

#include "Client_Defines.h"
#include "Obj_Plus.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CAnimModel;
END

BEGIN(Client)

class CObj_Anim final : public CObj_Plus
{
public:
	enum DIRECT { DIR_F, DIR_B, DIR_R, DIR_L, DIR_FR, DIR_BR, DIR_FL, DIR_BL, DIR_END };
	enum STATE {
		STATE_ATT1, STATE_ATT2, STATE_ATT3, STATE_ATT4, STATE_ATT5,
		STATE_RUN_B, STATE_RUN_F, STATE_RUN_L, STATE_RUN_R,
		STATE_APPROACH, STATE_IDLE, STATE_WALK, STATE_AVOIDATTACK, STATE_END = 100
	};

private:
	CObj_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObj_Anim(const CObj_Anim& rhs);
	virtual ~CObj_Anim() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	void PlayAnimation(_float fTimeDelta);

public:
	virtual void		Set_Info(OBJ_DESC _tInfo) override;

	void		ImGuiTick();
	void		Set_AnimState(STATE	_eState) { m_eCurState = _eState; }
	void		Set_AnimReserveState(STATE	_eState) { m_eReserveState = _eState; }
	STATE*		Get_AnimState() { return &m_eCurState; }
	STATE*		Get_AnimReserveState() { return &m_eReserveState; }
	_int		Get_MaxState() { return (int)STATE_END - 1; }
	void		Set_Stop(_bool _bStop) { m_bAnimStop = _bStop; }

private:

	void CheckEndAnim();
	void Set_Anim(STATE _eState);
	void CheckAnim();
	void CheckState();

	void Get_AnimMat();

private:
	CAnimModel*				m_pModelCom = nullptr;

	STATE					m_eReserveState = STATE_END;
	STATE					m_eCurState = STATE_ATT1;
	STATE					m_ePreState = STATE_END;

	_float4					m_AnimPos;
	_float4					m_PreAnimPos;

	_bool					m_bAgainAnim = false;

	_bool					m_bAnimStop = false;
	_float					m_fPlayTime = 0.f;
	vector<_float>			m_vecLimitTime[STATE_END];

	_float					m_fAnimSpeed = 1.f;

	DIRECT					m_eDir = DIR_END;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CObj_Anim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END