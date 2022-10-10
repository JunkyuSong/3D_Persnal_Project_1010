#pragma once
#include "Model.h"
#include "AnimModel.h"

BEGIN(Engine)
class ENGINE_DLL CMotionTrail final :
	public CModel
{
public:
	CMotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMotionTrail() = default;

public:
	HRESULT Initialize_Prototype(CAnimModel* pParent);

public:
	HRESULT Render(class CShader* pShader, _uint _iPass, _uint _iMeshIndex);
	_uint   Get_MaterialIndex(_uint _iMeshIndex) const;
	const _float4x4&		Get_PivotMatrix() { return m_PivotMatrix; }

public:
	void	Set_CombinedMat(vector<class CHierarchyNode*>* _vecParentBones, _float4x4 _pParentWorld);

	

public:
	static  CMotionTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CAnimModel* pParent);
	virtual CComponent* Clone(void* pArg = nullptr) { return nullptr; }
	virtual void Free() override;

public:
	vector<class CHierarchyNode*>* Get_HierarchyNodeVector() { return &m_HierarchyNodes; }
	void Tick(_float _timedelta) { m_TickTime += _timedelta; }
	_float	Get_Tick() { return m_TickTime; }
	void	Set_Tick(_float _Tick) { m_TickTime = _Tick; }

private:
	TANIMMODEL							m_tModel;

	CAnimModel*							m_pParent;

	_float4x4							m_PivotMatrix;
	vector<class CHierarchyNode*>		m_HierarchyNodes;

	_float4x4							m_ParentWorld;

	typedef vector<class CAnimMeshContainer*>	MESHES;
	MESHES								m_Meshes;

	_float								m_TickTime = 0.f;
};

END