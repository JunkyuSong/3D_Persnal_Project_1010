#pragma once

#include "Base.h"

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPipeLine	final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Player(CGameObject* _pPlayer) { m_pPlayer = _pPlayer; }
	CGameObject*	Get_Player() { return m_pPlayer; }

	void Set_Transform(TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);

	_matrix Get_TransformMatrix(TRANSFORMSTATE eTransformState) const {
		return XMLoadFloat4x4(&m_TransformMatrix[eTransformState]);
	}
	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATE eTransformState) const {
		return m_TransformMatrix[eTransformState];
	}
	_float4x4 Get_TransformFloat4x4_TP(TRANSFORMSTATE eTransformState) const {
		_float4x4		Transform;
		XMStoreFloat4x4(&Transform, XMMatrixTranspose(Get_TransformMatrix(eTransformState)));
		return Transform;
	}

	_matrix Get_TransformMatrixInverse(const TRANSFORMSTATE& eTransformState) const {
		return XMLoadFloat4x4(&m_TransformInverseMatrix[eTransformState]);
	}


	void Update();

	_float4 Get_CamPosition() const {
		return m_vCamPosition;
	}
	
private:
	_float4x4				m_TransformMatrix[D3DTS_END];
	_float4x4				m_TransformInverseMatrix[D3DTS_END];
	_float4					m_vCamPosition;

	CGameObject*			m_pPlayer;

public:
	virtual void Free() override;
};

END