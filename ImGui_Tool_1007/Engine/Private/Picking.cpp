#include "..\Public\Picking.h"
#include "Transform.h"

#include "PipeLine.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}


HRESULT CPicking::Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	m_hWnd = hWnd;

	m_iWinCX = iWinCX;

	m_iWinCY = iWinCY;

	return S_OK;
}

void CPicking::Tick()
{
	POINT			ptMouse;

	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	_vector			vMousePos = XMVectorSet(_float(ptMouse.x / (m_iWinCX * 0.5f) - 1), _float(ptMouse.y / (m_iWinCY * -0.5f) + 1),0.f,1.f);

	/* 투영공간상의 마우스 좌표를 구한다. */

	//XMVectorSetX(vMousePos, );
	//XMVectorSetY(vMousePos, );
	//XMVectorSetZ(vMousePos, 0.f);

	/* 뷰스페이스 상의 좌표를 구한다. */
	_matrix		ProjMatrixInv;

	ProjMatrixInv = XMMatrixInverse(nullptr, CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

	/* 뷰스페이스 상의 마우스 레이와 레이포스를 구하자. */
	m_vRayPos = _float3(0.f, 0.f, 0.f);

	XMStoreFloat3(&m_vRayDir, vMousePos - XMLoadFloat3(&m_vRayPos));

	/* 월드스페이스 상의 마우스 레이와 레이포스를 구하자. */
	_matrix		ViewMatrixInv;
	ViewMatrixInv = XMMatrixInverse(nullptr, CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	
	XMStoreFloat3(&m_vRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), ViewMatrixInv));

	XMStoreFloat3(&m_vRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), ViewMatrixInv));
	
}

void CPicking::Compute_LocalRayInfo(_vector& pRayDir, _vector& pRayPos, CTransform * pTransform)
{
	_matrix		WorldMatrixInv = XMMatrixIdentity();
	if (pTransform)
		WorldMatrixInv = pTransform->Get_WorldMatrixInverse();
	//XMStoreFloat3(pRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInv));
	pRayPos = XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInv);
	//XMStoreFloat3(pRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInv));
	pRayDir = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInv);
}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
