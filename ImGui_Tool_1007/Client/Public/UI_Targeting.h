#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CRenderer;
class CVIBuffer_Point;
END

BEGIN(Client)

class CUI_Targeting :
	public CGameObject
{

public:
	CUI_Targeting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Targeting(const CUI_Targeting& rhs);
	virtual ~CUI_Targeting() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	void		Targeting(_vector _vPlayerPos, _vector _vMonsterPos);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	_float3					m_vTargetPos{ 0.f, 0.f, 0.f };
	_float3					m_vPos[21];
	
	_float3					m_vRight;
	_float3					m_vUp;
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransCom = nullptr; // 플레이어 따라다님
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Point*		m_pVIBufferCom = nullptr; // 
	//CTexture*				m_pTextureCom[BAR_END] = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CUI_Targeting* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg) override;
	virtual void Free() override;

};

END