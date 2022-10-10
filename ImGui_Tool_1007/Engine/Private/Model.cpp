#include "..\Public\Model.h"
#include "MeshContainer.h"

#include "Texture.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_datLoad(rhs.m_datLoad)
{


	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}
}


HRESULT CModel::SetUp_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eType, const char * pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (m_Materials[iMaterialIndex].pTexture[eType] == nullptr)
		return S_OK;

	return m_Materials[iMaterialIndex].pTexture[eType]->Set_SRV(pShader, pConstantName);	
}

HRESULT CModel::Ready_Materials(TMATERIALS _tIn)
{
	m_iNumMaterials = _tIn.NumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		_MaterialDesc;
		ZeroMemory(&_MaterialDesc, sizeof(MATERIALDESC));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			if (!lstrcmp(_tIn.tMaterial[i].szPath[j], TEXT("")))
				continue;
			_MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, _tIn.tMaterial[i].szPath[j]);
			//텍스쳐 생성해서 머테리얼 배열에 넣는다

			if (nullptr == _MaterialDesc.pTexture[j])
				return E_FAIL;
		}
		m_Materials.push_back(_MaterialDesc);
	}
	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFilePath, TMATERIALS* _pOut)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;
	
	m_iNumMaterials = m_pAIScene->mNumMaterials;
	_pOut->NumMaterials = m_iNumMaterials;
	_pOut->tMaterial = new TMATERIAL[m_iNumMaterials];
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		_MaterialDesc;
		ZeroMemory(&_MaterialDesc, sizeof(MATERIALDESC));

		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];
		//씬 객체에서 머테리얼 가져오고
		//그걸 이제 머테리얼의 배열에 채워넣는다

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		strPath;
			//aiTextureType : 이넘값
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				//strPath에 해당 fbx에 들어있는 텍스쳐의 주소(각자의 컴퓨터에 있는 주소로 들어감)를 담는다
			{
				lstrcpy(_pOut->tMaterial[i].szPath[j], TEXT(""));
				continue;
			}
				
			//없으면 다음으로

			char szFullPath[MAX_PATH] = ""; //여기에 넣을 예정
			char szFileName[MAX_PATH] = ""; //파일 이름
			char szExt[MAX_PATH] = ""; //확장자

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
			//strPath에 파일 경로 떼고 파일 이름이랑 확장자 뺀다
			// ex) abc / .png

			strcpy_s(szFullPath, pModelFilePath);
			//파일 경로 넣고
			strcat_s(szFullPath, szFileName);
			//파일 이름 넣고
			strcat_s(szFullPath, szExt);
			//경로 넣고

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");
			
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);
			//_tchar로 변환 _ szFullPath만큼의 사이즈인 szFullPath를 szWideFullPath에 담는다
			
			lstrcpy(_pOut->tMaterial[i].szPath[j], szWideFullPath);

			_MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
			//텍스쳐 생성해서 머테리얼 배열에 넣는다

			if (nullptr == _MaterialDesc.pTexture[j])
				return E_FAIL;
			//만드는게 실패하면 ㅃㅇ
		}

		m_Materials.push_back(_MaterialDesc);
	}
	return S_OK;
}

//CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, const char * pModelFileName)
//{
//	CModel*			pInstance = new CModel(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, pModelFileName)))
//	{
//		MSG_BOX(TEXT("Failed To Created : CTexture"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//CComponent * CModel::Clone(void * pArg)
//{
//	CModel*			pInstance = new CModel(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX(TEXT("Failed To Cloned : CModel"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

void CModel::Free()
{
	__super::Free();

	for (auto& _Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(_Material.pTexture[i]);
	}
	m_Materials.clear();



	m_Importer.FreeScene();
}
