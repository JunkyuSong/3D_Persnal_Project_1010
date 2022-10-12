#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CDagger final : public CWeapon
{
private:
	CDagger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDagger(const CDagger& rhs);
	virtual ~CDagger() = default;

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
	static CDagger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END