#include "..\Public\AnimModel.h"

#include "HierarchyNode.h"
#include "AnimMeshContainer.h"
#include "Animation.h"
#include "Shader.h"
#include "Channel.h"

CAnimModel::CAnimModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CModel(pDevice, pContext)
{
}

CAnimModel::CAnimModel(CAnimModel & rhs)
	: CModel(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_Animations(rhs.m_Animations)
	, m_iNextAnimIndex(rhs.m_iNextAnimIndex)
	, m_iPreAnimIndex(rhs.m_iPreAnimIndex)
	, m_tModel(rhs.m_tModel)
	, m_HierarchyNodes(rhs.m_HierarchyNodes)

{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);

	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_AddRef(pHierarchyNode);

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);
	strcpy_s(m_tModel.Path, rhs.m_tModel.Path);
	strcpy_s(m_tModel.Name, rhs.m_tModel.Name);
}

HRESULT CAnimModel::Initialize_Prototype(const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	m_isCloned = false;

	char szExt[MAX_PATH] = "";

	_splitpath_s(pModelFileName, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

	if (!strcmp(szExt, ".dat"))
	{
		m_datLoad = true;
		return Load_Dat(pModelFilePath, pModelFileName);
	}

	ZeroMemory(&m_tModel, sizeof(TMODEL));
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	XMStoreFloat4x4(&m_tModel.Pivot, PivotMatrix);

	//fbx에서 정보 불러오기
	char		szFullPath[MAX_PATH] = "";

	strcpy_s(m_tModel.Path, pModelFilePath);
	strcpy_s(szFullPath, pModelFilePath);
	strcpy_s(m_tModel.Name, pModelFileName);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;

	iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;

	m_pAIScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	//뼈
	/*Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0);

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
	return pSour->Get_Depth() < pDest->Get_Depth();
	});*/

	/* 모델을 구성하는 메시들을 만든다. */
	Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0);

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	m_tModel.NumHierarcky = m_HierarchyNodes.size();
	m_tModel.tHierarcky = new THIERARCKY[m_tModel.NumHierarcky];
	for (int i = 0; i < m_HierarchyNodes.size(); ++i)
	{
		m_HierarchyNodes[i]->Set_Index(i);
		strcpy_s(m_tModel.tHierarcky[i].szName, m_HierarchyNodes[i]->Get_Name());
		XMStoreFloat4x4(&(m_tModel.tHierarcky[i].Offset), m_HierarchyNodes[i]->Get_OffSetMatrix());
		m_tModel.tHierarcky[i].Transformation = m_HierarchyNodes[i]->Get_Trans();
		m_tModel.tHierarcky[i].iDepth = m_HierarchyNodes[i]->Get_Depth();
		CHierarchyNode* pParent = m_HierarchyNodes[i]->Get_Parent();
		if (pParent != nullptr)
		{
			m_HierarchyNodes[i]->Set_ParentIndex(pParent->Get_Index());
			m_tModel.tHierarcky[i].ParentIndex = pParent->Get_Index();
		}
	}


	if (FAILED(Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;
	// 해당 메쉬에서 가질 텍스쳐를 불러온다
	if (FAILED(Ready_Materials(pModelFilePath, &(m_tModel.AllMaterials))))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	m_iPreAnimIndex = m_Animations.size();

	return S_OK;
}

HRESULT CAnimModel::Initialize(void * pArg)
{
	//Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0);

	//sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	//{
	//	return pSour->Get_Depth() < pDest->Get_Depth();
	//});

	//for (_uint i = 0; i < m_HierarchyNodes.size(); ++i)
	//{
	//	m_HierarchyNodes[i]->Set_Index(i);
	//}
	m_isCloned = true;
	for (_uint i = 0; i < m_HierarchyNodes.size(); ++i)
	{
		CHierarchyNode*		pHierarchyNode = m_HierarchyNodes[i]->Clone();
		if (nullptr == pHierarchyNode)
			return E_FAIL;
		Safe_Release(m_HierarchyNodes[i]);
		m_HierarchyNodes[i] = pHierarchyNode;
		if (i != 0)
			pHierarchyNode->Set_Parent(m_HierarchyNodes[pHierarchyNode->Get_ParentIndex()]);
	}

	for (_uint i = 0; i < m_Meshes.size(); ++i)
	{
		CAnimMeshContainer*		pMeshContainer = (CAnimMeshContainer*)m_Meshes[i]->Clone();
		if (nullptr == pMeshContainer)
			return E_FAIL;
		Safe_Release(m_Meshes[i]);
		m_Meshes[i] = pMeshContainer;
	}

	if (m_datLoad == true)
	{
		_uint iNumMeshes = m_Meshes.size();
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (nullptr != m_Meshes[i])
				m_Meshes[i]->SetUp_HierarchyNodes(this, m_tModel.tMeshes[i]);
		}
	}
	else
	{
		_uint iNumMeshes = m_Meshes.size();
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (nullptr != m_Meshes[i])
				m_Meshes[i]->SetUp_HierarchyNodes(this, m_pAIScene->mMeshes[i], &(m_tModel.tMeshes[i]));
		}
	}
	for (_uint i = 0; i < m_HierarchyNodes.size(); ++i)
	{
		XMStoreFloat4x4(&(m_tModel.tHierarcky[i].Offset), m_HierarchyNodes[i]->Get_OffSetMatrix());
	}


	for (_uint i = 0; i< m_Animations.size(); ++i)
	{
		CAnimation*		pAnimation = m_Animations[i]->Clone(this);
		if (nullptr == pAnimation)
			return E_FAIL;

		Safe_Release(m_Animations[i]);
		m_Animations[i] = pAnimation;
	}
	return S_OK;
}

