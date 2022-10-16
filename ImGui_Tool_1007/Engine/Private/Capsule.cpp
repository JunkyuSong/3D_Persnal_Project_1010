#include "..\Public\Capsule.h"
#include "PipeLine.h"

#include "AABB.h"
#include "OBB.h"
#include "Sphere.h"

CCapsule::CCapsule(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

CCapsule::CCapsule(const CCapsule & rhs)
	: CCollider(rhs)
	, m_pCapsule(rhs.m_pCapsule)
{

}

HRESULT CCapsule::Initialize_Prototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::Initialize_Prototype(eColliderType)))
		return E_FAIL;
	m_eColliderType = CCollider::TYPE_CAPSULE;
	return S_OK;
}

HRESULT CCapsule::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//일단 가장 높은 부분, 낮은 부분
	//그리고 반지름


	_matrix		RotationMatrix = XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
		XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
		XMMatrixRotationZ(m_ColliderDesc.vRotation.z);
	vOriginal_Center = m_ColliderDesc.vCenter;
	_vector _vDir = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), RotationMatrix);

	XMStoreFloat3(&m_Top, XMLoadFloat3(&m_ColliderDesc.vCenter) + (m_ColliderDesc.vSize.y  * 0.5f * _vDir));
	XMStoreFloat3(&m_Bottom, XMLoadFloat3(&m_ColliderDesc.vCenter) - (m_ColliderDesc.vSize.y  * 0.5f * _vDir));


	return S_OK;
}

void CCapsule::Update(_fmatrix TransformMatrix)
{
	XMStoreFloat3(&m_ColliderDesc.vCenter, XMVector3TransformCoord(XMLoadFloat3(&vOriginal_Center), TransformMatrix));

	_matrix fd = TransformMatrix;
	fd.r[3].m128_f32[0] = 0.f;
	fd.r[3].m128_f32[1] = 0.f;
	fd.r[3].m128_f32[2] = 0.f;

	fd.r[0] = XMVector3Normalize(fd.r[0]);
	fd.r[1] = XMVector3Normalize(fd.r[1]);
	fd.r[2] = XMVector3Normalize(fd.r[2]);

	_vector _vDir = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), fd);

	XMStoreFloat3(&m_Top, XMLoadFloat3(&m_ColliderDesc.vCenter) + (m_ColliderDesc.vSize.y * 0.5f * _vDir));
	XMStoreFloat3(&m_Bottom, XMLoadFloat3(&m_ColliderDesc.vCenter) - (m_ColliderDesc.vSize.y * 0.5f * _vDir));

	return;
}

