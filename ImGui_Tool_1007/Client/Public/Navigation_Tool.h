#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Navigation.h"
#include "PointInCell.h"
#include "Stage_01.h"
BEGIN(Client)
class CNavigation_Tool final:
	public CBase
{
	DECLARE_SINGLETON(CNavigation_Tool)

private:
	enum NAVIMODE	{ NAVI_POINT, NAVI_CELL, NAVI_END};
	enum ADDMODE	{ MODE_STRIP , MODE_LIST, MODE_END};
public:
	CNavigation_Tool();
	virtual ~CNavigation_Tool() =default;

public:
	void Tick();
	void PointTick();
	void CellTick();

	void Add_Strip(CPointInCell* _pPoint);
	void Add_List(CPointInCell* _pPoint);

	void Save(_tchar* _szName);
	void Load(_tchar* _szName);

	virtual void Free();


private:
	char			m_szName[260];
	_bool			m_bLoad = false;
	NAVIMODE		m_eNavi = NAVI_POINT;
	ADDMODE			m_eMode = MODE_STRIP;
	CNavigation*	m_pNavi = nullptr;
	_float3			m_vPoint[CCell::POINT_END];
	CPointInCell*	m_pPoint[CCell::POINT_END];

	_float			m_PointY = 0.f;
	CStage_01*	m_pPickModel = nullptr;
	
};

END