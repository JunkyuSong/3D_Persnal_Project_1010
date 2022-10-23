#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CNonAnimModel;
END

BEGIN(Client)

class CStage_02 final : public CGameObject
{
private:
	CStage_02(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage_02(const CStage_02& rhs);
	virtual ~CStage_02() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick( _float fTimeDelta);
	virtual void LateTick( _float fTimeDelta);
	virtual HRESULT Render();

	_bool	Picking(_float3& _vPos);

public:

	void		ImGuiTick();

private:
	CNonAnimModel*			m_pModelCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CStage_02* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END