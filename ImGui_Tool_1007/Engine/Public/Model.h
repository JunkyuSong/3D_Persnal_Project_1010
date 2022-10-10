#pragma once

#include "Component.h"
#include "Engine_Binary.h"
BEGIN(Engine)

class ENGINE_DLL CModel abstract : public CComponent
{
public:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	//virtual HRESULT Initialize_Prototype(const char* pModelFilePath, const char* pModelFileName) = 0;
	//virtual HRESULT Initialize(void* pArg) = 0;
	
public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eType, const char* pConstantName);

	_uint	Get_NumMesh() 
	{
		return m_iNumMeshes;
	}

protected:
	const aiScene*				m_pAIScene = nullptr; //정보를 담고 있는 객체
	Assimp::Importer			m_Importer; // 씬 객체를 채워주는 구조체

protected:
	_uint									m_iNumMeshes = 0;
	_bool									m_datLoad = false;


protected:
	HRESULT Ready_Materials(TMATERIALS _tIn);
	HRESULT Ready_Materials(const char* pModelFilePath, TMATERIALS*	_pOut);

public:
	_uint									m_iNumMaterials = 0;
	vector<MATERIALDESC>					m_Materials;

public:
	/*static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, const char* pModelFileName);
	virtual CComponent* Clone(void* pArg = nullptr);*/
	virtual void Free() override;
};

END