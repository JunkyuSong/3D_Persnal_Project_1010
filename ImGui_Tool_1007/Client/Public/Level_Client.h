#pragma once
#include "Client_Defines.h"
#include "../../Reference/Headers/Level.h"

BEGIN(Client)
class CLevel_Client abstract:
	public CLevel
{
public:
	CLevel_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Client() = default;
	virtual void				Free() override { __super::Free(); }
	virtual	void				NowLevel(_uint _CurLevel) override;

protected:
	HRESULT Loading(const LEVEL& _eLevel);

};

END