#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CAnimModel;
class CTransform;
END

BEGIN(Client)
class CMonster abstract:
	public CGameObject
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

protected:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CAnimModel*				m_pModelCom = nullptr;


public:
	virtual void Free() override;
};

END