#include "..\Public\MeshContainer.h"
#include "Transform.h"
#include "Picking.h"
#include "PipeLine.h"

CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
	ZeroMemory(m_szName, sizeof(char)*MAX_PATH);
	ZeroMemory(&m_in, sizeof(TCONTAINER));

}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_in(rhs.m_in)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CMeshContainer::Initialize_Prototype(const aiMesh * pAIMesh, TCONTAINER* _pOut)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	//해당 메쉬의 인덱스
	_pOut->iIndex = m_iMaterialIndex;
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	_pOut->NumVertices = m_iNumVertices;

	_pOut->pVertices = new VTXMODEL[m_iNumVertices];

	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&(_pOut->pVertices[i]), &(pVertices[i]), sizeof(VTXMODEL));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	Ready_IndexBuffer(pAIMesh, _pOut);

#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Initialize_Prototype(TCONTAINER tIn)
{
	m_in = tIn;
	m_iMaterialIndex = tIn.iIndex;
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = tIn.NumVertices;

	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&(pVertices[i]), &(tIn.pVertices[i]), sizeof(VTXMODEL));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	Ready_IndexBuffer(tIn);

#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Ready_IndexBuffer(const aiMesh* pAIMesh, TCONTAINER* _pOut)
{
	m_iNumPrimitives = pAIMesh->mNumFaces;
	_pOut->NumFaces = m_iNumPrimitives;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	_pOut->pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);
	ZeroMemory(_pOut->pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
		memcpy(&(_pOut->pIndices[i]), &(pIndices[i]), sizeof(FACEINDICES32));
	}

	m_in = *_pOut;
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}
HRESULT CMeshContainer::Ready_IndexBuffer(const aiMesh* pAIMesh, TANIMCONTAINER* _pOut)
{
	m_iNumPrimitives = pAIMesh->mNumFaces;
	_pOut->NumFaces = m_iNumPrimitives;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	_pOut->pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);
	ZeroMemory(_pOut->pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		pIndices[i]._0 = _pOut->pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = _pOut->pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = _pOut->pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
		//memcpy(&(_pOut->pIndices[i]), &(pIndices[i]), sizeof(FACEINDICES32));
	}


	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}
HRESULT CMeshContainer::Ready_IndexBuffer(TANIMCONTAINER _tIn)
{
	
	m_iNumPrimitives = _tIn.NumFaces;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{

		memcpy(&(pIndices[i]), &(_tIn.pIndices[i]), sizeof(FACEINDICES32));
	}


	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}
HRESULT CMeshContainer::Ready_IndexBuffer(TCONTAINER _tIn)
{
	m_iNumPrimitives = _tIn.NumFaces;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		//pIndices[i]._0 = _tIn.pIndices[i]._0 ;
		//pIndices[i]._1 = _tIn.pIndices[i]._1 ;
		//pIndices[i]._2 = _tIn.pIndices[i]._2 ;
		memcpy(&(pIndices[i]), &(_tIn.pIndices[i]), sizeof(FACEINDICES32));
	}


	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}
_bool CMeshContainer::Picking(CTransform * pTransform, _vector & pOut)
{
	CPicking*		pPicking = CPicking::Get_Instance();
	_bool _bPicking = false;
	Safe_AddRef(pPicking);

	_vector			vRayDir, vRayPos;

	pPicking->Compute_LocalRayInfo(vRayDir, vRayPos, pTransform);
	vRayDir = XMVector3Normalize(vRayDir);

	_vector		_pVerticesPos_0;
	_vector		_pVerticesPos_1;
	_vector		_pVerticesPos_2;
	_float		fDist;
	_matrix		WorldMatrix = pTransform->Get_WorldMatrix();


	_uint		iIndices[3] = { NULL };
	_float		_fFinalDistToCam = 0.f;
	_bool		_bOne = false;
	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		iIndices[0] = m_in.pIndices[i]._0;
		iIndices[1] = m_in.pIndices[i]._1;
		iIndices[2] = m_in.pIndices[i]._2;
		
		_pVerticesPos_0 = XMLoadFloat3(&(m_in.pVertices[iIndices[0]].vPosition));
		_pVerticesPos_1 = XMLoadFloat3(&(m_in.pVertices[iIndices[1]].vPosition));
		_pVerticesPos_2 = XMLoadFloat3(&(m_in.pVertices[iIndices[2]].vPosition));

		if (true == TriangleTests::Intersects(vRayPos, vRayDir, _pVerticesPos_0, _pVerticesPos_1, _pVerticesPos_2, fDist))
		{
			_vector	vPickPos = vRayPos + (vRayDir * fDist);

			if (_bOne == false)
			{
				pOut = XMVector3TransformCoord(vPickPos, WorldMatrix);
				_fFinalDistToCam = XMVectorGetX(XMVector3Length(pOut - XMLoadFloat4(&(CPipeLine::Get_Instance()->Get_CamPosition()))));
				
				_bOne = true;
			}

			vPickPos = XMVector3TransformCoord(vPickPos, WorldMatrix);
			_float _fCurDistToCam = XMVectorGetX(XMVector3Length(vPickPos - XMLoadFloat4(&(CPipeLine::Get_Instance()->Get_CamPosition()))));

			if (_fFinalDistToCam > _fCurDistToCam)
			{
				pOut = vPickPos;
				_fFinalDistToCam = _fCurDistToCam;
			}
			
			_bPicking = true;
		}
	}

	Safe_Release(pPicking);

	return _bPicking;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const aiMesh * pAIMesh, TCONTAINER*	_pOut)
{
	CMeshContainer*			pInstance = new CMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pAIMesh, _pOut)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TCONTAINER _tIn)
{
	CMeshContainer*			pInstance = new CMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(_tIn)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*			pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

}
