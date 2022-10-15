#pragma once

/* 특정 레벨에 사용되는 객체들응ㄹ 모아서 보관해둔다. */
/* 보관하고 잇는 개체들의 Tick를 호출해준다. */

#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iLayerIndex);


public:
	HRESULT Reserve_Container(_uint iNumLevels);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr, CGameObject** pOut = nullptr);
	HRESULT Add_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	map<const _tchar*, class CLayer*>  Get_Layers(_uint iLevelIndex);

public:
	void Tick(const _float& fTimeDelta);
	void LateTick(const _float& fTimeDelta);
	void Clear(const _uint& iLevelIndex);
	
public: //Tool
	class CLayer*									Get_Layer(_uint iLevelIndex, _tchar* _pLayerTag);


private: /* 원형객체들을 보관할 컨테이너 */
	typedef map<const _tchar*, class CGameObject*>	PROTOTYPES;
	PROTOTYPES										m_Prototypes;


private: /* 사본객체들을 보관할 컨테이너 */
	_uint											m_iNumLevels = 0;
	typedef map<const _tchar*, class CLayer*>		LAYERS;
	LAYERS*											m_pLayers = nullptr;

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END