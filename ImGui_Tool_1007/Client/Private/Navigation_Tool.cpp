#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"

#include "stdafx.h"
#include "Navigation_Tool.h"
#include "GameInstance.h"
#include "TerrainMgr.h"
#include "ImGuiMgr.h"

#include "Layer.h"

#include "Obj_NonAnim.h"

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
	//네비 추가 : 이름 적고
	if (ImGui::Button("Get_Model"))
	{
		m_pPickModel = static_cast<CStage_01*>(_Instance->Get_Layer(g_eCurLevel, TEXT("Layer_Stage"))->Get_ObjFromLayer(0));
	}
	if (ImGui::Button("Get_Layer"))
	{
		m_pLayer = static_cast<CLayer*>(_Instance->Get_Layer(g_eCurLevel, TEXT("Layer_Map")));
	}
	if (ImGui::Button("Load_Navi"))
	{
		Safe_Release(m_pNavi);
		m_pNavi = static_cast<CNavigation*>(CTerrainMgr::Get_Instance()->Get_Terrain(g_eCurLevel)->Get_ComponentPtr(TEXT("Com_Navigation")));
		Safe_AddRef(m_pNavi);
	}
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

	ImGui::InputFloat("Point.Y",&m_PointY);

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
	
	if (_Instance->MouseDown(DIMK::DIMK_LBUTTON) && _Instance->KeyPressing(DIK_RSHIFT))
	{
		//점을 추가 합시ㅏㄷ거미ㅏ
		CPointInCell* _pPoint = nullptr;
		_pPoint = m_pNavi->PickingPoint();
		if (_pPoint == nullptr)
		{
			_float3 _vPos;
			_float	_fDIs = 1000.f;
			//if (false == CTerrainMgr::Get_Instance()->Get_Terrain(g_eCurLevel)->Picking(_vPos))
			/*if(m_pPickModel == nullptr || false == m_pPickModel->Picking(_vPos))
			{
				return;
			}*/
			
			for (auto& _Obj : *(m_pLayer->Get_ListFromLayer()))
			{
				_float3 _vCurPos;
				if (static_cast<CObj_NonAnim*>(_Obj)->Picking(_vCurPos))
				{
					_float	_fCurDis = XMVector3Length(XMLoadFloat4(&CGameInstance::Get_Instance()->Get_CamPosition()) - XMVectorSetW(XMLoadFloat3(&_vCurPos), 1.f)).m128_f32[0];
					if (_fDIs > _fCurDis)
					{
						_vPos = _vCurPos;
						_fDIs = _fCurDis;
					}
				}				
			}
			if (_fDIs == 1000.f)
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
	ImGui::InputFloat("OneMore_PlusY", &m_fPlusY);
	if (ImGui::Button("OneMore_Load"))
	{
		m_bMoreLoad = true;
	}
	if (m_bMoreLoad)
	{
		ImGui::InputText("Load", m_szName, 260);
		if (ImGui::Button("Enter") || _Instance->KeyDown(DIK_RETURN))
		{
			m_bLoad = false; _tchar* _szTemp = CImGui::Get_Instance()->ConvertCtoWC(m_szName);
			OneMoreLoad(_szTemp);
			Safe_Delete_Array(_szTemp);

		}
	}
}

void CNavigation_Tool::CellTick()
{
	//셀 목록 받아오고, 셀 포인트 수정은 못하고, 속성 수정가능하고, 인덱스 확인가능하고
	// 현재 선택된 셀은 색 다르게 뜨고, 셀 삭제 가능하고.
	// 이건 내일 오전에 하고..

	AUTOINSTANCE(CGameInstance, _pInstance);
	CCell* _Cell(nullptr);
	if (_pInstance->MouseDown(DIMK::DIMK_LBUTTON) && _pInstance->KeyPressing(DIK_RSHIFT))
	{
		_vector _vPos{ 0.f,0.f,0.f,1.f };
		_Cell = m_pNavi->PickingCell(_vPos);
		if (_Cell != nullptr)
		{
			XMStoreFloat3(&m_vPickingPos, _vPos);
			Safe_Release(m_pCell);
			m_pCell = _Cell;
			Safe_AddRef(m_pCell);
		}
	}
	if (m_pCell != nullptr)
	{
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		ImGui::Text("Index : %d", m_pCell->Get_Index());

		ImGui::PushItemWidth(100);
		ImGui::Text("PickingPos_X : %.3f", m_vPickingPos.x);
		ImGui::SameLine();
		ImGui::Text("PickingPos_Y : %.3f", m_vPickingPos.y);
		ImGui::SameLine();
		ImGui::Text("PickingPos_Z : %.3f", m_vPickingPos.z);

		ImGui::Text("POINT.A_X : %.3f", m_pCell->Get_Point(CCell::POINT_A).x);
		ImGui::SameLine();
		ImGui::Text("POINT.A_Y : %.3f", m_pCell->Get_Point(CCell::POINT_A).y);
		ImGui::SameLine();
		ImGui::Text("POINT.A_Z : %.3f", m_pCell->Get_Point(CCell::POINT_A).z);

		ImGui::Text("POINT.B_X : %.3f", m_pCell->Get_Point(CCell::POINT_B).x);
		ImGui::SameLine();
		ImGui::Text("POINT.B_Y : %.3f", m_pCell->Get_Point(CCell::POINT_B).y);
		ImGui::SameLine();
		ImGui::Text("POINT.B_Z : %.3f", m_pCell->Get_Point(CCell::POINT_B).z);

		ImGui::Text("POINT.C_X : %.3f", m_pCell->Get_Point(CCell::POINT_C).x);
		ImGui::SameLine();
		ImGui::Text("POINT.C_Y : %.3f", m_pCell->Get_Point(CCell::POINT_C).y);
		ImGui::SameLine();
		ImGui::Text("POINT.C_Z : %.3f", m_pCell->Get_Point(CCell::POINT_C).z);

		ImGui::PopItemWidth();
	}
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

void CNavigation_Tool::Save(_tchar* _szName)
{
	_tchar szFullPath[MAX_PATH] = TEXT(""); //여기에 넣을 예정
	char szFileName[MAX_PATH] = ""; //파일 이름
	char szExt[MAX_PATH] = ""; //확장자

	//strPath에 파일 경로 떼고 파일 이름이랑 확장자 뺀다
	// ex) abc / .png

	lstrcpy(szFullPath, TEXT("../Bin/Data/"));
	//파일 경로 넣고
	lstrcat(szFullPath, _szName);
	//파일 이름 넣고
	lstrcat(szFullPath, TEXT(".dat"));
	//경로 넣고
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
	Safe_Release(m_pNavi);
	//로드하면 일단 셀들 로드하고 그거 싹 돌면서 포인트 생성하고 있으면 거기에 셀에 넣어주고
	// 좀 빡세겠는데
	_tchar szFullPath[MAX_PATH] = TEXT(""); //여기에 넣을 예정

	lstrcpy(szFullPath, TEXT("../Bin/Data/"));
	//파일 경로 넣고
	lstrcat(szFullPath, _szName);
	//파일 이름 넣고
	lstrcat(szFullPath, TEXT(".dat"));
	//경로 넣고
	AUTOINSTANCE(CGameInstance, _Instance);
	//Safe_Release(m_pNavi);
	

	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(szFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return;

	m_pNavi = static_cast<CNavigation*>(_Instance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Ampty")));
	CTerrainMgr::Get_Instance()->Get_Terrain(g_eCurLevel)->Set_Navi(m_pNavi);

	while (true)
	{
		_float3			vPoints[3];
		CPointInCell*	pPoints[3];
		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		for (_uint i = 0; i < 3; ++i)
		{
			pPoints[i] = m_pNavi->FindPoint(vPoints[i]);
			if (pPoints[i] == nullptr)
			{
				pPoints[i] = CPointInCell::Create(vPoints[i]);
				m_pNavi->MakePoint(pPoints[i]);
			}
		}
		
		m_pNavi->MakeCell(pPoints[0], pPoints[1], pPoints[2]);
	}
	m_pNavi->Ready_Neighbor();
	CloseHandle(hFile);
}

void CNavigation_Tool::OneMoreLoad(_tchar * _szName)
{
	//로드하면 일단 셀들 로드하고 그거 싹 돌면서 포인트 생성하고 있으면 거기에 셀에 넣어주고
	// 좀 빡세겠는데
	_tchar szFullPath[MAX_PATH] = TEXT(""); //여기에 넣을 예정

	lstrcpy(szFullPath, TEXT("../Bin/Data/"));
	//파일 경로 넣고
	lstrcat(szFullPath, _szName);
	//파일 이름 넣고
	lstrcat(szFullPath, TEXT(".dat"));
	//경로 넣고
	AUTOINSTANCE(CGameInstance, _Instance);
	//Safe_Release(m_pNavi);


	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(szFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return;

	while (true)
	{
		_float3			vPoints[3];
		CPointInCell*	pPoints[3];
		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		vPoints[0].y += m_fPlusY;
		vPoints[1].y += m_fPlusY;
		vPoints[2].y += m_fPlusY;
		if (0 == dwByte)
			break;

		for (_uint i = 0; i < 3; ++i)
		{
			pPoints[i] = m_pNavi->FindPoint(vPoints[i]);
			if (pPoints[i] == nullptr)
			{
				pPoints[i] = CPointInCell::Create(vPoints[i]);
				m_pNavi->MakePoint(pPoints[i]);
			}
		}

		m_pNavi->MakeCell(pPoints[0], pPoints[1], pPoints[2]);
	}
	m_pNavi->Ready_Neighbor();
	CloseHandle(hFile);
}

void CNavigation_Tool::Free()
{
	Safe_Release(m_pNavi);
	for (auto& _pPoint : m_pPoint)
		Safe_Release(_pPoint);
	Safe_Release(m_pCell);
}
