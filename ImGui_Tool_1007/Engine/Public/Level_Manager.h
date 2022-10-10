#pragma once

#include "Base.h"

/* 현재 보여줘야할 레벨의 퐁니터를 가지고 있는다. */
/* 레벨의 틱, 렌더를 무한호출한다. */
/* 레[벨교체시에 기존레벨 삭제,. 새로운로 교체한다. */
//CLevel_Manager

BEGIN(Engine)

class CLevel;

class CLevel_Manager final :
	public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

	// 레벨 세팅 및 게임 루프
public:
	HRESULT					Check_Level();
	HRESULT					Loading_Level(const _uint & _iLoadingLevel, CLevel * _pLoadingLevel);
	HRESULT					Reserve_Level(const _uint& _iReserveLevel);

	HRESULT					Open_Level(const _uint& _iLevelIndex, CLevel* _pNewLevel);
	void					Tick(const _float& _fTimeDelta);
	HRESULT					Render();

	const _bool&			Get_bLoading() { return m_bLoading; }
	const _uint&			Get_LoadingLv() { return m_iLoadingLevel; }

	//생성, 소멸
public:
	virtual void			Free() override;

private:
	CLevel*					m_pCurrentLevel;
	CLevel*					m_pLoadingLevel;
	_uint					m_iCurrentLevel;
	_uint					m_iReserveLevel;

	_uint					m_iLoadingLevel;

	_bool					m_bLoading;
};

END