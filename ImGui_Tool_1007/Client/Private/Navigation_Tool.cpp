#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"

#include "stdafx.h"
#include "Navigation_Tool.h"
#include "GameInstance.h"
#include "TerrainMgr.h"

IMPLEMENT_SINGLETON(CNavigation_Tool)

CNavigation_Tool::CNavigation_Tool()
{
	for (auto& _pPoint : m_pPoint)
		_pPoint = nullptr;

	ZeroMemory(m_szName,260);
}

void CNavigation_Tool::Tick()
{

	AUTOINSTANCE(CGameInstance, _Instance);
	//네비 추가 : 이름 적고
	//ImGui::InputText("", m_szName, 260);
	if (ImGui::Button("Create_Navi"))
	{
		if (m_pNavi == nullptr)
		{			
			m_pNavi = static_cast<CNavigation*>(_Instance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Ampty")));
			CTerrainMgr::Get_Instance()->Get_Terrain(g_eCurLevel)->Set_Navi(m_pNavi);
		}		
	}
	
	if (m_pNavi)
	{
		if (ImGui::RadioButton("NAVI_POINT", m_eNavi == NAVI_POINT))
		{
			m_eNavi = NAVI_POINT;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("NAVI_CELL", m_eNavi == NAVI_CELL))
		{
			m_eNavi = NAVI_CELL;
		}

		switch (m_eNavi)
		{
		case Client::CNavigation_Tool::NAVI_POINT:
			PointTick();
			break;
		case Client::CNavigation_Tool::NAVI_CELL:
			CellTick();
			break;
		}
	}
}

void CNavigation_Tool::PointTick()
{
	AUTOINSTANCE(CGameInstance, _Instance);
	//셀추가 모드 선택
	if (ImGui::RadioButton("MODE_STRIP", m_eMode == MODE_STRIP))
	{
		m_eMode = MODE_STRIP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("MODE_LIST", m_eMode == MODE_LIST))
	{ 
		m_eMode = MODE_LIST;
	}

	if (ImGui::Button("Point_Clear"))
	{
		for (auto& pPoint : m_pPoint)
		{
			pPoint = nullptr;
		}
	}
	
	if (ImGui::Button("Delete_Point.A"))
	{
		m_pNavi->DeletePoint(m_pPoint[CCell::POINT_A]);
		m_pPoint[CCell::POINT_A] = nullptr;
	}

	for (int i = 0; i < CCell::POINT_END - 1; ++i)
	{
		if (m_pPoint[i])
		{
			//ImGui::Text("Point.A");
			memcpy(&(m_vPoint[i]), &(m_pPoint[i]->Get_Point()), sizeof(_float3));
			ImGui::PushItemWidth(100.f);
			ImGui::InputFloat("x", &(m_vPoint[i].x));
			ImGui::SameLine();
			ImGui::InputFloat("y", &(m_vPoint[i].y));
			ImGui::SameLine();
			ImGui::InputFloat("z", &(m_vPoint[i].z));
			ImGui::PopItemWidth();
			m_pPoint[i]->RepairPoint(m_vPoint[i]);
		}
	}
	
	if (_Instance->MouseDown(DIMK::DIMK_LBUTTON) && _Instance->KeyPressing(DIK_LSHIFT))
	{
		//점을 추가 합시ㅏㄷ거미ㅏ
		CPointInCell* _pPoint = nullptr;
		_pPoint = m_pNavi->PickingPoint();
		if (_pPoint == nullptr)
		{
			_float3 _vPos;
			if (false == CTerrainMgr::Get_Instance()->Get_Terrain(g_eCurLevel)->Picking(_vPos))
			{
				return;
			}
			_pPoint = CPointInCell::Create(_vPos);
			m_pNavi->MakePoint(_pPoint);
		}

		switch (m_eMode)
		{
		case MODE_STRIP:
			Add_Strip(_pPoint);
			break;
		case MODE_LIST:
			Add_List(_pPoint);
			break;
		}
	}
	if (ImGui::Button("Set_Naver"))
	{
		m_pNavi->Ready_Neighbor();
	}
	//저장, 로드
	if (ImGui::Button("Save"))
	{
		
	}
	if (ImGui::Button("Load"))
	{
		
	}
}

void CNavigation_Tool::CellTick()
{
	//셀 목록 받아오고, 셀 포인트 수정은 못하고, 속성 수정가능하고, 인덱스 확인가능하고
	// 현재 선택된 셀은 색 다르게 뜨고, 셀 삭제 가능하고.
	// 이건 내일 오전에 하고..
}

void CNavigation_Tool::Add_Strip(CPointInCell* _pPoint)
{
	//만약 앞 2개가 있다면
	if (m_pPoint[CCell::POINT_A] == nullptr)
	{
		m_pPoint[CCell::POINT_A] = _pPoint;
		return;
	}

	if (m_pPoint[CCell::POINT_B] == nullptr)
	{
		m_pPoint[CCell::POINT_B] = _pPoint;
		return;
	}

	m_pPoint[CCell::POINT_C] = _pPoint;

	m_pNavi->MakeCell(m_pPoint[CCell::POINT_A], m_pPoint[CCell::POINT_B], m_pPoint[CCell::POINT_C]);
	m_pPoint[CCell::POINT_A] = m_pPoint[CCell::POINT_B];
	m_pPoint[CCell::POINT_B] = m_pPoint[CCell::POINT_C];
	m_pPoint[CCell::POINT_C] = nullptr;
}

void CNavigation_Tool::Add_List(CPointInCell* _pPoint)
{
	if (m_pPoint[CCell::POINT_A] == nullptr)
	{
		m_pPoint[CCell::POINT_A] = _pPoint;
		return;
	}

	if (m_pPoint[CCell::POINT_B] == nullptr)
	{
		m_pPoint[CCell::POINT_B] = _pPoint;
		return;
	}
	m_pPoint[CCell::POINT_C] = _pPoint;

	m_pNavi->MakeCell(m_pPoint[CCell::POINT_A], m_pPoint[CCell::POINT_B], m_pPoint[CCell::POINT_C]);
	m_pPoint[CCell::POINT_A] = nullptr;
	m_pPoint[CCell::POINT_B] = nullptr;
	m_pPoint[CCell::POINT_C] = nullptr;
}

void CNavigation_Tool::Save()
{

}

void CNavigation_Tool::Load()
{
}

void CNavigation_Tool::Free()
{
	Safe_Release(m_pNavi);
	for (auto& _pPoint : m_pPoint)
		Safe_Release(_pPoint);
}
