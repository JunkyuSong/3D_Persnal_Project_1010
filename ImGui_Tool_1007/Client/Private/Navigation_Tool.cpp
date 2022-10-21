#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"

#include "stdafx.h"
#include "Navigation_Tool.h"
#include "GameInstance.h"
#include "TerrainMgr.h"
#include "ImGuiMgr.h"

IMPLEMENT_SINGLETON(CNavigation_Tool)

CNavigation_Tool::CNavigation_Tool()
{
	for (auto& _pPoint : m_pPoint)
		_pPoint = nullptr;

	ZeroMemory(m_szName,260);
	strcpy_s(m_szName, "NAME");
}

void CNavigation_Tool::Tick()
{
	AUTOINSTANCE(CGameInstance, _Instance);
	//�׺� �߰� : �̸� ����
	ImGui::InputText("Name", m_szName, 260);
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
	//���߰� ��� ����
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
		//���� �߰� �սä����Ź̤�
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
	//����, �ε�
	if (ImGui::Button("Save"))
	{
		_tchar* _szTemp = CImGui::Get_Instance()->ConvertCtoWC(m_szName);
		Save(_szTemp);
		Safe_Delete_Array(_szTemp);
	}
	if (ImGui::Button("Load"))
	{
		m_bLoad = true;
	}
	if (m_bLoad)
	{
		ImGui::InputText("Load", m_szName, 260);
		if (ImGui::Button("Enter") || _Instance->KeyDown(DIK_RETURN))
		{
			m_bLoad = false; _tchar* _szTemp = CImGui::Get_Instance()->ConvertCtoWC(m_szName);
			Load(_szTemp);
			Safe_Delete_Array(_szTemp);
			
		}
	}
}

void CNavigation_Tool::CellTick()
{
	//�� ��� �޾ƿ���, �� ����Ʈ ������ ���ϰ�, �Ӽ� ���������ϰ�, �ε��� Ȯ�ΰ����ϰ�
	// ���� ���õ� ���� �� �ٸ��� �߰�, �� ���� �����ϰ�.
	// �̰� ���� ������ �ϰ�..
}

void CNavigation_Tool::Add_Strip(CPointInCell* _pPoint)
{
	//���� �� 2���� �ִٸ�
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

void CNavigation_Tool::Save(_tchar* _szName)
{
	_tchar szFullPath[MAX_PATH] = TEXT(""); //���⿡ ���� ����
	char szFileName[MAX_PATH] = ""; //���� �̸�
	char szExt[MAX_PATH] = ""; //Ȯ����

	//strPath�� ���� ��� ���� ���� �̸��̶� Ȯ���� ����
	// ex) abc / .png

	lstrcpy(szFullPath, TEXT("../Bin/Data/"));
	//���� ��� �ְ�
	lstrcat(szFullPath, _szName);
	//���� �̸� �ְ�
	lstrcat(szFullPath, TEXT(".dat"));
	//��� �ְ�
	HANDLE		hFile = CreateFile(szFullPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	
	if (0 == hFile)
		return;
	_ulong		dwByte = 0;
	_float3		vPoints[3];
	vector<CCell*> _CellList = *(m_pNavi->Get_CellsPtr());

	for (auto& _pCell : _CellList)
	{
		ZeroMemory(vPoints, sizeof(_float3) * 3);
		vPoints[CCell::POINT_A] = _pCell->Get_Point(CCell::POINT_A);
		vPoints[CCell::POINT_B] = _pCell->Get_Point(CCell::POINT_B);
		vPoints[CCell::POINT_C] = _pCell->Get_Point(CCell::POINT_C);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
	}
	
	CloseHandle(hFile);

	return;
}

void CNavigation_Tool::Load(_tchar* _szName)
{
	_tchar szFullPath[MAX_PATH] = TEXT(""); //���⿡ ���� ����

	lstrcpy(szFullPath, TEXT("../Bin/Data/"));
	//���� ��� �ְ�
	lstrcat(szFullPath, _szName);
	//���� �̸� �ְ�
	lstrcat(szFullPath, TEXT(".dat"));
	//��� �ְ�
	

	return;
}

void CNavigation_Tool::Free()
{
	Safe_Release(m_pNavi);
	for (auto& _pPoint : m_pPoint)
		Safe_Release(_pPoint);
}
