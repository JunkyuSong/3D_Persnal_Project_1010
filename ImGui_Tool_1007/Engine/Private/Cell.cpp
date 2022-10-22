#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"
#include "Shader.h"
#include "PipeLine.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CCell::Mapping_Point()
{

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pVIBuffer->Map(&mappedResource);
	VTXCOL _Temp[3];
	memcpy(_Temp, mappedResource.pData, sizeof(VTXCOL) * 3);
	_Temp[0].vPosition = m_vPoints[0];
	_Temp[1].vPosition = m_vPoints[1];
	_Temp[2].vPosition = m_vPoints[2];
	memcpy(mappedResource.pData, _Temp, sizeof(VTXCOL) * 3);
	m_pVIBuffer->UnMap();

}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	_vector		vLine[LINE_END];
	
	//여기서 체크하고
	vLine[LINE_AB] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));	
	m_vNormal[LINE_AB] = _float3(XMVectorGetZ(vLine[LINE_AB]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_AB]));

	_vector vAtoC = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_A]));

	_float _fCos = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vAtoC), XMLoadFloat3(&m_vNormal[LINE_AB])));

	if (_fCos > 0)
	{
		_float3 _vTemp = m_vPoints[POINT_B];
		m_vPoints[POINT_B] = m_vPoints[POINT_C];
		m_vPoints[POINT_C] = _vTemp;
		vLine[LINE_AB] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
		m_vNormal[LINE_AB] = _float3(XMVectorGetZ(vLine[LINE_AB]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_AB]));
	}
	
	vLine[LINE_BC] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	vLine[LINE_CA] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
	
	
	m_vNormal[LINE_BC] = _float3(XMVectorGetZ(vLine[LINE_BC]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_BC]));
	m_vNormal[LINE_CA] = _float3(XMVectorGetZ(vLine[LINE_CA]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_CA]));

	m_iIndex = iIndex;

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare(const _float3 & vSourPoint, const _float3 & vDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;		

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPosition, _int * pNeighborIndex, _vector* _vSlide)
{

	_uint _iOut = 0;
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vDir = XMVectorSetW(vPosition - XMLoadFloat3(&m_vPoints[i]), 0.f);
		vDir = XMVector3Normalize(vDir);
		_vector _vTemp = XMVector3Dot(vDir, XMLoadFloat3(&m_vNormal[i]));
		_float  _fTemp = XMVectorGetX(_vTemp);

		if (0.f <= _fTemp)
		{
			if (XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormal[i]))) != 0)
			{
				if (_vSlide != nullptr)
					*_vSlide = XMLoadFloat3(&m_vNormal[i]) * (-1.f);
				if (_iOut == 0)
					*pNeighborIndex = m_iNeighborIndex[i];
			}
			
			++_iOut;
		}				
	}

	if (_iOut > 0)
	{
		if (_iOut > 1/* && *pNeighborIndex == -1*/)
		{
			*pNeighborIndex = -1;
			if (_vSlide != nullptr)
				*_vSlide = XMVectorSet(0.f,0.f,0.f,0.f);
		}
		return false;
	}		
	return true;
}

#ifdef _DEBUG
HRESULT CCell::Render_Cell(_float fHeight, _float4 vColor)
{
	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4			WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix.m[1][3] = fHeight;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	m_pShader->Begin(0);

	m_pVIBuffer->Render();

	return S_OK;
}
void CCell::Set_Normal()
{
	_vector		vLine[LINE_END];

	vLine[LINE_AB] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	vLine[LINE_BC] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	vLine[LINE_CA] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));

	m_vNormal[LINE_AB] = _float3(XMVectorGetZ(vLine[LINE_AB]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_AB]));
	m_vNormal[LINE_BC] = _float3(XMVectorGetZ(vLine[LINE_BC]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_BC]));
	m_vNormal[LINE_CA] = _float3(XMVectorGetZ(vLine[LINE_CA]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_CA]));
}
#endif // _DEBUG

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*			pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CCell"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
#endif // _DEBUG

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
