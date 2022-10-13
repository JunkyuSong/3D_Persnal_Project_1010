#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_PlayerHpBar :
	public CGameObject
{
private:
	enum HPBAR { BAR_LEFTEDGE, BAR_BACK, BAR_RIGHTEDGE, BAR_HP, BAR_END	};

public:
	CUI_PlayerHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerHpBar(const CUI_PlayerHpBar& rhs);
	virtual ~CUI_PlayerHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	class CStatus*			m_pTarget = nullptr;//스테이터스 가져올까ㅇㅇ
	_float					m_fPreHP;
	_float					m_fMaxHP;

	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;

	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransCom[BAR_END] = { nullptr };
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom =  nullptr ;
	CTexture*				m_pTextureCom[BAR_END] = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CUI_PlayerHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg) override;
	virtual void Free() override;

};

END