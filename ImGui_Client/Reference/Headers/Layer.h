#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;
public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag, _uint iLayerIndex);
	class CGameObject* Get_ObjFromLayer(const _uint _iIndex)
	{
		if (_iIndex >= m_GameObjects.size())
		{
			return nullptr;
		}
		auto& iter = m_GameObjects.begin();
		for (_uint i = 0; i < _iIndex; ++iter);
		return *iter;
	}
	list<class CGameObject*>* Get_ListFromLayer()
	{
		return &m_GameObjects;
	}

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);

	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

private:
	list<class CGameObject*>				m_GameObjects;
	typedef list<class CGameObject*>		GAMEOBJECTS;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END