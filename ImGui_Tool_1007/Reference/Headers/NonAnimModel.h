#pragma once
#include "Model.h"

BEGIN(Engine)
class ENGINE_DLL CNonAnimModel final :
	public CModel
{
public:
	CNonAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNonAnimModel(const CNonAnimModel& rhs);
	virtual ~CNonAnimModel() = default;

public:
	HRESULT Initialize_Prototype(const char* pModelFilePath, const char* pModelFileName);
	HRESULT Initialize(void* pArg);

	HRESULT Render(_uint _iMeshIndex);
	_uint   Get_MaterialIndex(_uint _iMeshIndex) const;

	TMODEL Get_ForSave() {return m_tModel;}

	_bool Picking(class CTransform* pTransform, _vector& pOut);

private:
	HRESULT Ready_MeshContainers();

	HRESULT	Load_Dat(const char * pModelFilePath, const char * pModelFileName);


public:
	static CNonAnimModel * Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, const char * pModelFileName);
	CComponent * CNonAnimModel::Clone(void * pArg);
	virtual void Free() override;

private:
	typedef vector<class CMeshContainer*>	MESHES;
	vector<class CMeshContainer*>			m_Meshes;
	TMODEL									m_tModel;
};

END