_bool CCapsule::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_ColliderType();

	m_isColl = false;
	//_float3 _vCenter;
	BoundingSphere _Sphere;
	switch (eType)
	{
	case CCollider::TYPE_AABB:
		_Sphere = Get_Sphere(pTargetCollider, eType);
		//_Sphere.Center = _vCenter;
		m_isColl = _Sphere.Intersects(((CAABB*)pTargetCollider)->Get_Collider());
		break;
	case CCollider::TYPE_OBB:
		_Sphere = Get_Sphere(pTargetCollider, eType);
		//_Sphere.Center = _vCenter;
		m_isColl = _Sphere.Intersects(((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		_Sphere = Get_Sphere(pTargetCollider, eType);
		//_Sphere.Center = _vCenter;
		m_isColl = _Sphere.Intersects(((CSphere*)pTargetCollider)->Get_Collider());
		break;
	case CCollider::TYPE_CAPSULE:
		m_isColl = CapsuleCollision((CCapsule*)pTargetCollider);
		break;
	}


	return m_isColl;
}

_bool CCapsule::CapsuleCollision(CCollider * pTargetCollider)
{
	_vector _Top = XMLoadFloat3(&m_Top);
	_vector _Bottom = XMLoadFloat3(&m_Bottom);

	_vector _Normal = XMVector3Normalize(_Top - _Bottom);
	_vector _LineEdge = _Normal * m_ColliderDesc.vSize.x / 2.f;
	_vector _LineTop = _Top - _LineEdge;
	_vector _LineBottom = _Bottom + _LineEdge;
	_Normal = XMVector3Normalize(_LineBottom - _LineTop);

	// Target Capsule
	CCapsule* pTarget = (CCapsule*)pTargetCollider;
	_vector _pTarget_Top = XMLoadFloat3(&pTarget->m_Top);
	_vector _pTarget_Bottom = XMLoadFloat3(&pTarget->m_Bottom);

	_vector _pTarget_Normal = XMVector3Normalize(_pTarget_Top - _pTarget_Bottom);
	_vector _pTarget_LineEdge = _pTarget_Normal * pTarget->m_ColliderDesc.vSize.x / 2.f;
	_vector _pTarget_LineTop = _pTarget_Top - _pTarget_LineEdge;
	_vector _pTarget_LineBottom = _pTarget_Bottom + _pTarget_LineEdge;
	_pTarget_Normal = XMVector3Normalize(_pTarget_LineBottom - _pTarget_LineTop);

	_vector vDistance0 = _pTarget_LineTop - _LineTop;
	_vector vDistance1 = _pTarget_LineBottom - _LineTop;
	_vector vDistance2 = _pTarget_LineTop - _LineBottom;
	_vector vDistance3 = _pTarget_LineBottom - _LineBottom;

	_float _fDistance0 = fabs(XMVectorGetX(XMVector3Length(vDistance0)));
	_float _fDistance1 = fabs(XMVectorGetX(XMVector3Length(vDistance1)));
	_float _fDistance2 = fabs(XMVectorGetX(XMVector3Length(vDistance2)));
	_float _fDistance3 = fabs(XMVectorGetX(XMVector3Length(vDistance3)));

	_vector Point, TargetPoint;
	if (_fDistance2 < _fDistance0 || _fDistance2 < _fDistance1 || _fDistance3 < _fDistance0 || _fDistance3 < _fDistance1)
		Point = _LineBottom;
	else
		Point = _LineTop;

	_float _Ratio = XMVectorGetX(XMVector3Dot(Point - _pTarget_LineTop, _pTarget_Normal));
	TargetPoint = _pTarget_LineTop + (_Ratio * _pTarget_Normal);

	_Ratio = XMVectorGetX(XMVector3Dot(TargetPoint - _LineTop, _Normal));
	Point = _LineTop + (_Ratio * _Normal);

	_vector _vSphereDistance = TargetPoint - Point;
	_float _fSphereDistance = fabs(XMVectorGetX(XMVector3Length(_vSphereDistance)));

	if (_fSphereDistance < (m_ColliderDesc.vSize.x + pTarget->m_ColliderDesc.vSize.x) / 2.f)
	{
		XMStoreFloat3(&m_vDir, _vSphereDistance);
		m_fDis = _fSphereDistance - (m_ColliderDesc.vSize.x + pTarget->m_ColliderDesc.vSize.x) / 2.f;
		return true;
	}
		

	return false;
}

BoundingSphere CCapsule::Get_Sphere(CCollider * pTargetCollider, TYPE _eType)
{
	// this Capsule
	_vector _Top = XMLoadFloat3(&m_Top);
	_vector _Bottom = XMLoadFloat3(&m_Bottom);

	_vector _Normal = XMVector3Normalize(_Bottom - _Top);
	_vector _LineEdge = _Normal * m_ColliderDesc.vSize.x / 2.f;
	_vector _LineTop = _Top - _LineEdge;
	_vector _LineBottom = _Bottom + _LineEdge;

	_Normal = XMVector3Normalize(_LineBottom - _LineTop);

	_vector _TargetCenter;
	_float3 _float3_TargetCenter;
	
	switch (_eType)
	{
	case Engine::CCollider::TYPE_AABB:
		_float3_TargetCenter = static_cast<CAABB*>(pTargetCollider)->Get_Collider().Center;
		_TargetCenter = XMLoadFloat3(&_float3_TargetCenter);
		break;
	case Engine::CCollider::TYPE_OBB:
		_float3_TargetCenter = static_cast<COBB*>(pTargetCollider)->Get_Collider().Center;
		_TargetCenter = XMLoadFloat3(&_float3_TargetCenter);
		break;
	case Engine::CCollider::TYPE_SPHERE:
		_float3_TargetCenter = static_cast<CSphere*>(pTargetCollider)->Get_Collider().Center;
		_TargetCenter = XMLoadFloat3(&_float3_TargetCenter);
		break;
	}	

	_float _Ratio = XMVectorGetX(XMVector3Dot(_TargetCenter - _LineTop, _Normal));
	_float3 _vCenter;
	XMStoreFloat3(&_vCenter, _Top + _Ratio * _Normal);
	return BoundingSphere(_vCenter, m_ColliderDesc.vSize.x);
}

#ifdef _DEBUG

HRESULT CCapsule::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();
	
	//DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}

#endif // _DEBUG

CCapsule * CCapsule::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	CCapsule*			pInstance = new CCapsule(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : CAABB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCapsule::Clone(void * pArg)
{
	CCapsule*			pInstance = new CCapsule(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAABB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CCapsule::Free()
{
	__super::Free();

	//Safe_Delete(m_pOriginal_AABB);
	//Safe_Delete(m_pAABB);
}
