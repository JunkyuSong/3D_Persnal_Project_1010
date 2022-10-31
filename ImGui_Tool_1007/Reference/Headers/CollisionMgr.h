#pragma once
#include "Base.h"

#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollisionMgr :
	public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

public:
	enum GAMEOBJ_TYPE {
		TYPE_PLAYER_BODY, TYPE_PLAYER_WEAPON, TYPE_PLAYER_PARRY, TYPE_PLAYER_PUSH,
		TYPE_MONSTER_BODY, TYPE_MONSTER_WEAPON, TYPE_MONSTER_PUSH,
		TYPE_ITEM,
		GAMEOBJ_END
	};
	/*enum COLLSION_TYPE {
		TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_CAPSULE, COLLISION_END
	};*/

private:
	CCollisionMgr();
	virtual ~CCollisionMgr() = default;

public:
	void	Add_CollisoinList(GAMEOBJ_TYPE _GameObjType, CCollider * _pCollider, CGameObject* _pObj);
	void	Clear_CollisoinList();
	void	Tick();


	CGameObject*	Collision(GAMEOBJ_TYPE _GameObjType, CCollider * _pCollider);

private:
	void	PlayerWeapon_vs_MonsterBody();
	void	MonsterBody_vs_MonsterBody();
	void	PlayerBody_vs_MonsterBody();	
	void	PlayerParry_vs_MonsterWeapon();
	_bool	MonsterWeapon_vs_PlayerBody();


public:
	virtual void Free() override;

private:
	list<pair<CGameObject*,CCollider*>>	m_CollisionList[GAMEOBJ_END];
};

END