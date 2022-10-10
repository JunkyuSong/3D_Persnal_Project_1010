#include "MotionTrail.h"

#include "HierarchyNode.h"
#include "AnimMeshContainer.h"
#include "Animation.h"
#include "Shader.h"

#include "Texture.h"

//¾Ö´Ô¸ðµ¨

CMotionTrail::CMotionTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CModel(pDevice,pContext)
	, m_pParent(nullptr)
{
}

//CMotionTrail::CMotionTrail(CMotionTrail & rhs)
//	: CModel(rhs)
//	, m_PivotMatrix(rhs.m_PivotMatrix)
//	, m_Meshes(rhs.m_Meshes)
//	, m_iNumAnimations(rhs.m_iNumAnimations)
//	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
//	, m_Animations(rhs.m_Animations)
//	, m_iNextAnimIndex(rhs.m_iNextAnimIndex)
//	, m_iPreAnimIndex(rhs.m_iPreAnimIndex)
//	, m_tModel(rhs.m_tModel)
//	, m_HierarchyNodes(rhs.m_HierarchyNodes)
//
//{
//	for (auto& pMeshContainer : m_Meshes)
//		Safe_AddRef(pMeshContainer);
//
//	for (auto& pHierarchyNode : m_HierarchyNodes)
//		Safe_AddRef(pHierarchyNode);
//
//	for (auto& pAnimation : m_Animations)
//		Safe_AddRef(pAnimation);
//	strcpy_s(m_tModel.Path, rhs.m_tModel.Path);
//	strcpy_s(m_tModel.Name, rhs.m_tModel.Name);
//}

HRESULT CMotionTrail::Initialize_Prototype(CAnimModel* pParent)
{
	m_pParent = pParent;
	Safe_AddRef(m_pParent);

	m_tModel = pParent->Get_ModelInfo();

	m_PivotMatrix = m_pParent->Get_PivotMatrix();

	m_Materials = pParent->Get_Materials();
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	vector<class CHierarchyNode*> _vecTemp = *m_pParent->Get_HierarchyNodeVector();

	for (auto& pNode : _vecTemp)
	{
		CHierarchyNode* _Temp = pNode->Clone();
		if (_Temp == nullptr)
		{
			MSG_BOX(TEXT("fail to clone motiontrail Node"));
			return E_FAIL;
		}
		m_HierarchyNodes.push_back(_Temp);
	}

	vector<class CAnimMeshContainer*> _vecSour = *m_pParent->Get_MeshesVector();

	//for (auto& pNode : _vecSour)
	for (int i = 0 ; i < _vecSour.size(); ++i)
	{
		CAnimMeshContainer* _Temp = static_cast<CAnimMeshContainer*>(_vecSour[i]->Clone());
		if (_Temp == nullptr)
		{
			MSG_BOX(TEXT("fail to clone motiontrail Node"));
			return E_FAIL;
		}
		_Temp->SetUp_HierarchyNodes(&m_HierarchyNodes, m_tModel.tMeshes[i]);
		m_Meshes.push_back(_Temp);
	}

	m_iNumMeshes = m_iNumMaterials =m_Meshes.size();
	return S_OK;
}

HRESULT CMotionTrail::Render(CShader * pShader, _uint _iPass, _uint _iMeshIndex)
{
	_float4x4		BoneMatrices[256];
	_float4x4		_World;
	XMStoreFloat4x4(&_World,XMMatrixTranspose(XMLoadFloat4x4(&m_ParentWorld)));
	
	m_Meshes[_iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	if (FAILED(pShader->Set_RawValue("g_WorldMatrix", &_World, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 256)))
		return E_FAIL;
	

	pShader->Begin(_iPass);

	m_Meshes[_iMeshIndex]->Render();
	return S_OK;
}

_uint CMotionTrail::Get_MaterialIndex(_uint _iMeshIndex) const
{
	return m_Meshes[_iMeshIndex]->Get_MaterialIndex();
}

void CMotionTrail::Set_CombinedMat(vector<class CHierarchyNode*>* _vecParentBones, _float4x4 _pParentWorld)
{
	m_ParentWorld = _pParentWorld;
	for (int i = 0; i < m_HierarchyNodes.size(); ++i)
	{
		m_HierarchyNodes[i]->Set_MatrixForTrail((*_vecParentBones)[i]->Get_CombinedTransformation());
	}
}

CMotionTrail * CMotionTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CAnimModel* pParent)
{
	CMotionTrail*			pInstance = new CMotionTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pParent)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMotionTrail::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	Safe_Release(m_pParent);
}