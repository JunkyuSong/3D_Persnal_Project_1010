#pragma once
#include "Model.h"


BEGIN(Engine)
class ENGINE_DLL CAnimModel final :
public CModel
{
public:
	CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimModel(CAnimModel& rhs);
	virtual ~CAnimModel() = default;

public:
	HRESULT Initialize_Prototype(const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix);
	HRESULT Initialize(void * pArg);

public:
	HRESULT Set_AnimationIndex(_uint _AnimationIndex);
	_bool	Play_Animation(_float _fTimeDelta, _float4* _vAnim, _float* pOut, _bool& _bAgain);
	_bool	Play_Animation(_float _fTimeDelta, _float4* _vAnim, _float* pOut, _bool& _bAgain, char* RootY);
	void	DirectAnim(_uint _iAnimIndex);
	_bool	Repeat_Animation(_float _fTimeDelta, _float4* _vAnim, _float* pOut);
	HRESULT Render(class CShader* pShader, _uint _iPass, _uint _iMeshIndex);
	_uint   Get_MaterialIndex(_uint _iMeshIndex) const;

	const _float4x4&		Get_PivotMatrix() { return m_PivotMatrix; }

private:


	HRESULT Ready_HierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Ready_Animations();
	HRESULT Ready_Animations(int _Num);



public:
	static  CAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

public:
	class CHierarchyNode* Get_HierarchyNode(char* pNodeName);
	vector<class CHierarchyNode*>* Get_HierarchyNodeVector() { return &m_HierarchyNodes; }
	vector<class CAnimMeshContainer*>* Get_MeshesVector() { return &m_Meshes; }
	vector<MATERIALDESC>			Get_Materials() { return m_Materials; }

	void							Repeat() { m_bRepeat = true; }

private:
	TANIMMODEL							m_tModel;

	_float4x4							m_PivotMatrix;
	vector<class CHierarchyNode*>		m_HierarchyNodes;


	_uint								m_iPreAnimIndex = 0;
	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNextAnimIndex;

	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;

	typedef vector<class CAnimMeshContainer*>	MESHES;
	MESHES								m_Meshes;

	set<_uint>							m_TotalChannel;

	_bool								m_bRepeat = false;

private:
	HRESULT	Load_Dat(const char * pModelFilePath, const char * pModelFileName);
	HRESULT	Load_Default(const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix);



	//Åø¿ë
public:
	vector<CAnimation*>* Get_AllAnimation() {
		return &m_Animations;
	}
	TANIMMODEL Get_ForSaveAnim();
	TANIMMODEL Get_ModelInfo() { return m_tModel; }
};

END