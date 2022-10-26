#include "..\Public\HierarchyNode.h"



CHierarchyNode::CHierarchyNode()
{
}

CHierarchyNode::CHierarchyNode(const CHierarchyNode & rhs)
	: m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_Transformation(rhs.m_Transformation)
	, m_CombinedTransformation(rhs.m_CombinedTransformation)
	, m_pParent(nullptr)
	, m_iParentIndex(rhs.m_iParentIndex)
	, m_iDepth(rhs.m_iDepth)
	, m_iIndex(rhs.m_iIndex)
	, m_OriginTransformation(rhs.m_OriginTransformation)
	, m_tDefaultKeyFrame(rhs.m_tDefaultKeyFrame)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CHierarchyNode::Initialize_Proto(aiNode * pAINode, CHierarchyNode * pParent, _uint iDepth)
{
	strcpy_s(m_szName, pAINode->mName.data);

	//뼈 로컬을 정점의 로컬로 바꿔주는 행렬
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	

	//m_Transformation : 뼈의 로컬상태
	memcpy(&m_Transformation, &pAINode->mTransformation, sizeof(_float4x4));
	m_OriginTransformation = m_Transformation;
	_vector vScale;
	_vector vRotation;
	_vector vPosition;
	
	XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&m_OriginTransformation));

	XMStoreFloat3(&m_tDefaultKeyFrame.vScale, vScale);
	XMStoreFloat4(&m_tDefaultKeyFrame.vRotation, vRotation);
	XMStoreFloat3(&m_tDefaultKeyFrame.vPosition, vPosition);

	//애니메이션 진행마다 업데이트해줄 부모의 뼈 상태와 현재 뼈 상태를 연산한 행렬
	XMStoreFloat4x4(&m_CombinedTransformation, XMMatrixIdentity());

	m_iDepth = iDepth;
	m_pParent = pParent;
	Safe_AddRef(m_pParent);

	return S_OK;
}

HRESULT CHierarchyNode::Initialize_Proto(THIERARCKY _tIn, vector<CHierarchyNode*>* _HierarchyNodes)
{
	strcpy_s(m_szName, _tIn.szName);
	m_iParentIndex = _tIn.ParentIndex;
	if (m_iParentIndex >= 0)
	{
		m_pParent = (*_HierarchyNodes)[m_iParentIndex];
		Safe_AddRef(m_pParent);
	}
		
	else
		m_iParentIndex = 0;
	m_OffsetMatrix = _tIn.Offset;
	m_Transformation = m_OriginTransformation = _tIn.Transformation;
	
	_vector vScale;
	_vector vRotation;
	_vector vPosition;
	XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&m_OriginTransformation));

	XMStoreFloat3(&m_tDefaultKeyFrame.vScale, vScale);
	XMStoreFloat4(&m_tDefaultKeyFrame.vRotation, vRotation);
	XMStoreFloat3(&m_tDefaultKeyFrame.vPosition, vPosition);

	//애니메이션 진행마다 업데이트해줄 부모의 뼈 상태와 현재 뼈 상태를 연산한 행렬
	XMStoreFloat4x4(&m_CombinedTransformation, XMMatrixIdentity());

	return S_OK;
}

HRESULT CHierarchyNode::Initialize(void * pArg)
{
	return S_OK;
}

void CHierarchyNode::Set_CombinedTransformation(_float4* _vAnim, _float4x4 _pivot)
{
	//if (!strcmp(m_szName, "root"))
	//{
	//	m_Transformation._41 = 0.f;
	//	m_Transformation._42 = 0.f;
	//	m_Transformation._43 = 0.f;

	//	XMStoreFloat4x4(_AnimMatrix, XMLoadFloat4x4(&m_Transformation));
	//}

	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));
	else
		m_CombinedTransformation = m_Transformation;
	

	if (!strcmp(m_szName, "root"))
	{
		_float4x4 _Temp;
	
		XMStoreFloat4x4(&_Temp, XMLoadFloat4x4(&m_CombinedTransformation)*(XMLoadFloat4x4(&_pivot)));
		_vAnim->x = _Temp._41;
		_vAnim->y = _Temp._42;
		_vAnim->z = _Temp._43;
		_vAnim->w = _Temp._44;
		m_CombinedTransformation._41 = 0.f;
		m_CombinedTransformation._42 = 0.f;
		m_CombinedTransformation._43 = 0.f;	
	}
}