HRESULT CAnimModel::Load_Dat(const char * pModelFilePath, const char * pModelFileName)
{

#pragma region loadfile

	char szFullPath[MAX_PATH] = ""; //여기에 넣을 예정



	strcpy_s(szFullPath, pModelFilePath);
	//파일 경로 넣고
	strcat_s(szFullPath, pModelFileName);
	//파일 이름 넣고


	_tchar			szWideFullPath[MAX_PATH] = TEXT("");

	MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);

	HANDLE		hFile = CreateFile(szWideFullPath,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);



	DWORD		dwByte = 0;

	ReadFile(hFile, &(m_tModel.bAnim), sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &(m_tModel.Path), sizeof(char) * 260, &dwByte, nullptr);
	ReadFile(hFile, &(m_tModel.Name), sizeof(char) * 260, &dwByte, nullptr);
	ReadFile(hFile, &(m_tModel.Pivot), sizeof(_float4x4), &dwByte, nullptr);
	m_PivotMatrix = m_tModel.Pivot;
	ReadFile(hFile, &(m_tModel.NumMeshes), sizeof(int), &dwByte, nullptr);
	m_tModel.tMeshes = new TANIMCONTAINER[m_tModel.NumMeshes];
	for (int i = 0; i < m_tModel.NumMeshes; ++i)
	{
		ReadFile(hFile, &(m_tModel.tMeshes[i].szName), sizeof(char) * 260, &dwByte, nullptr);
		ReadFile(hFile, &(m_tModel.tMeshes[i].iIndex), sizeof(int), &dwByte, nullptr);

		ReadFile(hFile, &(m_tModel.tMeshes[i].NumVertices), sizeof(int), &dwByte, nullptr);
		m_tModel.tMeshes[i].pVertices = new VTXANIMMODEL[m_tModel.tMeshes[i].NumVertices];
		for (int j = 0; j < m_tModel.tMeshes[i].NumVertices; ++j)
		{
			ReadFile(hFile, &(m_tModel.tMeshes[i].pVertices[j]), sizeof(VTXANIMMODEL), &dwByte, nullptr);
			if (dwByte == 0)
				int i = 9;
		}

		ReadFile(hFile, &(m_tModel.tMeshes[i].NumFaces), sizeof(int), &dwByte, nullptr);
		m_tModel.tMeshes[i].pIndices = new FACEINDICES32[m_tModel.tMeshes[i].NumFaces];
		for (int j = 0; j < m_tModel.tMeshes[i].NumFaces; ++j)
		{
			ReadFile(hFile, &(m_tModel.tMeshes[i].pIndices[j]), sizeof(FACEINDICES32), &dwByte, nullptr);
		}
		if (dwByte == 0)
			int i = 9;
		ReadFile(hFile, &(m_tModel.tMeshes[i].NumBones), sizeof(int), &dwByte, nullptr);
		m_tModel.tMeshes[i].BoneIndecis = new int[m_tModel.tMeshes[i].NumBones];
		for (int j = 0; j < m_tModel.tMeshes[i].NumBones; ++j)
		{
			ReadFile(hFile, &(m_tModel.tMeshes[i].BoneIndecis[j]), sizeof(int), &dwByte, nullptr);
		}

	}
	ReadFile(hFile, &(m_tModel.AllMaterials.NumMaterials), sizeof(int), &dwByte, nullptr);
	m_tModel.AllMaterials.tMaterial = new TMATERIAL[m_tModel.AllMaterials.NumMaterials];
	for (int i = 0; i < m_tModel.AllMaterials.NumMaterials; ++i)
	{
		ReadFile(hFile, &(m_tModel.AllMaterials.tMaterial[i]), sizeof(TMATERIAL), &dwByte, nullptr);
	}
	ReadFile(hFile, &(m_tModel.NumHierarcky), sizeof(int), &dwByte, nullptr);
	m_tModel.tHierarcky = new THIERARCKY[m_tModel.NumHierarcky];
	for (int i = 0; i < m_tModel.NumHierarcky; ++i)
	{
		ReadFile(hFile, &(m_tModel.tHierarcky[i]), sizeof(THIERARCKY), &dwByte, nullptr);
	}

	ReadFile(hFile, &(m_tModel.NumAnim), sizeof(int), &dwByte, nullptr);
	m_tModel.tAnim = new TANIM[m_tModel.NumAnim];
	for (int i = 0; i < m_tModel.NumAnim; ++i)
	{
		ReadFile(hFile, &(m_tModel.tAnim[i].szName), sizeof(char) * 260, &dwByte, nullptr);
		ReadFile(hFile, &(m_tModel.tAnim[i].fDuration), sizeof(float), &dwByte, nullptr);
		ReadFile(hFile, &(m_tModel.tAnim[i].fTickPerSecond), sizeof(float), &dwByte, nullptr);
		ReadFile(hFile, &(m_tModel.tAnim[i].fLimitTime), sizeof(float), &dwByte, nullptr);
		ReadFile(hFile, &(m_tModel.tAnim[i].fMaxTermTime), sizeof(float), &dwByte, nullptr);
		ReadFile(hFile, &(m_tModel.tAnim[i].NumChannel), sizeof(int), &dwByte, nullptr);
		m_tModel.tAnim[i].Bones = new int[m_tModel.tAnim[i].NumChannel];
		m_tModel.tAnim[i].Channels = new TCHANNEL[m_tModel.tAnim[i].NumChannel];
		for (int j = 0; j < m_tModel.tAnim[i].NumChannel; ++j)
		{
			ReadFile(hFile, &(m_tModel.tAnim[i].Bones[j]), sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, &(m_tModel.tAnim[i].Channels[j].NumKeyFrame), sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, &(m_tModel.tAnim[i].Channels[j].szName), sizeof(char) * 260, &dwByte, nullptr);
			m_tModel.tAnim[i].Channels[j].KeyFrames = new KEYFRAME[m_tModel.tAnim[i].Channels[j].NumKeyFrame];
			for (int k = 0; k < m_tModel.tAnim[i].Channels[j].NumKeyFrame; ++k)
			{
				ReadFile(hFile, &(m_tModel.tAnim[i].Channels[j].KeyFrames[k]), sizeof(KEYFRAME), &dwByte, nullptr);
			}
		}
	}
	if (dwByte == 0)
		int i = 9;
	int i = 0;
	ReadFile(hFile, &(i), sizeof(int), &dwByte, nullptr);
	if (dwByte == 0)
		int i = 9;
	CloseHandle(hFile);

