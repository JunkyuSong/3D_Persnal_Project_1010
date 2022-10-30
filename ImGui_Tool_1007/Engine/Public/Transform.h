#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct tagTransformDesc
	{
		_float			fSpeedPerSec;
		_float			fRotationPerSec;
	}TRANSFORMDESC;


private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4 Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}

	void	Set_WorldFloat4x4(_float4x4 _WorldMatrix)
	{
		m_WorldMatrix = _WorldMatrix;
	}

	_float4x4 Get_WorldFloat4x4_TP() const {
		_float4x4	WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return WorldMatrix;
	}

	_matrix Get_WorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, Get_WorldMatrix());
	}

	void Set_State(STATE eState, _fvector vState);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr);

	void Go_Up(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Down(_float fTimeDelta, class CNavigation* pNavigation = nullptr);

	void Go_Dir(_vector _vDir, _float fTimeDelta);

	void Set_Scale(_fvector vScaleInfo);
	_float3 Get_Scale();

	void Turn(_fvector vAxis, _float fTimeDelta);
	void Turn(_vector StartLook, _vector EndLook, _float _fRatio);
	void Turn_Angle(_fvector vAxis, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);

	void LookAt(_fvector vAt);
	void LookAt_ForLandObject(_fvector vAt);
	void Move(_fvector vTargetPos, _float fTimeDelta, _float fLimitDistance = 0.1f);

	void Set_Rotation(_fvector _vRotation) { XMStoreFloat3(&m_vRotation, _vRotation); }
	_float3 Get_Rotation() { return m_vRotation; };

	void Set_PrePos(_float3 _vPrePos) { m_vPrePos = _vPrePos; }
	void Rewind_Pos() { memcpy(m_WorldMatrix.m[3], &m_vPrePos, sizeof(_float3)); }

private:
	_float4x4				m_WorldMatrix;
	_float3					m_vPrePos;
	TRANSFORMDESC			m_TransformDesc;
	_float3					m_vRotation;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END