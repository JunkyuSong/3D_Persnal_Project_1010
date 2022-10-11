#include "stdafx.h"
#include "..\Public\Client_Camere.h"

CClient_Camere::CAMERATYPE CClient_Camere::m_eCameraType = CAMERA_PLAYER;

CClient_Camere::CClient_Camere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CClient_Camere::CClient_Camere(const CClient_Camere & rhs, CTransform::TRANSFORMDESC * pArg)
	: CCamera(rhs, pArg)
{
}


void CClient_Camere::Free()
{
	__super::Free();
}