#pragma endregion

	//하이어라키
	for (int i = 0; i < m_tModel.NumHierarcky; ++i)
	{
		CHierarchyNode* pNode = CHierarchyNode::Create(m_tModel.tHierarcky[i], &m_HierarchyNodes);
		if (pNode == nullptr)
		{
			MSG_BOX(TEXT("Fail to pushback tHierarcky"));
			return E_FAIL;
		}
		pNode->Set_Depth(m_tModel.tHierarcky[i].iDepth);
		pNode->Set_Index(i);
		m_HierarchyNodes.push_back(pNode);
	}

	//메쉬
	m_iNumMeshes = m_tModel.NumMeshes;
	for (int i = 0; i < m_tModel.NumMeshes; ++i)
	{
		//ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _fmatrix PivotMatrix, TANIMCONTAINER _tIn)
		CAnimMeshContainer*		pMeshContainer = CAnimMeshContainer::Create(m_pDevice, m_pContext, XMLoadFloat4x4(&(m_tModel.Pivot)), m_tModel.tMeshes[i]);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}
	m_iNumMaterials = m_tModel.NumMeshes;
	//머테리얼
	if (FAILED(Ready_Materials(m_tModel.AllMaterials)))
		return E_FAIL;

	if (FAILED(Ready_Animations(m_tModel.NumAnim)))
		return E_FAIL;

	m_iPreAnimIndex = m_Animations.size();
	return S_OK;
}

HRESULT CAnimModel::Load_Default(const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	return S_OK;
}


