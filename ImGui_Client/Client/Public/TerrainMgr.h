#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"
#include "Terrain.h"

BEGIN(Client)

class CTerrainMgr :
	public CBase
{
	DECLARE_SINGLETON(CTerrainMgr)
private:
	CTerrainMgr();
	virtual ~CTerrainMgr() = default;

public:
	void		Add_Terrain(const LEVEL& _eLev, CTerrain* _pTerrain) 
	{
		Safe_Release(m_Terrains[_eLev]);
		m_Terrains[_eLev] = _pTerrain;
		Safe_AddRef(m_Terrains[_eLev]);
	}
	CTerrain*	Get_Terrain(const LEVEL& _eLev) { return m_Terrains[_eLev]; }
	virtual void Free() override;

private:
	CTerrain*	m_Terrains[LEVEL_END];
};

END