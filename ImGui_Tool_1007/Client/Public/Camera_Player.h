#pragma once

#include "Client_Defines.h"
#include "Client_Camere.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CCamera_Player final : public CClient_Camere
{
private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Get_Target(CTransform* _pTarget) { m_pTarget = _pTarget; }
	void Check_Dis();
	void Targeting();

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CGameObject*	m_pPlayer = nullptr;
	CTransform	*	m_pTarget = nullptr;
	_float3			m_vDis;
	_float			m_fAngleY;
	CNavigation*	m_pNaviCom = nullptr;
};

END