HRESULT CAnimModel::Set_AnimationIndex(_uint _AnimationIndex)
{

	if (m_Animations.size() <= _AnimationIndex)
	{
		MSG_BOX(TEXT("Over the AnimationIndex"));
		return E_FAIL;
	}
	//지금 들어온 애니메이션이 현재와 다르다면, 현재 애니메이션을 프리에 넣고, 현재 애니메이션을 바꿔준 후
	// 

	if (m_iPreAnimIndex == m_Animations.size())
	{
		m_iPreAnimIndex = _AnimationIndex;
		m_iCurrentAnimIndex = _AnimationIndex;
	}

	if (m_iCurrentAnimIndex != _AnimationIndex)
	{
		m_Animations[m_iCurrentAnimIndex]->Reset_KeyFrame();
		m_iCurrentAnimIndex = _AnimationIndex;
	}

	return S_OK;
}

_bool CAnimModel::Play_Animation(_float _fTimeDelta, _float4* _vAnim, _float* pOut, _bool& _bAgain)
{
	_bool _bResult = false;

	if (_bAgain == true)
	{
		//이미 진행하는 애니메이션과 보간한다.
		if (m_Animations[m_iCurrentAnimIndex]->Play_Animation(_fTimeDelta, m_Animations[m_iCurrentAnimIndex], m_TotalChannel))
		{
			m_iPreAnimIndex = m_iCurrentAnimIndex;
			_bAgain = false;
			return _bResult;
		}
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Set_CombinedTransformation(_vAnim, true);
		}
		return _bResult;
	}

	if (m_iPreAnimIndex == m_iCurrentAnimIndex)
	{
		if (m_Animations[m_iCurrentAnimIndex]->Play_Animation(_fTimeDelta, pOut))
		{
			//_bResult = true;
			return true;
		}
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Set_CombinedTransformation(_vAnim, m_PivotMatrix);
		}
	}
	else if (m_iPreAnimIndex != m_iCurrentAnimIndex)//다른 애니메이션 보간
	{
		if (m_Animations[m_iPreAnimIndex]->Play_Animation(_fTimeDelta, m_Animations[m_iCurrentAnimIndex], m_TotalChannel))
		{
			m_iPreAnimIndex = m_iCurrentAnimIndex;
			return _bResult;
		}
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Set_CombinedTransformation(_vAnim, true);
		}
	}

	return _bResult;
}

_bool CAnimModel::Play_Animation(_float _fTimeDelta, _float4 * _vAnim, _float * pOut, _bool & _bAgain, char * RootY)
{
	_bool _bResult = false;

	if (_bAgain == true)
	{
		//이미 진행하는 애니메이션과 보간한다.
		if (m_Animations[m_iCurrentAnimIndex]->Play_Animation(_fTimeDelta, m_Animations[m_iCurrentAnimIndex], m_TotalChannel))
		{
			m_iPreAnimIndex = m_iCurrentAnimIndex;
			_bAgain = false;
			return _bResult;
		}
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Set_CombinedTransformation(_vAnim, true, RootY);
		}
		return _bResult;
	}

	if (m_iPreAnimIndex == m_iCurrentAnimIndex)
	{
		if (m_Animations[m_iCurrentAnimIndex]->Play_Animation(_fTimeDelta, pOut))
		{
			//_bResult = true;
			return true;
		}
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Set_CombinedTransformation(_vAnim, m_PivotMatrix, RootY);
		}
	}
	else if (m_iPreAnimIndex != m_iCurrentAnimIndex)//다른 애니메이션 보간
	{
		if (m_Animations[m_iPreAnimIndex]->Play_Animation(_fTimeDelta, m_Animations[m_iCurrentAnimIndex], m_TotalChannel))
		{
			m_iPreAnimIndex = m_iCurrentAnimIndex;
			return _bResult;
		}
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Set_CombinedTransformation(_vAnim, true, RootY);
		}
	}

	return _bResult;
}

void CAnimModel::DirectAnim(_uint _iAnimIndex)
{
	KEYFRAME Dest;
	for (int i = 0; i < m_HierarchyNodes.size(); ++i)
	{
		vector<CChannel*> NextAnimChannels = m_Animations[_iAnimIndex]->Get_vecChannel();

		if (NextAnimChannels[i] == nullptr)
		{
			Dest = m_HierarchyNodes[i]->Get_DefaultKeyFrame();
		}
		else
		{
			Dest = NextAnimChannels[i]->Get_KeyFrame(0);
		}

		CHierarchyNode* pNode = m_HierarchyNodes[i];

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&Dest.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&Dest.vRotation), XMVectorSetW(XMLoadFloat3(&Dest.vPosition), 1.f));

		if (nullptr != pNode)
			pNode->Set_Transformation(TransformationMatrix);
	}
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Set_CombinedTransformation(nullptr, true);
	}
	m_iPreAnimIndex = _iAnimIndex;
}

