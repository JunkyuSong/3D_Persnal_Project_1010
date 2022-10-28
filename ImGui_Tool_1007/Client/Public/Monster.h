#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CAnimModel;
class CCollider;
class CTransform;
class CNavigation;
END

BEGIN(Client)
class CMonster abstract:
	public CGameObject
{
public:
	struct MONSTERINFO
	{
		_vector _vPos;
		_uint	_iIndex;
	};
public:
	enum ATTACK { ATTACK_ATTACK, ATTACK_IDLE, ATTACK_PARRY, ATTACK_AVOID, ATTACK_STUN, ATTACK_DEFENCE, ATTACK_DISAPPEAR, ATTACK_DEAD, ATTACK_END};
	enum MONSTERTYPE {MONSTER_MAGIC,MONSTER_BAT,MONSTER_EXTRA01, MONSTER_EXTRA02, MONSTER_PUPPET,MONSTER_END};
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	ATTACK					Get_MonsterState() { return m_eMonsterState; }
	void					Set_MonsterState(ATTACK _eMonsterState) { m_eMonsterState = _eMonsterState; }

	_bool	Get_Coll_Target() {
		return m_Coll_Target;
	}
	void	Set_Coll_Target(_bool _Coll_Target) { m_Coll_Target = _Coll_Target; }

	MONSTERTYPE	Get_Type() { return m_eMonsterType; }

protected:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CAnimModel*				m_pModelCom = nullptr;

	class CStatus*			m_pStatusCom = nullptr;

	ATTACK					m_eMonsterState = ATTACK_IDLE;

	CNavigation*			m_pNavigationCom = nullptr;

	_bool					m_Coll_Target = false;

	MONSTERTYPE				m_eMonsterType = MONSTER_END;

public:
	virtual void Free() override;
};

END