#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point final : public CVIBuffer
{
private:
	CVIBuffer_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point(const CVIBuffer_Point& rhs);
	virtual ~CVIBuffer_Point() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBuffer_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END