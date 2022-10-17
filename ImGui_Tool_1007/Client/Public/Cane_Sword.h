#pragma once

#include "Client_Defines.h"
#include "Weapon.h"


BEGIN(Client)

class CCane_Sword final : public CWeapon
{
private:
	CCane_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCane_Sword(const CCane_Sword& rhs);
	virtual ~CCane_Sword() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta, CGameObject* _pUser);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();





private:
	HRESULT Ready_Components();

public:
	static CCane_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	_float*		m_fParentAlpha = nullptr;
};

END