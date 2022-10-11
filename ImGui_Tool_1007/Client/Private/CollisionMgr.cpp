#include "stdafx.h"
#include "..\Public\CollisionMgr.h"
#include "OBB.h"
#include "AABB.h"
#include "Sphere.h"
IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}


void CCollisionMgr::Add_CollisoinList(GAMEOBJ_TYPE _GameObjType, COLLSION_TYPE  _CollisionType, CCollider * _pCollider)
{
	m_CollisionList[_GameObjType][_CollisionType].push_back(_pCollider);
	Safe_AddRef(_pCollider);
}

void CCollisionMgr::Clear_CollisoinList()
{
	for (int i = 0; i < GAMEOBJ_END; ++i)
	{
		for (int j = 0; j < COLLISION_END; ++j)
		{
			for (auto& iter : m_CollisionList[i][j])
			{
				Safe_Release(iter);
			}
		}
	}
}

void CCollisionMgr::Tick()
{
	//플레이어가 회피중일땐 안넣으면 그만인데, 패링할때는 어쩌지... 넣어야하는데, 그게...음. 오반데
	//그냥 sr때처럼 각 객체안에서 돌릴까..
	// 콜리젼 틱 돌리면 쓰레드 따로 돌릴 수 있는데
	// 객체 안에서 돌리면 쓰레드 따로 돌릴 수 있나...
	if (PlayerBody_vs_MonsterWeapon())
	{
		return;
	}
	PlayerWeapon_vs_MonsterBody();
	Clear_CollisoinList();
}

_bool CCollisionMgr::PlayerBody_vs_MonsterWeapon()
{
	for (auto& Sour : m_CollisionList[TYPE_PLAYER_BODY][TYPE_OBB])
	{
		for (auto& Dest : m_CollisionList[TYPE_MONSTER_WEAPON][TYPE_OBB])
		{
			if ((COBB*)Sour->Collision(Dest))
			{
				return true;
			}
		}
	}
	return false;
}

void CCollisionMgr::PlayerWeapon_vs_MonsterBody()
{
	for (auto& Sour : m_CollisionList[TYPE_PLAYER_WEAPON][TYPE_OBB])
	{
		for (auto& Dest : m_CollisionList[TYPE_MONSTER_BODY][TYPE_OBB])
		{
			(COBB*)Dest->Collision(Sour);
		}
	}
}

void CCollisionMgr::Free()
{
	for (int i = 0; i < GAMEOBJ_END; ++i)
	{
		for (int j = 0; j < COLLISION_END; ++j)
		{
			for (auto& iter : m_CollisionList[i][j])
			{
				Safe_Release(iter);
			}
		}
	}
}
