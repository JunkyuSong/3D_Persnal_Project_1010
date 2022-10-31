#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
END

BEGIN(Client)

class CPosCube final : public CGameObject
{
public:
	typedef struct tagPosCubeDesc
	{
		_float4 vColor;
		_float3 vPos;
		_float fCamSpeed = 0.f;
		_float fStopLimit = 0.f;
	}POSCUBEDESC;

private:
	CPosCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPosCube(const CPosCube& rhs);
	virtual ~CPosCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void GetKeyInput(_float fTimeDelta);
	_float3 Get_Pos();
	void Set_Pos(_float3 vPos);
	_float Get_CamSpeed() { return m_fCamSpeed; }
	_float Get_StopLimit() { return m_fStopLimit; }
	_float4 Get_RGB() { return m_vRGB; }
	void Set_CamSpeed(_float fSpeed) { m_fCamSpeed = fSpeed; }
	void Set_StopLimit(_float fLimit) { m_fStopLimit = fLimit; m_fStopTemp = m_fStopLimit; }
	bool Get_Stop() {return m_bStop; }
	void Set_Stop() { m_bStop = true; }
	void Stop(_float fTimeDelta);
	void PlayEnd() { m_fStopLimit = m_fStopTemp; }



private:
	_float m_fStopLimit = 0.f;
	_float m_fStopTemp = 0.f;
	_float m_fStopAcc = 0.f;
	_float4 m_vRGB;
	_float m_fCamSpeed = 0.f;
	bool m_bStop = false; 

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Cube*	m_pVIBufferCom = nullptr;



private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CPosCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END