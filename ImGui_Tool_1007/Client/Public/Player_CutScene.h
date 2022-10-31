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

class CPlayer_CutScene final : public CObj_Plus
{
public:
	enum CUTSCENE {
		CUT_STAGE01_START,
		CUT_STAGE01_END,
		CUT_STAGE02_01_START,
		CUT_STAGE02_01_END,
		CUT_STAGE02_START,
		CUT_STAGE02_END,
		CUT_STAGE03_START,
		CUT_STAGE03_END,
		CUT_END
	};

private:
	CPlayer_CutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_CutScene(const CPlayer_CutScene& rhs);
	virtual ~CPlayer_CutScene() = default;

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
	void		Set_AnimState(CUTSCENE	_eState) { m_eCurState = _eState; }
	void		Set_AnimReserveState(CUTSCENE	_eState) { m_eReserveState = _eState; }
	_int		Get_MaxState() { return (int)CUT_END - 1; }
	void		Set_Stop(_bool _bStop) { m_bAnimStop = _bStop; }

private:

	void CheckEndAnim();
	void Set_Anim(CUTSCENE _eState);
	void CheckAnim();
	void CheckState();

	void Get_AnimMat();

private:
	CAnimModel*				m_pModelCom = nullptr;

	CUTSCENE				m_eReserveState = CUT_END;
	CUTSCENE				m_eCurState = CUT_STAGE01_START;
	CUTSCENE					m_ePreState = CUT_END;

	_float4					m_AnimPos;
	_float4					m_PreAnimPos;

	_bool					m_bAgainAnim = false;

	_bool					m_bAnimStop = false;
	_float					m_fPlayTime = 0.f;
	vector<_float>			m_vecLimitTime[CUT_END];

	_float					m_fAnimSpeed = 1.f;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CPlayer_CutScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END