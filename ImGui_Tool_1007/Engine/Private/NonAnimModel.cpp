#include "..\Public\NonAnimModel.h"

#include "MeshContainer.h"

#include "Texture.h"

#include "PipeLine.h"


CNonAnimModel::CNonAnimModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CModel(pDevice, pContext)
{
}

CNonAnimModel::CNonAnimModel(const CNonAnimModel & rhs)
	: CModel(rhs)
	, m_Meshes(rhs.m_Meshes)
	, m_tModel(rhs.m_tModel)
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);
}

HRESULT CNonAnimModel::Initialize_Prototype(const char * pModelFilePath, const char * pModelFileName)
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
	m_tModel.bAnim = false;
	strcpy_s(m_tModel.Name, pModelFileName);
	strcpy_s(m_tModel.Path, pModelFilePath);
	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;


	iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	//	iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;

	m_pAIScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* 모델을 구성하는 메시들을 만든다. */
	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	// 텍스쳐들을 불러온다
	if (FAILED(Ready_Materials(pModelFilePath, &(m_tModel.AllMaterials))))
		return E_FAIL;

	return S_OK;
}

HRESULT CNonAnimModel::Initialize(void * pArg)
{
	m_isCloned = true;
	return S_OK;
}

HRESULT CNonAnimModel::Render(_uint _iMeshIndex)
{
	m_Meshes[_iMeshIndex]->Render();
	return S_OK;
}

_bool CNonAnimModel::Picking(CTransform * pTransform, _vector & pOut)
{
	_vector	_vFinalPos{ 0.f,0.f,0.f,1.f };
	_float	_vFinalDis = 10000.f;

	_float	_vCurDis = 0.f;

	_bool	_bPicking = false;

	for (auto& _Mesh : m_Meshes)
	{
		if (_Mesh->Picking(pTransform, pOut))
		{
			
			_vCurDis = XMVectorGetX(XMVector3Length(pOut - XMLoadFloat4(&(CPipeLine::Get_Instance()->Get_CamPosition()))));
			if (_vCurDis < _vFinalDis)
			{
				_vFinalDis = _vCurDis;
				_vFinalPos = pOut;
			}
			_bPicking = true;

		}
			
	}
	pOut = _vFinalPos;
	return _bPicking;
}

HRESULT CNonAnimModel::Ready_MeshContainers()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;
	m_tModel.NumMeshes = m_iNumMeshes;
	
	m_tModel.tMeshes = new TCONTAINER[m_tModel.NumMeshes];

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], &(m_tModel.tMeshes[i]));
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CNonAnimModel::Load_Dat(const char * pModelFilePath, const char * pModelFileName)
{
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
	ReadFile(hFile, &(m_tModel.NumMeshes), sizeof(int), &dwByte, nullptr);
	m_tModel.tMeshes = new TCONTAINER[m_tModel.NumMeshes];
	for (int i = 0; i < m_tModel.NumMeshes; ++i)
	{
		ReadFile(hFile, &(m_tModel.tMeshes[i].szName), sizeof(char) * 260, &dwByte, nullptr);
		ReadFile(hFile, &(m_tModel.tMeshes[i].iIndex), sizeof(int), &dwByte, nullptr);

		ReadFile(hFile, &(m_tModel.tMeshes[i].NumVertices), sizeof(int), &dwByte, nullptr);
		m_tModel.tMeshes[i].pVertices = new VTXMODEL[m_tModel.tMeshes[i].NumVertices];
		for (int j = 0; j < m_tModel.tMeshes[i].NumVertices; ++j)
		{
			ReadFile(hFile, &(m_tModel.tMeshes[i].pVertices[j]), sizeof(VTXMODEL), &dwByte, nullptr);
		}

		ReadFile(hFile, &(m_tModel.tMeshes[i].NumFaces), sizeof(int), &dwByte, nullptr);
		m_tModel.tMeshes[i].pIndices = new FACEINDICES32[m_tModel.tMeshes[i].NumFaces];
		for (int j = 0; j < m_tModel.tMeshes[i].NumFaces; ++j)
		{
			ReadFile(hFile, &(m_tModel.tMeshes[i].pIndices[j]), sizeof(FACEINDICES32), &dwByte, nullptr);
		}
	}
	ReadFile(hFile, &(m_tModel.AllMaterials.NumMaterials), sizeof(int), &dwByte, nullptr);
	m_tModel.AllMaterials.tMaterial = new TMATERIAL[m_tModel.AllMaterials.NumMaterials];
	for (int i = 0; i < m_tModel.AllMaterials.NumMaterials; ++i)
	{
		ReadFile(hFile, &(m_tModel.AllMaterials.tMaterial[i]), sizeof(TMATERIAL), &dwByte, nullptr);
	}

	CloseHandle(hFile);

#pragma endregion

	//메쉬
	m_iNumMeshes = m_tModel.NumMeshes;
	for (int i = 0; i < m_tModel.NumMeshes; ++i)
	{
		//ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _fmatrix PivotMatrix, TANIMCONTAINER _tIn)
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_tModel.tMeshes[i]);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}
	m_iNumMaterials = m_tModel.NumMeshes;
	//머테리얼
	if (FAILED(Ready_Materials(m_tModel.AllMaterials)))
		return E_FAIL;
	return S_OK;
}

_uint CNonAnimModel::Get_MaterialIndex(_uint _iMeshIndex) const
{

	return m_Meshes[_iMeshIndex]->Get_MaterialIndex();

}

CNonAnimModel * CNonAnimModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, const char * pModelFileName)
{
	CNonAnimModel*			pInstance = new CNonAnimModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, pModelFileName)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNonAnimModel::Clone(void * pArg)
{
	CModel*			pInstance = new CNonAnimModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNonAnimModel::Free()
{
	__super::Free();
	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	if (m_isCloned == false)
	{
		for (int i = 0; i < m_tModel.NumMeshes; ++i)
		{
			Safe_Delete_Array(m_tModel.tMeshes[i].pVertices);
			Safe_Delete_Array(m_tModel.tMeshes[i].pIndices);

		}
		Safe_Delete_Array(m_tModel.tMeshes);
		
		Safe_Delete_Array(m_tModel.AllMaterials.tMaterial);
	}
}