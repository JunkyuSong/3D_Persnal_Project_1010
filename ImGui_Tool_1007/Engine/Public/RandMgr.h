#pragma once
#include "Base.h"

BEGIN(Engine)
class CRandMgr final:
	public CBase
{
	DECLARE_SINGLETON(CRandMgr)
private:
	CRandMgr() = default;
	virtual ~CRandMgr() = default;

public:
	const _float& Rand_Float(const _float& _fMin, const _float& _fMax);
	const _int& Rand_Int(const _int& _iMin, const _int& _iMax);

	virtual void Free() override;
};

END