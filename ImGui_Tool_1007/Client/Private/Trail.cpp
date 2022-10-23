#include "stdafx.h"
#include "Trail.h"
#include "GameInstance.h"


CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
	, m_pShaderCom(nullptr)
{
}

CTrail::CTrail(const CTrail & rhs)
	: CVIBuffer(rhs)
	, m_fMaxTIme(0.01f)
	, m_fCurTime(0.f)
	, m_bTrailOn(false)
	, m_iVtxCount(0)
	, m_pShaderCom(nullptr)
{
}

HRESULT CTrail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CTrail::Initialize(void * pArg)
{
	//ZeroMemory(m_RealData, sizeof(REALDATA)*22);

#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 22;
	m_iStride = sizeof(VTXTRAIL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC/*D3D11_USAGE_DEFAULT*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE/*0*/;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXTRAIL*		pVertices = new VTXTRAIL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTRAIL) * m_iNumVertices);
	/*for (int i = 0; i < 22; ++i)
	{
	pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);
	pVertices[i].iIndex = unsigned int(i);
	}*/
	for (int i = 0; i < 22; i += 2)
	{
		pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);
		pVertices[i].vTexture = _float2(0.f, 0.f);
		//pVertices[i].iIndex = unsigned int(i);

		pVertices[i + 1].vPosition = _float3(0.f, 0.f, 0.f);
		pVertices[i + 1].vTexture = _float2(0.f, 0.f);
		//pVertices[i + 1].iIndex = unsigned int(i + 1);
	}
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = 20;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES16*		pIndices = new FACEINDICES16[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; i += 2)
	{
		pIndices[i]._0 = i + 3;
		pIndices[i]._1 = i + 1;
		pIndices[i]._2 = i;

		pIndices[i + 1]._0 = i + 2;
		pIndices[i + 1]._1 = i + 3;
		pIndices[i + 1]._2 = i;
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	if (pArg == nullptr)
	{
		MSG_BOX(TEXT("Plz High and Low"));
		return E_FAIL;
	}

	TRAILINFO _tInfo = *static_cast<TRAILINFO*>(pArg);
	m_HighAndLow = _tInfo._HighAndLow;
	m_Color = _tInfo._Color;

	AUTOINSTANCE(CGameInstance, pGameInstance);
	m_pShaderCom = static_cast<CShader*>(pGameInstance->Clone_Component(LEVEL_GAMEPLAY,TEXT("Prototype_Component_Shader_Trail")));
	
	if (m_pShaderCom == nullptr)
	{
		MSG_BOX(TEXT("Clone Trail Shader"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrail::Render()
{
	if (m_bTrailOn == false)
	{
		return S_OK;
	}
	AUTOINSTANCE(CGameInstance, pGameInstance);
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color", &m_Color, sizeof(_float4))))
		return E_FAIL;
	/*if (FAILED(m_pShaderCom->Set_RawValue("g_vPosition", m_RealData, sizeof(REALDATA) * 22)))
		return E_FAIL;*/
	/*if (FAILED(m_pShaderCom->Set_RawValue("g_vTexture", m_vTexture, sizeof(_float2) * 22)))
		return E_FAIL;*/


	m_pShaderCom->Begin(0);
	
	if (FAILED(__super::Render()))
	{
		MSG_BOX(TEXT("Trail Render"));
	}


	return S_OK;
}

void CTrail::Tick(const _float& _fTimeDelta, _matrix _matWeapon)
{
	if (m_bTrailOn == false)
	{
		return;
	}

	m_fCurTime += _fTimeDelta;

	//if (m_fCurTime > m_fMaxTIme)
	{
		m_fCurTime -= 0.01f;
		if (m_iVtxCount >= m_iNumVertices)
		{
			m_iVtxCount = 20;
			for (_uint i = 0; i < m_iNumVertices - 2; ++i)
			{
				m_RealData[i].vPosition = m_RealData[i + 2].vPosition;
			}
			//if ( XMLoadFloat3(&m_vPosition[10]) == XMLoadFloat3(&m_vPosition[0]))
			if (m_RealData[21].vPosition.x == m_RealData[1].vPosition.x &&
				m_RealData[21].vPosition.y == m_RealData[1].vPosition.y &&
				m_RealData[21].vPosition.z == m_RealData[1].vPosition.z)
			{
				m_iVtxCount = 0;
				m_bTrailOn = false;
				return;
			}
		}
		_fvector High = XMVector3TransformCoord(XMLoadFloat3(&(m_HighAndLow.vHigh)), _matWeapon);
		_fvector Low = XMVector3TransformCoord(XMLoadFloat3(&(m_HighAndLow.vLow)), _matWeapon);
		//_float3 High =  - _float3(0.f, 0.01f, 0.f);
		//_float3 Low = m_HighAndLow.vLow - _float3(0.f, 0.01f, 0.f);
		XMStoreFloat3(&(m_RealData[m_iVtxCount].vPosition), High);
		XMStoreFloat3(&(m_RealData[m_iVtxCount + 1].vPosition), Low);
		//D3DXVec3TransformCoord(&pVertices[m_iVtxCount].vPosition, &Low, &_matWeapon);
		//D3DXVec3TransformCoord(&pVertices[m_iVtxCount + 1].vPosition, &High, &_matWeapon);

		for (_uint i = 0; i < m_iVtxCount; i += 2)
		{
			_float _iVtxCount = 0.f;
			if (m_iVtxCount < 1)
			{
				_iVtxCount = 1.f;
			}
			else
			{
				_iVtxCount = (_float)m_iVtxCount;
			}
			m_RealData[i].vTexture = { ((_float)i) / ((_float)_iVtxCount),0.f };
			m_RealData[i + 1].vTexture = { ((_float)i) / ((_float)_iVtxCount),1.f };
		}
		m_iVtxCount += 2;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, m_RealData, sizeof(VTXTRAIL) * 22);
	m_pContext->Unmap(m_pVB, 0);
}

void CTrail::TrailOn(_matrix _matWeapon)
{
	
	//_float3 High = { 0.5f, 0.f, 0.5f };
	//_float3 Low = { -0.5f, 0.f, -0.5f };
	for (_uint i = 0; i < 22; i += 2)
	{
		/*D3DXVec3TransformCoord(&pVertices[i].vPosition, &Low, &_matWeapon);
		D3DXVec3TransformCoord(&pVertices[i + 1].vPosition, &High, &_matWeapon);*/
		_fvector High = XMVector3TransformCoord(XMLoadFloat3(&(m_HighAndLow.vHigh)), _matWeapon);
		_fvector Low = XMVector3TransformCoord(XMLoadFloat3(&(m_HighAndLow.vLow)), _matWeapon);
		/*XMStoreFloat3(&(m_vPosition[i]), High);
		XMStoreFloat3(&(m_vPosition[i + 1]), Low);*/
		XMStoreFloat3(&(m_RealData[i].vPosition), High);
		XMStoreFloat3(&(m_RealData[i + 1].vPosition), Low);
	}
	
	m_bTrailOn = true;
}


void CTrail::Spline()
{
	/*m_iEndIndex = m_iCatmullRomCount * 2 + m_iVtxCount;
	m_iCatmullRomIndex[2] = m_iEndIndex - 2;
	m_iCatmullRomIndex[3] = m_iEndIndex;

	m_RealData[m_iEndIndex - 2].vPosition = m_RealData[m_iVtxCount - 2].vPosition;
	m_RealData[m_iEndIndex - 1].vPosition = m_RealData[m_iVtxCount - 1].vPosition;

	m_RealData[m_iEndIndex].vPosition = 
		XMVector3TransformCoord(XMLoadFloat3(&m_HighAndLow.vLow), _matWeapon);
	m_RealData[m_iEndIndex-1].vPosition =
		XMVector3TransformCoord(XMLoadFloat3(&m_HighAndLow.vHigh), _matWeapon);

	for (_uint i = 0; i < m_iCatmullRomCount; ++i)
	{
		_uint index = i * 2 + m_iVtxCount - 2;
		_float fWeight = _float(i + 1) / (m_iCatmullRomCount + 1);

		XMVectorCatmullRom()
	}
*/
}

CTrail * CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail*			pInstance = new CTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTrail::Clone(void * pArg)
{
	CTrail*			pInstance = new CTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	Safe_Release(m_pShaderCom);
	__super::Free();
}
