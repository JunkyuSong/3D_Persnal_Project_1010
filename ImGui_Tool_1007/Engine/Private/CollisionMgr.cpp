
#include "..\Public\CollisionMgr.h"
#include "GameInstance.h"
#include "GameObject.h"
//#include "OBB.h"
//#include "AABB.h"
//#include "Sphere.h"
IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}


void CCollisionMgr::Add_CollisoinList(GAMEOBJ_TYPE _GameObjType, CCollider * _pCollider, CGameObject* _pObj)
{
	pair<CGameObject*, CCollider*> _pair = { _pObj , _pCollider };
	m_CollisionList[_GameObjType].push_back(_pair);
	Safe_AddRef(_pObj);
	Safe_AddRef(_pCollider);
}

void CCollisionMgr::Clear_CollisoinList()
{
	for (int i = 0; i < GAMEOBJ_END; ++i)
	{		
		for (auto& iter : m_CollisionList[i])
		{
			iter.second->Clear_Target();
			Safe_Release(iter.first);
			Safe_Release(iter.second);
		}
		m_CollisionList[i].clear();		
	}
}

void CCollisionMgr::Tick()
{
	//ƽ�� �����ǵ� �׷� �̰� �������� �����ϰ�, Ÿ���� ä���ְ�.
	// Ʈ�縦 ������ �ݶ��̴��� Ÿ���� �޾ƿͼ� �װɷ� ����ó�� ���ְ�
	// ƽ Ŭ�����Ҷ�(����Ʈ ƽ ����) Ŭ���� Ÿ�ٵ� ���༭ ������ ���ְ�
	// �ణ �����ѵ�, ��ſ� Ȯ���ϱ� �ѵ� �浹�� �Ǿ��ٶ�°� Ÿ���� ������� �ƴ����� üũ���༭ ������ �ɵ�.
	// ���� ����

	// 1. �÷��̾� �ٵ� vs ���� ����

	PlayerParry_vs_MonsterBody();

	if (MonsterWeapon_vs_PlayerBody())
		return;
	
	// 2. ���� �ٵ� vs �÷��̾� ����
	PlayerWeapon_vs_MonsterBody();
}

