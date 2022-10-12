#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTransform;
class CMotionTrail;
class CHierarchyNode;
END

BEGIN(Client)

class CDummy final:
	public CGameObject
{
private:
	CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy(const CDummy& rhs);
	virtual ~CDummy() = default;

public:
	virtual HRESULT Initialize_Prototype(CMotionTrail* _pModel, _float4x4 _ParentWorld);
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

	void	Set_World(_float4x4 _ParentWorld);

	_float	Get_Tick();
	void	Set_Tick(_float _tick);
	void	Set_CombinedMat(vector<class CHierarchyNode*>* _vecParentBones, _float4x4 _pParentWorld);

	virtual CGameObject *	Clone(void * pArg) override;
	static	CDummy*			Creat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CMotionTrail* _pModel,_float4x4 _ParentWorld);
	virtual void			Free() override;

private:
	CMotionTrail*		m_pModelCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
};

END