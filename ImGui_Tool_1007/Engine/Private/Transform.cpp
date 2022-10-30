#include "..\Public\Transform.h"
#include "Navigation.h"
CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_vRotation(rhs.m_vRotation)
	, m_vPrePos(rhs.m_WorldMatrix.m[3])
{
	
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix	WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

HRESULT CTransform::Initialize_Prototype()
{
	/* vector -> float : XMStore*/
	/* float -> vector : XMLoad */

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	XMStoreFloat3(&m_vRotation, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	
	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	_bool		isMove = true;

	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != pNavigation)
	{
		isMove = pNavigation->isMove(vPosition, &vNormal);

		if (true == isMove)
		{
			vPosition.m128_f32[1] = pNavigation->Get_PosY(vPosition);
			Set_State(CTransform::STATE_POSITION, vPosition);
		}
		else
		{

			if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
				return;
			_vector		vPosition = Get_State(CTransform::STATE_POSITION);
			_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook) * (-1.f), vNormal)) * vNormal;
			vSlide += XMVector3Normalize(vLook);
			vPosition += vSlide * m_TransformDesc.fSpeedPerSec * fTimeDelta;
			vPosition.m128_f32[1] = pNavigation->Get_PosY(vPosition);
			Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
	else
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
		
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	_bool		isMove = true;

	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != pNavigation)
	{
		isMove = pNavigation->isMove(vPosition, &vNormal);

		if (true == isMove)
		{
			vPosition.m128_f32[1] = pNavigation->Get_PosY(vPosition);
			Set_State(CTransform::STATE_POSITION, vPosition);
		}
		else
		{
			if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
				return;
			_vector		vPosition = Get_State(CTransform::STATE_POSITION);
			_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook) * (-1.f), vNormal)) * vNormal;
			vSlide += vLook;
			vPosition -= XMVector3Normalize(vSlide) * m_TransformDesc.fSpeedPerSec * fTimeDelta;
			vPosition.m128_f32[1] = pNavigation->Get_PosY(vPosition);
			Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
	else
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	_bool		isMove = true;

	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != pNavigation)
	{
		isMove = pNavigation->isMove(vPosition, &vNormal);

		if (true == isMove)
		{
			vPosition.m128_f32[1] = pNavigation->Get_PosY(vPosition);
			Set_State(CTransform::STATE_POSITION, vPosition);
		}

		else
		{
			if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
				return;
			_vector		vPosition = Get_State(CTransform::STATE_POSITION);
			_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vRight) * (-1.f), vNormal)) * vNormal;
			vSlide += vRight;
			vPosition -= XMVector3Normalize(vSlide) * m_TransformDesc.fSpeedPerSec * fTimeDelta;
			vPosition.m128_f32[1] = pNavigation->Get_PosY(vPosition);
			Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
	else
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	_bool		isMove = true;

	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != pNavigation)
	{
		isMove = pNavigation->isMove(vPosition, &vNormal);

		if (true == isMove)
		{
			vPosition.m128_f32[1] = pNavigation->Get_PosY(vPosition);
			Set_State(CTransform::STATE_POSITION, vPosition);
		}

		else
		{
			if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
				return;
			_vector		vPosition = Get_State(CTransform::STATE_POSITION);
			_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vRight) * (-1.f), vNormal)) * vNormal;
			vSlide += vRight;
			vPosition += XMVector3Normalize(vSlide) * m_TransformDesc.fSpeedPerSec * fTimeDelta;
			vPosition.m128_f32[1] = pNavigation->Get_PosY(vPosition);
			Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
	else
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Up(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	_bool		isMove = true;

	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != pNavigation)
	{
		isMove = pNavigation->isMove(vPosition, &vNormal);

		if (true == isMove)
			Set_State(CTransform::STATE_POSITION, vPosition);
		else
		{
			if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
				return;
			_vector		vPosition = Get_State(CTransform::STATE_POSITION);
			_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vUp) * (-1.f), vNormal)) * vNormal;
			vSlide += vUp;
			vPosition += XMVector3Normalize(vSlide) * m_TransformDesc.fSpeedPerSec * fTimeDelta;
			Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
	else
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}

}

void CTransform::Go_Down(_float fTimeDelta, CNavigation* pNavigation)
{
}

void CTransform::Go_Dir(_vector _vDir, _float fTimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(_vDir) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Set_Scale(_fvector vScaleInfo)
{
	Set_State(CTransform::STATE_RIGHT,
		XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * XMVectorGetX(vScaleInfo));
	Set_State(CTransform::STATE_UP,
		XMVector3Normalize(Get_State(CTransform::STATE_UP)) * XMVectorGetY(vScaleInfo));
	Set_State(CTransform::STATE_LOOK,
		XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * XMVectorGetZ(vScaleInfo));
}

_float3 CTransform::Get_Scale()
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK))));
}

void CTransform::Turn(_vector StartLook, _vector EndLook, _float _fRatio)
{
	_vector vLook, vRight;
	vLook = (XMVector3Normalize(StartLook) * _fRatio) + (XMVector3Normalize(EndLook)*(1.f - _fRatio));

	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	//vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Turn_Angle(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fTimeDelta);

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	// Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(60.0f));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		Scale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f), RotationMatrix) * Scale.x);
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), RotationMatrix) * Scale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), RotationMatrix) * Scale.z);
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(CTransform::STATE_POSITION);

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_ForLandObject(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(CTransform::STATE_POSITION);




	_float3		vScale = Get_Scale();

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;

	vLook = XMVector3Normalize(XMVector3Cross(vRight, Get_State(CTransform::STATE_UP))) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_LOOK, vLook);

}

void CTransform::Move(_fvector vTargetPos, _float fTimeDelta, _float fLimitDistance)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vDirection = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDirection));

	if (fDistance > fLimitDistance)
		vPosition += XMVector3Normalize(vDirection) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*			pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*			pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}
