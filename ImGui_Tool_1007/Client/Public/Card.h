#pragma once

#include "Client_Defines.h"
#include "Weapon.h"


BEGIN(Client)

class CCard final : public CWeapon
{
private:
	CCard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCard(const CCard& rhs);
	virtual ~CCard() = default;

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
	static CCard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	_float3		m_vOriginPos;
};

END