_bool CAnimModel::Repeat_Animation(_float _fTimeDelta, _float4 * _vAnim, _float * pOut)
{
	_bool _bResult = false;
	if (m_Animations[m_iPreAnimIndex]->Play_Animation(_fTimeDelta, m_Animations[m_iCurrentAnimIndex], m_TotalChannel))
	{
		m_iPreAnimIndex = m_iCurrentAnimIndex;
		return _bResult;
	}
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Set_CombinedTransformation(_vAnim, true);
	}
	return _bResult;
}


HRESULT CAnimModel::Render(CShader * pShader, _uint _iPass, _uint _iMeshIndex)
{
	_float4x4		BoneMatrices[256];


	m_Meshes[_iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	if (FAILED(pShader->Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 256)))
		return E_FAIL;


	pShader->Begin(_iPass);

	m_Meshes[_iMeshIndex]->Render();
	return S_OK;
}

_uint CAnimModel::Get_MaterialIndex(_uint _iMeshIndex) const
{
	return m_Meshes[_iMeshIndex]->Get_MaterialIndex();
}

HRESULT CAnimModel::Ready_MeshContainers(_fmatrix PivotMatrix)
{

	m_iNumMeshes = m_pAIScene->mNumMeshes;
	m_tModel.NumMeshes = m_iNumMeshes;
	m_tModel.tMeshes = new TANIMCONTAINER[m_tModel.NumMeshes];
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CAnimMeshContainer*		pMeshContainer = CAnimMeshContainer::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], this, PivotMatrix, &(m_tModel.tMeshes[i]));
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CAnimModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;
	m_tModel.NumAnim = m_iNumAnimations;
	m_tModel.tAnim = new TANIM[m_tModel.NumAnim];
	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*		pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation*			pAnimation = CAnimation::Create(pAIAnimation, this, i);

		if (nullptr == pAnimation)
			return E_FAIL;
		m_tModel.tAnim[i] = pAnimation->Get_ForSave();
		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CAnimModel::Ready_Animations(int _Num)
{
	m_iNumAnimations = _Num;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation*			pAnimation = CAnimation::Create(this, m_tModel.tAnim[i], i);

		if (nullptr == pAnimation)
			return E_FAIL;
		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}



CAnimModel * CAnimModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	CAnimModel*			pInstance = new CAnimModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, pModelFileName, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CAnimModel::Clone(void * pArg)
{
	CAnimModel*			pInstance = new CAnimModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimModel::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	if (m_isCloned == false)
	{
		for (int i = 0; i < m_tModel.NumMeshes; ++i)
		{
			Safe_Delete_Array(m_tModel.tMeshes[i].pVertices);
			Safe_Delete_Array(m_tModel.tMeshes[i].pIndices);
			Safe_Delete_Array(m_tModel.tMeshes[i].BoneIndecis);
		}
		Safe_Delete_Array(m_tModel.tMeshes);


		Safe_Delete_Array(m_tModel.AllMaterials.tMaterial);

		Safe_Delete_Array(m_tModel.tHierarcky);

		//for (int i = 0; i < m_tModel.NumAnim; ++i)
		//{
		//	for (int j = 0; j < m_tModel.tAnim[i].NumChannel; ++j)
		//	{
		//		Safe_Delete_Array(m_tModel.tAnim[i].Channels[j].KeyFrames);
		//	}
		//	Safe_Delete_Array(m_tModel.tAnim[i].Channels);
		//}
		Safe_Delete_Array(m_tModel.tAnim);
	}
}

CHierarchyNode * CAnimModel::Get_HierarchyNode(char * pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNodeName, pNode->Get_Name());
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

HRESULT CAnimModel::Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{

	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode, pParent, iDepth);
	++iDepth;
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth);
	}

	return S_OK;
}

TANIMMODEL CAnimModel::Get_ForSaveAnim()
{
	m_tModel.NumAnim = m_Animations.size();
	Safe_Delete_Array(m_tModel.tAnim);
	m_tModel.tAnim = new TANIM[m_tModel.NumAnim];
	for (int i = 0; i < m_tModel.NumAnim; ++i)
	{
		m_tModel.tAnim[i] = m_Animations[i]->Get_ForSave();
	}

	return m_tModel;
}