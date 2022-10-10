#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CNonAnimModel;
END

BEGIN(Client)

class CSaber final : public CWeapon
{
private:
	CSaber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSaber(const CSaber& rhs);
	virtual ~CSaber() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	




private:
	HRESULT Ready_Components();

public:
	static CSaber* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END