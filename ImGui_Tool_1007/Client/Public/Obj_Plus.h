#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CObj_Plus abstract : public CGameObject
{

public:
	struct OBJ_DESC
	{
		LEVEL		eLevel;
		_float4x4	matWorld;
		_tchar		szModelTag[MAX_PATH];
	};
	enum PASS { PASS_NONPICK, PASS_PICK, PASS_ADD, PASS_END };
protected:
	CObj_Plus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObj_Plus(const CObj_Plus& rhs);
	virtual ~CObj_Plus() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render() = 0;

public:
	virtual void		Set_Info(OBJ_DESC _tInfo) = 0;
	OBJ_DESC	Get_Info() { return m_tInfo; }

	void Set_Pass(PASS _ePass) { m_ePass = _ePass; }

	void		ImGuiTick();

protected:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

	OBJ_DESC				m_tInfo;
	PASS					m_ePass;

public:
	virtual void Free() override;
};

END