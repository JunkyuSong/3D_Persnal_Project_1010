#pragma once
#include "MeshContainer.h"

BEGIN(Engine)

class CAnimMeshContainer :
	public CMeshContainer
{
public:
	CAnimMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimMeshContainer(CAnimMeshContainer& rhs);
	virtual ~CAnimMeshContainer() = default;
public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, class CAnimModel* pModel, _fmatrix PivotMatrix, TANIMCONTAINER* _pOut);
	virtual HRESULT Initialize_Prototype(_fmatrix PivotMatrix, TANIMCONTAINER _tIn);
	virtual HRESULT Initialize(void* pArg);
	
	
	
public:
	void SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix);
	HRESULT SetUp_HierarchyNodes(CAnimModel * pModel, aiMesh* pAIMesh, TANIMCONTAINER* _pOut);
	HRESULT SetUp_HierarchyNodes(CAnimModel * pModel, TANIMCONTAINER _tIn);
	HRESULT SetUp_HierarchyNodes(vector<class CHierarchyNode*> * pHierarchyNodes, TANIMCONTAINER _tIn);

public:
	static CAnimMeshContainer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const aiMesh * pAIMesh, CAnimModel * pModel, _fmatrix PivotMatrix, TANIMCONTAINER* _pOut);
	static CAnimMeshContainer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _fmatrix PivotMatrix, TANIMCONTAINER _tIn);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	HRESULT Ready_Vertex(const aiMesh* pAIMesh, CAnimModel* pModel, TANIMCONTAINER* _pOut);
	HRESULT Ready_Vertex(TANIMCONTAINER _tin);

private:
	/* 이 메시에 영향ㅇ르 주는 뼈의 갯수. */
	_uint							m_iNumBones = 0;

	/* 이 메시에 영향을 주는 뼈들을 모아놓느다. */
	/* why ? : 메시 별로 렌더링할 때 이 메시에 영향ㅇ르 주는 뼏르의 행렬을 모아서 셰이더로 던질려고. */
	vector<class CHierarchyNode*>	m_Bones;
	

	//_uint m_iMaterialIndex; //현재 메쉬의 인덱스
};

END