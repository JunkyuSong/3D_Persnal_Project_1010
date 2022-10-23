#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	 _float Get_TimeDelta(void) { return m_fTimeDelta * m_fTimeSpeed; }

	 void	Set_TimeSpeed(_float _fTimeSpeed) { m_fTimeSpeed = _fTimeSpeed; }

	 _float	Get_TimeSpeed() { return m_fTimeSpeed; }

public:
	HRESULT Initialize();
	void Update(void);

private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;

	_float				m_fTimeDelta;
	_float				m_fTimeSpeed;

public:
	static CTimer* Create();
	virtual void Free() override;
};

END