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
	m_eColliderType = CCollider::TYPE_AABB;
	return S_OK;
}

HRESULT CCapsule::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	/* 복제될때 셋팅하고자하는 상태로 갱신한다. */
	//m_pOriginal_Capsule = new BoundingBox(m_ColliderDesc.vCenter, _float3(m_ColliderDesc.vSize.x * 0.5f,
	//	m_ColliderDesc.vSize.y * 0.5f, 
	//	m_ColliderDesc.vSize.z * 0.5f));

	//m_pCapsule = new BoundingBox(*m_pOriginal_pCapsule);

	return S_OK;
}

void CCapsule::Update(_fmatrix TransformMatrix)
{
	//m_pOriginal_AABB->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
}

_bool CCapsule::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_ColliderType();

	m_isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		//m_isColl = m_pAABB->Intersects(((CCapsule*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_OBB:
		//m_isColl = m_pAABB->Intersects(((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		//m_isColl = m_pAABB->Intersects(((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}	

	return m_isColl;
}

//#ifdef _DEBUG

//HRESULT CCapsule::Render()
//{
//	if (FAILED(__super::Render()))
//		return E_FAIL;
//
//	m_pBatch->Begin();
//	
//	//DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));
//
//	m_pBatch->End();
//
//	return S_OK;
//}
//
//#endif // _DEBUG

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