CGameObject * CCollisionMgr::Collision(GAMEOBJ_TYPE _GameObjType, CCollider * _pCollider)
{
	switch (_pCollider->Get_ColliderType())
	{
	case CCollider::TYPE_AABB:
		{
			CAABB* _Collider = static_cast<CAABB*>(_pCollider);
			for (auto& _pair : m_CollisionList[_GameObjType])
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
			for (auto& _pair : m_CollisionList[_GameObjType])
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
			for (auto& _pair : m_CollisionList[_GameObjType])
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

void CCollisionMgr::PlayerWeapon_vs_MonsterBody()
{
	for (auto& _WeaponPair : m_CollisionList[TYPE_PLAYER_WEAPON])
	{

		switch (_WeaponPair.second->Get_ColliderType())
		{
		case CCollider::TYPE_AABB:
		{
			CAABB* _Collider = static_cast<CAABB*>(_WeaponPair.second);
			for (auto& _Body_Pair : m_CollisionList[TYPE_MONSTER_BODY])
			{
				if (_Collider->Collision(_Body_Pair.second))
				{
					_Collider->Set_Target(_Body_Pair.first);
					_Body_Pair.second->Set_Target(_WeaponPair.first);
				}
			}
		}
		break;
		case CCollider::TYPE_OBB:
		{
			COBB* _Collider = static_cast<COBB*>(_WeaponPair.second);
			for (auto& _Body_Pair : m_CollisionList[TYPE_MONSTER_BODY])
			{
				if (_Collider->Collision(_Body_Pair.second))
				{
					_Collider->Set_Target(_Body_Pair.first);
					_Body_Pair.second->Set_Target(_WeaponPair.first);
				}
			}
		}
		break;
		case CCollider::TYPE_SPHERE:
		{
			CSphere* _Collider = static_cast<CSphere*>(_WeaponPair.second);
			for (auto& _Body_Pair : m_CollisionList[TYPE_MONSTER_BODY])
			{
				if (_Collider->Collision(_Body_Pair.second))
				{
					_Collider->Set_Target(_Body_Pair.first);
					_Body_Pair.second->Set_Target(_WeaponPair.first);
				}
			}
		}
		break;
		}
	}	
}

void CCollisionMgr::PlayerParry_vs_MonsterBody()
{
	for (auto& _WeaponPair : m_CollisionList[TYPE_MONSTER_WEAPON])
	{

		switch (_WeaponPair.second->Get_ColliderType())
		{
		case CCollider::TYPE_AABB:
		{
			CAABB* _Collider = static_cast<CAABB*>(_WeaponPair.second);
			for (auto& _Body_Pair : m_CollisionList[TYPE_PLAYER_PARRY])
			{
				if (_Collider->Collision(_Body_Pair.second))
				{
					_Collider->Set_Target(_Body_Pair.first);
					_Body_Pair.second->Set_Target(_WeaponPair.first);
					return;
				}
			}
		}
		break;
		case CCollider::TYPE_OBB:
		{
			COBB* _Collider = static_cast<COBB*>(_WeaponPair.second);
			for (auto& _Body_Pair : m_CollisionList[TYPE_PLAYER_PARRY])
			{
				if (_Collider->Collision(_Body_Pair.second))
				{
					_Collider->Set_Target(_Body_Pair.first);
					_Body_Pair.second->Set_Target(_WeaponPair.first);
					return;
				}
			}
		}
		break;
		case CCollider::TYPE_SPHERE:
		{
			CSphere* _Collider = static_cast<CSphere*>(_WeaponPair.second);
			for (auto& _Body_Pair : m_CollisionList[TYPE_PLAYER_PARRY])
			{
				if (_Collider->Collision(_Body_Pair.second))
				{
					_Collider->Set_Target(_Body_Pair.first);
					_Body_Pair.second->Set_Target(_WeaponPair.first);
					return;
				}
			}
		}
		break;
		}
	}
}

_bool CCollisionMgr::MonsterWeapon_vs_PlayerBody()
{
	for (auto& _WeaponPair : m_CollisionList[TYPE_MONSTER_WEAPON])
	{

		switch (_WeaponPair.second->Get_ColliderType())
		{
		case CCollider::TYPE_AABB:
		{
			CAABB* _Collider = static_cast<CAABB*>(_WeaponPair.second);
			for (auto& _Body_Pair : m_CollisionList[TYPE_PLAYER_BODY])
			{
				if (_Collider->Collision(_Body_Pair.second))
				{
					_Collider->Set_Target(_Body_Pair.first);
					_Body_Pair.second->Set_Target(_WeaponPair.first);
					return true;
				}
			}
		}
		break;
		case CCollider::TYPE_OBB:
		{
			COBB* _Collider = static_cast<COBB*>(_WeaponPair.second);
			for (auto& _Body_Pair : m_CollisionList[TYPE_PLAYER_BODY])
			{
				if (_Collider->Collision(_Body_Pair.second))
				{
					_Collider->Set_Target(_Body_Pair.first);
					_Body_Pair.second->Set_Target(_WeaponPair.first);
					return true;
				}
			}
		}
		break;
		case CCollider::TYPE_SPHERE:
		{
			CSphere* _Collider = static_cast<CSphere*>(_WeaponPair.second);
			for (auto& _Body_Pair : m_CollisionList[TYPE_PLAYER_BODY])
			{
				if (_Collider->Collision(_Body_Pair.second))
				{
					_Collider->Set_Target(_Body_Pair.first);
					_Body_Pair.second->Set_Target(_WeaponPair.first);
					return true;
				}
			}
		}
		break;
		}
	}
	return false;
}

void CCollisionMgr::Free()
{
	for (int i = 0; i < GAMEOBJ_END; ++i)
	{
		for (auto& iter : m_CollisionList[i])
		{
			Safe_Release(iter.first);
			Safe_Release(iter.second);
		}
		m_CollisionList[i].clear();
	}
}
