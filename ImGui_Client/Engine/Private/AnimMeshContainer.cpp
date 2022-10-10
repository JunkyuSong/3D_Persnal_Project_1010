#include "..\Public\AnimMeshContainer.h"

#include "HierarchyNode.h"
#include "AnimModel.h"

CAnimMeshContainer::CAnimMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMeshContainer(pDevice, pContext)
{
}

CAnimMeshContainer::CAnimMeshContainer(CAnimMeshContainer & rhs)
	: CMeshContainer(rhs)
	//, m_iNumBones(rhs.m_iNumBones)
	//, m_iMaterialIndex(rhs.m_iMaterialIndex)
{
	
}

HRESULT CAnimMeshContainer::Initialize_Prototype(const aiMesh * pAIMesh, CAnimModel * pModel, _fmatrix PivotMatrix, TANIMCONTAINER* _pOut)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	strcpy_s(_pOut->szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	_pOut->iIndex = m_iMaterialIndex;
	if (FAILED(Ready_Vertex(pAIMesh, pModel, _pOut)))
		return E_FAIL;

	if (FAILED(__super::Ready_IndexBuffer(pAIMesh, _pOut)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimMeshContainer::Initialize_Prototype(_fmatrix PivotMatrix, TANIMCONTAINER _tIn)
{
	strcpy_s(m_szName, _tIn.szName);
	m_iMaterialIndex = _tIn.iIndex;
	if (FAILED(Ready_Vertex(_tIn)))
		return E_FAIL;
	if (FAILED(__super::Ready_IndexBuffer(_tIn)))
		return E_FAIL;
	return S_OK;
}

HRESULT CAnimMeshContainer::Initialize(void * pArg)
{
	return S_OK;
}

/* 메시의 정점을 그리기위해 셰이더에 넘기기위한 뼈행렬의 배열을 구성한다. */
void CAnimMeshContainer::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	if (0 == m_iNumBones) // 본 갯수가..? <- 있나 이런 경우가? 연결도 해놨는데?
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());
		return;
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_fmatrix fds = XMMatrixTranspose(m_Bones[i]->Get_OffSetMatrix() * m_Bones[i]->Get_CombinedTransformation() * PivotMatrix);

		XMStoreFloat4x4(&pBoneMatrices[i], fds);
	}

}

HRESULT CAnimMeshContainer::SetUp_HierarchyNodes(CAnimModel * pModel, aiMesh * pAIMesh, TANIMCONTAINER* _pOut)
{
	m_iNumBones = pAIMesh->mNumBones;


	if (0 == m_iNumBones)
	{

		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_szName);

		if (nullptr == pNode)
			return S_OK;
		
		m_iNumBones = 1;
		
		_pOut->NumBones = m_iNumBones;
		_pOut->BoneIndecis = new int[m_iNumBones];
		_pOut->BoneIndecis[0] = pNode->Get_Index();
		m_Bones.push_back(pNode);
		return S_OK;
	}

	_pOut->NumBones = m_iNumBones;
	_pOut->BoneIndecis = new int[m_iNumBones];

	/* 현재 메시에 영향ㅇ르 ㅈ2ㅜ는 뼈들을 순회한다ㅏ. */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];

		CHierarchyNode*		pHierarchyNode = pModel->Get_HierarchyNode(pAIBone->mName.data);

		_float4x4			OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		pHierarchyNode->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_Bones.push_back(pHierarchyNode);

		_pOut->BoneIndecis[i] = pHierarchyNode->Get_Index();

		Safe_AddRef(pHierarchyNode);
	}

	
	return S_OK;
}

HRESULT CAnimMeshContainer::SetUp_HierarchyNodes(CAnimModel * pModel, TANIMCONTAINER _tIn)
{
	m_iNumBones = _tIn.NumBones;


	vector<CHierarchyNode*>		HierarchyNodes = *(pModel->Get_HierarchyNodeVector());


	/* 현재 메시에 영향ㅇ르 ㅈ2ㅜ는 뼈들을 순회한다ㅏ. */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		CHierarchyNode*		pHierarchyNode = HierarchyNodes[_tIn.BoneIndecis[i]];

		_float4x4			OffsetMatrix;

		m_Bones.push_back(pHierarchyNode);

		Safe_AddRef(pHierarchyNode);
	}


	return S_OK;
}

