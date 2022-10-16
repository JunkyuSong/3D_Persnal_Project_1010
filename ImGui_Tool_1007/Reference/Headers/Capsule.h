#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCapsule final : public CCollider
{
protected:
	CCapsule(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCapsule(const CCapsule& rhs);
	virtual ~CCapsule() = default;

public:
	BoundingBox Get_Collider() {
		return *m_pCapsule;
	}
	

public:
	virtual HRESULT Initialize_Prototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);

	_bool	CapsuleCollision(CCollider* pTargetCollider);
	BoundingSphere	Get_Sphere(CCollider* pTargetCollider, TYPE _eType);

	void	Set_Dis(_float _fDis) {
		m_fDis = _fDis;
	}
	void	Set_Dir(_float3 _vDir) {
		m_vDir = _vDir;
	}

	_float	Get_Dis() { return m_fDis; }
	_float3	Get_Dir() { return m_vDir; }

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG

private:
	BoundingBox*			m_pOriginal_Capsule = nullptr;
	BoundingBox*			m_pCapsule = nullptr;

	_float3					m_Top;
	_float3					m_Bottom;

	_float3					vOriginal_Center;

	_float3					m_vDir;
	_float					m_fDis;



public:
	static CCapsule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;


};

END