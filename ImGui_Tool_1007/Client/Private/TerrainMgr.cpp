#include "stdafx.h"
#include "..\Public\TerrainMgr.h"

IMPLEMENT_SINGLETON(CTerrainMgr)

CTerrainMgr::CTerrainMgr()
{
	ZeroMemory(m_Terrains, sizeof(CTerrain*) * LEVEL_END);
}

void CTerrainMgr::Free()
{
	for (auto& iter : m_Terrains)
		Safe_Release(iter);
}