void CHierarchyNode::Set_CombinedTransformation(_float4 * _vAnim, _float4x4 _pivot, char * RootY)
{
	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));
	else
		m_CombinedTransformation = m_Transformation;


	if (!strcmp(m_szName, "root"))
	{
		_float4x4 _Temp;

		XMStoreFloat4x4(&_Temp, XMLoadFloat4x4(&m_CombinedTransformation)*(XMLoadFloat4x4(&_pivot)));
		_vAnim->x = _Temp._41;
		_vAnim->y = _Temp._42;
		_vAnim->z = _Temp._43;
		_vAnim->w = _Temp._44;
		m_CombinedTransformation._41 = 0.f;
		m_CombinedTransformation._42 = 0.f;
		m_CombinedTransformation._43 = 0.f;
	}
	else if (!strcmp(m_szName, RootY))
	{
		/*_float4x4 _Temp;

		XMStoreFloat4x4(&_Temp, XMLoadFloat4x4(&m_CombinedTransformation)*(XMLoadFloat4x4(&_pivot)));

		_vAnim->y += _Temp._42;

		m_CombinedTransformation._42 = 0.f;*/

	}
}

void CHierarchyNode::Set_CombinedTransformation(_float4* _vAnim, _bool)
{
	//if (!strcmp(m_szName, "root"))
	//{
	//	m_Transformation._41 = 0.f;
	//	m_Transformation._42 = 0.f;
	//	m_Transformation._43 = 0.f;

	//	XMStoreFloat4x4(_AnimMatrix, XMLoadFloat4x4(&m_Transformation));
	//}

	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));
	else
		m_CombinedTransformation = m_Transformation;

	if (!strcmp(m_szName, "root"))
	{
		m_CombinedTransformation._41 = 0.f;
		m_CombinedTransformation._42 = 0.f;
		m_CombinedTransformation._43 = 0.f;
	}
}

void CHierarchyNode::Set_CombinedTransformation(_float4 * _vAnim, _bool, char * RootY)
{
	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));
	else
		m_CombinedTransformation = m_Transformation;

	if (!strcmp(m_szName, "root"))
	{
		m_CombinedTransformation._41 = 0.f;
		m_CombinedTransformation._42 = 0.f;
		m_CombinedTransformation._43 = 0.f;
	}
	else if (!strcmp(m_szName, RootY))
	{
		//m_CombinedTransformation._42 = 0.f;
	}
}

void CHierarchyNode::Set_OffsetMatrix(_fmatrix OffsetMatrix)
{
	XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
}

CHierarchyNode * CHierarchyNode::Create(aiNode * pAINode, CHierarchyNode * pParent, _uint iDepth)
{
	CHierarchyNode*			pInstance = new CHierarchyNode();

	if (FAILED(pInstance->Initialize_Proto(pAINode, pParent, iDepth)))
	{
		MSG_BOX(TEXT("Failed To Created : CHierarchyNode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHierarchyNode * CHierarchyNode::Create(THIERARCKY _tIn, vector<CHierarchyNode*>* _HierarchyNodes)
{
	CHierarchyNode*			pInstance = new CHierarchyNode();

	if (FAILED(pInstance->Initialize_Proto(_tIn, _HierarchyNodes)))
	{
		MSG_BOX(TEXT("Failed To Created : CHierarchyNode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHierarchyNode * CHierarchyNode::Clone(void * pArg)
{
	CHierarchyNode*			pInstance = new CHierarchyNode(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);
}