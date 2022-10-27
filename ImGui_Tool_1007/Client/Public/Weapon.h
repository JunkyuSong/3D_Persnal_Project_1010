#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CNonAnimModel;
END

BEGIN(Client)
class CWeapon abstract:
	public CGameObject
{
protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta, CGameObject* _pUser);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual HRESULT SetUp_State(_fmatrix StateMatrix);

	_bool	Trail_GetOn();
	void	TrailOn();
	void	TrailOff();
	void	Set_CollisionOn(_bool _bColliderOn) { m_bColliderOn = _bColliderOn; }



protected:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pParentTransformCom = nullptr;
	CNonAnimModel*			m_pModelCom = nullptr;

	CCollider*				m_pColliderCom = nullptr;

	TRAILPOS				m_tTrailPos;
	class CTrail*			m_pTrailCom = nullptr;

	_bool					m_bColliderOn = false;


public:
	virtual void Free() override;

	

};

END