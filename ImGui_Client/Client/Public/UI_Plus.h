#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Plus final : public CGameObject
{
public:
	typedef struct tag_UI_Desc
	{
		//버퍼갯수
		//버퍼갯수에 대응되는 텍스쳐 - 그럼 벡터로 갖고있어야하고
		_float3		vPos;
		_float3		vScale;
		_tchar*		szTextureTag;
		_tchar*		szUIName;
		//_uint		iIncludeNum;
		
	}UI_DESC;
	typedef struct tag_UI_Include_Desc
	{
		CVIBuffer_Rect* vecBuffer;
		

	}UI_Include_DESC;
	enum PASS { PASS_NONPICK, PASS_PICK, PASS_END };
private:
	CUI_Plus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Plus(const CUI_Plus& rhs);
	virtual ~CUI_Plus() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void	Set_Info(UI_DESC _tInfo);
	UI_DESC	Get_Info(UI_DESC _tInfo) { return m_tInfo; }

	void Set_Pass(PASS _ePass) { m_ePass = _ePass; }

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pTextureCom2 = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;
	PASS					m_ePass;

private:
	HRESULT Ready_Components();

public:
	static CUI_Plus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	UI_DESC m_tInfo;
};

END