#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CAnimModel;
class CTransform;
END

BEGIN(Client)
class CMonster abstract:
	public CGameObject
{
public:
	enum ATTACK { ATTACK_ATTACK, ATTACK_IDLE, ATTACK_PARRY, ATTACK_AVOID, ATTACK_STUN, ATTACK_DEFENCE, ATTACK_END};

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	ATTACK					Get_MonsterState() { return m_eMonsterState; }
	void					Set_MonsterState(ATTACK _eMonsterState) { m_eMonsterState = _eMonsterState; }

protected:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CAnimModel*				m_pModelCom = nullptr;

	class CStatus*			m_pStatusCom = nullptr;

	ATTACK					m_eMonsterState = ATTACK_IDLE;


public:
	virtual void Free() override;
};

END