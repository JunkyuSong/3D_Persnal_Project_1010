#pragma once
#include "Base.h"
#include "Client_Defines.h"

#include "GameObject.h"
#include "Collider.h"

BEGIN(Client)

class CCollisionMgr :
	public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

public:
	enum GAMEOBJ_TYPE {
		TYPE_PLAYER_BODY, TYPE_PLAYER_WEAPON, TYPE_MONSTER_BODY, TYPE_MONSTER_WEAPON, TYPE_ITEM, GAMEOBJ_END
	};
	enum COLLSION_TYPE {
		TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_CAPSULE, COLLISION_END
	};

private:
	CCollisionMgr();
	virtual ~CCollisionMgr() = default;

public:
	void	Add_CollisoinList(GAMEOBJ_TYPE _GameObjType, COLLSION_TYPE  _CollisionType, CCollider * _pCollider);
	void	Clear_CollisoinList();
	void	Tick();

	_bool	PlayerBody_vs_MonsterWeapon();
	void	PlayerWeapon_vs_MonsterBody();

	list<CCollider*>	Get_CollisionList(GAMEOBJ_TYPE _GameObjType, COLLSION_TYPE _CollisionType)
	{
		return m_CollisionList[_GameObjType][_CollisionType];
	}

public:
	virtual void Free() override;

private:
	list<CCollider*>	m_CollisionList[GAMEOBJ_END][COLLISION_END];
};

END