HRESULT CAnimMeshContainer::SetUp_HierarchyNodes(vector<CHierarchyNode*> * pHierarchyNodes, TANIMCONTAINER _tIn)
{
	m_iNumBones = _tIn.NumBones;


	vector<CHierarchyNode*>		HierarchyNodes = *(pHierarchyNodes);


	/* 현재 메시에 영향ㅇ르 ㅈ2ㅜ는 뼈들을 순회한다ㅏ. */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		CHierarchyNode*		pHierarchyNode = HierarchyNodes[_tIn.BoneIndecis[i]];

		_float4x4			OffsetMatrix;

		m_Bones.push_back(pHierarchyNode);

		Safe_AddRef(pHierarchyNode);
	}


	return S_OK;
}

CAnimMeshContainer * CAnimMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const aiMesh * pAIMesh, CAnimModel * pModel, _fmatrix PivotMatrix, TANIMCONTAINER* _pOut)
{
	CAnimMeshContainer*			pInstance = new CAnimMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pAIMesh, pModel, PivotMatrix, _pOut)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
CAnimMeshContainer * CAnimMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _fmatrix PivotMatrix, TANIMCONTAINER _tIn)
{
	CAnimMeshContainer*			pInstance = new CAnimMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(PivotMatrix, _tIn)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
CComponent * CAnimMeshContainer::Clone(void * pArg)
{
	CAnimMeshContainer*			pInstance = new CAnimMeshContainer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAnimMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimMeshContainer::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_Bones)
		Safe_Release(pHierarchyNode);

	m_Bones.clear();
}

HRESULT CAnimMeshContainer::Ready_Vertex(const aiMesh* pAIMesh, CAnimModel* pModel, TANIMCONTAINER* _pOut)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iStride = sizeof(VTXANIMMODEL);
	_pOut->NumVertices = m_iNumVertices;
	
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	_pOut->pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);
	ZeroMemory(_pOut->pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);


	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&(_pOut->pVertices[i]), &pVertices[i], sizeof(VTXANIMMODEL));
	}

	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];



		/* i번째 뼈가 어떤 정점들에게 영향ㅇ르 주는지 순회한다. */
		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			_uint		iVertexIndex = pAIBone->mWeights[j].mVertexId;

			if (0.0f == pVertices[iVertexIndex].vBlendWeight.x)
			{
				pVertices[iVertexIndex].vBlendIndex.x = _pOut->pVertices[iVertexIndex].vBlendIndex.x = i;
				pVertices[iVertexIndex].vBlendWeight.x = _pOut->pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.y)
			{
				pVertices[iVertexIndex].vBlendIndex.y = _pOut->pVertices[iVertexIndex].vBlendIndex.y = i;
				pVertices[iVertexIndex].vBlendWeight.y = _pOut->pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.z)
			{
				pVertices[iVertexIndex].vBlendIndex.z = _pOut->pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = _pOut->pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.w)
			{
				pVertices[iVertexIndex].vBlendIndex.w = _pOut->pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = _pOut->pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	//if (0 == m_iNumBones) // 뼈 없는 메쉬일 경우 붙일 뼈의 상태를 먹여주기 위해서!
	//{

	//	CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_szName);

	//	if (nullptr == pNode)
	//		return S_OK;

	//	m_iNumBones = 1;

	//	m_Bones.push_back(pNode);

	//}

	return S_OK;
}

HRESULT CAnimMeshContainer::Ready_Vertex(TANIMCONTAINER _tin)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = _tin.NumVertices;
	m_iStride = sizeof(VTXANIMMODEL);
	
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i], &(_tin.pVertices[i]), sizeof(VTXANIMMODEL));
	}

	m_iNumBones = _tin.NumBones;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	//if (0 == m_iNumBones) // 뼈 없는 메쉬일 경우 붙일 뼈의 상태를 먹여주기 위해서!
	//{

	//	CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_szName);

	//	if (nullptr == pNode)
	//		return S_OK;

	//	m_iNumBones = 1;

	//	m_Bones.push_back(pNode);

	//}

	return S_OK;

}
