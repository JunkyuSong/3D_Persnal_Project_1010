#pragma once
#include "Base.h"
#include "Engine_Binary.h"

BEGIN(Engine)

class ENGINE_DLL CHierarchyNode :
	public CBase
{
private:
	CHierarchyNode();
	CHierarchyNode(const CHierarchyNode& rhs);
	virtual ~CHierarchyNode() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}
	_uint Get_Depth() const {
		return m_iDepth;
	}

	void Set_Depth(_uint _iDepth) {
		m_iDepth = _iDepth;
	}

	_matrix Get_OffSetMatrix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombinedTransformation() {
		return XMLoadFloat4x4(&m_CombinedTransformation);
	}


	KEYFRAME Get_DefaultKeyFrame() {
		return m_tDefaultKeyFrame;
	}

	_float4x4	Get_Origin() { return m_OriginTransformation; }
	_float4x4	Get_Trans() { return m_Transformation; }

public:
	void Set_Transformation(_fmatrix Transformation) {
		XMStoreFloat4x4(&m_Transformation, Transformation);
	}
	void Set_MatrixForTrail(_fmatrix _combined) { XMStoreFloat4x4 (&m_CombinedTransformation, _combined); }

public:
	HRESULT Initialize_Proto(aiNode* pAINode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Initialize_Proto(THIERARCKY _tIn, vector<CHierarchyNode*>* _HierarchyNodes);
	HRESULT Initialize(void* pArg = nullptr);
	void Set_CombinedTransformation(_float4* _vAnim, _float4x4 _pivot);
	void Set_CombinedTransformation(_float4* _vAnim, _float4x4 _pivot, char* RootY);
	void Set_CombinedTransformation(_float4* _vAnim, _bool);
	void Set_CombinedTransformation(_float4* _vAnim, _bool, char* RootY);
	void Set_OffsetMatrix(_fmatrix OffsetMatrix);

	void	Set_Index(_uint _iIndex)	{ m_iIndex = _iIndex; }
	_uint	Get_Index()					{ return m_iIndex; }

	void	Set_ParentIndex(_uint _iIndex) { m_iParentIndex = _iIndex; }
	_int	Get_ParentIndex() { return m_iParentIndex; }

	CHierarchyNode* Get_Parent() {
		return m_pParent;
	}
	void Set_Parent(CHierarchyNode* _pParent) {
		Safe_Release(m_pParent);
		m_pParent = _pParent;
		Safe_AddRef(m_pParent);
	}

private:
	char				m_szName[MAX_PATH] = "";
	_float4x4			m_OffsetMatrix;

	_float4x4			m_Transformation;
	_float4x4			m_CombinedTransformation;
	CHierarchyNode*		m_pParent = nullptr;
	_int				m_iParentIndex;
	_uint				m_iDepth = 0;
	_uint				m_iIndex;

	_float4x4			m_OriginTransformation;

	KEYFRAME			m_tDefaultKeyFrame;

public:
	static CHierarchyNode* Create(aiNode* pAINode, class CHierarchyNode* pParent, _uint iDepth);
	static CHierarchyNode* Create(THIERARCKY _tIn, vector<CHierarchyNode*>* _HierarchyNodes);
	CHierarchyNode* Clone(void* pArg = nullptr);
	virtual void Free();
};

END