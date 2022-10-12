#include "stdafx.h"
#include "..\Public\CollisionMgr.h"
#include "GameInstance.h"
//#include "OBB.h"
//#include "AABB.h"
//#include "Sphere.h"
IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}


void CCollisionMgr::Add_CollisoinList(GAMEOBJ_TYPE _GameObjType, COLLSION_TYPE  _CollisionType, CCollider * _pCollider, CGameObject* _pObj)
{
	pair<CGameObject*, CCollider*> _pair = { _pObj , _pCollider };
	m_CollisionList[_GameObjType][_CollisionType].push_back(_pair);
	Safe_AddRef(_pObj);
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
				Safe_Release(iter.first);
				Safe_Release(iter.second);
			}
			m_CollisionList[i][j].clear();
		}
	}
}

CGameObject * CCollisionMgr::Collision(GAMEOBJ_TYPE _GameObjType, COLLSION_TYPE _CollisionType, CCollider * _pCollider)
{
	switch (_pCollider->Get_ColliderType())
	{
	case CCollider::TYPE_AABB:
		{
			CAABB* _Collider = static_cast<CAABB*>(_pCollider);
			for (auto& _pair : m_CollisionList[_GameObjType][_CollisionType])
			{
				if (_Collider->Collision(_pair.second))
				{
					return _pair.first;
				}
			}
		}
		break;
	case CCollider::TYPE_OBB:
		{
			COBB* _Collider = static_cast<COBB*>(_pCollider);
			for (auto& _pair : m_CollisionList[_GameObjType][_CollisionType])
			{
				if (_Collider->Collision(_pair.second))
				{
					return _pair.first;
				}
			}
		}
		break;
	case CCollider::TYPE_SPHERE:
		{
			CSphere* _Collider = static_cast<CSphere*>(_pCollider);
			for (auto& _pair : m_CollisionList[_GameObjType][_CollisionType])
			{
				if (_Collider->Collision(_pair.second))
				{
					return _pair.first;
				}
			}
		}
		break;
	//case CCollider::TYPE_CAPSULE:
	//{
	//	/*CCap* _Collider = static_cast<CAABB*>(_pCollider);
	//	for (auto& _pair : m_CollisionList[_GameObjType][_CollisionType])
	//	{
	//		_Collider->Collision(_pair.second);
	//	}*/
	//}
	//	break;
	}
	
	
	return nullptr;
}

void CCollisionMgr::Free()
{
	for (int i = 0; i < GAMEOBJ_END; ++i)
	{
		for (int j = 0; j < COLLISION_END; ++j)
		{
			for (auto& iter : m_CollisionList[i][j])
			{
				Safe_Release(iter.first);
				Safe_Release(iter.second);
			}
			m_CollisionList[i][j].clear();
		}		
	}
}
