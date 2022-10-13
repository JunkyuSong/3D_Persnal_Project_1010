#include "..\Public\Status.h"

CStatus::CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_fHp(0)
	, m_fAttack(0)
{
}

CStatus::CStatus(const CStatus & _rhs)
	: CComponent(_rhs)
	,m_fHp(_rhs.m_fHp)
	,m_fAttack(_rhs.m_fAttack)
{
}

CStatus * CStatus::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStatus*		pInstance = new CStatus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCane"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CStatus::Clone(void * _pArg)
{
	CStatus*		pInstance = new CStatus(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : Status"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatus::Free()
{
	__super::Free();
}

HRESULT CStatus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStatus::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		STATUS _tStatus = *static_cast<STATUS*>(pArg);
		m_fHp = _tStatus.fHp;
		m_fAttack = _tStatus.fAttack;
		m_fMaxHp = _tStatus.fMaxHp;
	}
	else
	{
		m_fHp = 100.f;
		m_fAttack = 5.f;
		m_fMaxHp = 100.f;
	}
	return S_OK;
}
