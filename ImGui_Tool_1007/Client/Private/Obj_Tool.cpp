#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "stdafx.h"
#include "Obj_Tool.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"
#include "Anim_Tool.h"

#include "TerrainMgr.h"
#include "ReleaseMgr.h"
#include "Navigation_Tool.h"

IMPLEMENT_SINGLETON(CObj_Tool)

CObj_Tool::CObj_Tool()
	: m_bTool_Obj_Pick(false)
	, m_bTool_Obj_Add(false)
	, m_pLayer(nullptr)
	, m_pPick(nullptr)
	, m_iPickIndex(NULL)
	, m_bModels(false)
	, m_iSelectModel(0)
	, m_vPos(0.f, 0.f, 0.f)
	, m_vScale(1.f, 1.f, 1.f)
	, m_vAngle(0.f, 0.f, 0.f)
	, m_iSelectLayer(0)
	, m_bAddType(TYPE_ANIM)
{
	ZeroMemory(&m_tObj_Desc, sizeof(CObj_Plus::OBJ_DESC));
	ZeroMemory(&m_szModelTag, 256);
	ZeroMemory(&m_szLayer, 256);
	
	m_pLevels.push_back("LEVEL_STAGE_LOBBY");
	m_pLevels.push_back("LEVEL_STAGE_01");
	m_pLevels.push_back("LEVEL_STAGE_02_1");
	m_pLevels.push_back("LEVEL_STAGE_02");
	m_pLevels.push_back("LEVEL_STAGE_LAST");
}


CObj_Tool::~CObj_Tool()
{
	/*if (m_tObj_Desc.szModelTag)
	{
	Safe_Delete_Array(m_tObj_Desc.szModelTag);
	}*/
	for (auto& iter : m_pLayers)
	{
		Safe_Delete_Array(iter);
	}
	for (auto& iter : m_pModels)
	{
		Safe_Delete_Array(iter);
	}

}

void CObj_Tool::Free()
{
	CAnim_Tool::Destroy_Instance();
	for (auto& iter : m_mapToolProto)
	{
		//Safe_Delete_Array(iter.first);
		Safe_Release(iter.second);
	}
}

void CObj_Tool::Save_Map()
{
	//레벨 저장? 필요없는거같은데 어차피 해당 레벨에서 로드해올거임
	//레이어 저장? 필요한가 여기서 찍는 맵은 전부 맵오브젝트로 들어갈건데 -> 인스턴싱 될 확률이 큰데요... 필요할지도...? 툴에서 로드할땐..? 그때도 그냥 레이어맵에
	//결국 레이어는 저장할 필요 없고, 필요한 데이터는 행렬값이랑 모델 태그값 2개다

	_tchar szFullPath[MAX_PATH] = TEXT("../Bin/Map/"); //여기에 넣을 예정
	_tchar szFileName[MAX_PATH] = TEXT(""); //파일 이름

	switch (g_eCurLevel)
	{
	case Client::LEVEL_STATIC:
		break;
	case Client::LEVEL_LOADING:
		return;
	case Client::LEVEL_LOGO:
		return;
	case Client::LEVEL_GAMEPLAY:
		lstrcpy(szFileName, TEXT("LEVEL_GAMEPLAY"));
		break;
	case Client::LEVEL_STAGE_02:
		lstrcpy(szFileName, TEXT("LEVEL_STAGE_02"));
		break;
	case Client::LEVEL_STAGE_LAST:
		lstrcpy(szFileName, TEXT("LEVEL_STAGE_LAST"));
		break;
	case Client::LEVEL_STAGE_LOBBY:
		lstrcpy(szFileName, TEXT("LEVEL_STAGE_LOBBY"));
		break;
	case Client::LEVEL_STAGE_02_1:
		lstrcpy(szFileName, TEXT("LEVEL_STAGE_02_1"));
		break;
	default:
		return;
	}
	AUTOINSTANCE(CGameInstance, pGameInstance);


	//_splitpath_s(_tModel.Name, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
	//strPath에 파일 경로 떼고 파일 이름이랑 확장자 뺀다
	// ex) abc / .png

	lstrcat(szFullPath, szFileName);
	//파일 이름 넣고
	lstrcat(szFullPath, TEXT(".dat"));
	//경로 넣고


	// 2. 파일 쓰기
	//레벨별로 리스트를 얻어온다


	HANDLE		hFile = CreateFile(szFullPath,			// 파일 경로와 이름 명시
		GENERIC_WRITE,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		CREATE_ALWAYS,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	DWORD		dwByte = 0;

	list<CGameObject*> list = *(m_pLayer->Get_ListFromLayer());
	CObj_Plus::OBJ_DESC	_tInfo;
	ZeroMemory(&_tInfo, sizeof(CObj_Plus::OBJ_DESC));
	for (auto& _Obj : list)
	{
		ZeroMemory(&_tInfo, sizeof(CObj_Plus::OBJ_DESC));
		_tInfo = static_cast<CObj_Plus*>(_Obj)->Get_Info();
		WriteFile(hFile, &_tInfo, sizeof(CObj_Plus::OBJ_DESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);

}

void CObj_Tool::Load_Map()
{
	//레벨 저장? 필요없는거같은데 어차피 해당 레벨에서 로드해올거임
	//레이어 저장? 필요한가 여기서 찍는 맵은 전부 맵오브젝트로 들어갈건데 -> 인스턴싱 될 확률이 큰데요... 필요할지도...? 툴에서 로드할땐..? 그때도 그냥 레이어맵에
	//결국 레이어는 저장할 필요 없고, 필요한 데이터는 행렬값이랑 모델 태그값 2개다

	AUTOINSTANCE(CGameInstance, _Instance);

	_tchar szFullPath[MAX_PATH] = TEXT("../Bin/Map/"); //여기에 넣을 예정
	_tchar szFileName[MAX_PATH] = TEXT(""); //파일 이름

	switch (g_eCurLevel)
	{
	case Client::LEVEL_STATIC:
		break;
	case Client::LEVEL_LOADING:
		return;
	case Client::LEVEL_LOGO:
		return;
	case Client::LEVEL_GAMEPLAY:
		lstrcpy(szFileName, TEXT("LEVEL_GAMEPLAY"));
		break;
	case Client::LEVEL_STAGE_02:
		lstrcpy(szFileName, TEXT("LEVEL_STAGE_02"));
		break;
	case Client::LEVEL_STAGE_LAST:
		lstrcpy(szFileName, TEXT("LEVEL_STAGE_LAST"));
		break;
	case Client::LEVEL_STAGE_LOBBY:
		lstrcpy(szFileName, TEXT("LEVEL_STAGE_LOBBY"));
		break;
	case Client::LEVEL_END:
		return;
	default:
		return;
	}

	_tchar*		_szFileName = nullptr;


	lstrcat(szFullPath, szFileName);
	//파일 이름 넣고
	lstrcat(szFullPath, TEXT(".dat"));
	//경로 넣고



	HANDLE		hFile = CreateFile(szFullPath,
		GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	DWORD		dwByte = 0;

	CObj_Plus::OBJ_DESC	_tInfo;
	ZeroMemory(&_tInfo, sizeof(CObj_Plus::OBJ_DESC));
	while (true)
	{
		ZeroMemory(&_tInfo, sizeof(CObj_Plus::OBJ_DESC));
		ReadFile(hFile, &_tInfo, sizeof(CObj_Plus::OBJ_DESC), &dwByte, nullptr);
		if (dwByte == 0)
			break;

		if (FAILED(_Instance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Obj_NonAnim"), g_eCurLevel, TEXT("Layer_Map"), &_tInfo)))
		{
			MSG_BOX(TEXT("FAILED LOAD MAP"));
			CloseHandle(hFile);
			return;
		}
	}

	CloseHandle(hFile);

}


HRESULT CObj_Tool::Tool_Obj()
{
	if (ImGui::BeginCombo("Level", m_pLevels[m_iSelectLevel], 1))
	{
		for (int i = 0; i < m_pLevels.size(); ++i)
		{
			const bool isSelected = (m_iSelectLevel == i);
			if (ImGui::Selectable(m_pLevels[i], isSelected))
			{
				m_iSelectLevel = i;
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Reseve Level"))
	{
		ReserveLevel();
	}

	if (ImGui::Button("Load Layers"))
	{
		Load_Layers();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Model"))
	{
		Load_Model();
	}

	ImGui::Separator();

	//ImGui::SameLine();

	ImGui::InputText("Layer_Name", m_szLayer, 256, ImGuiInputTextFlags_CharsNoBlank);
	ImGui::SameLine();
	if (ImGui::Button("Add Layer"))
	{
		//ImGui::Begin("Add Layer");
		//
		//ImGui::End();
		_tchar* _szTemp = CImGui::Get_Instance()->ConvertCtoWC(m_szLayer);
		CGameInstance::Get_Instance()->Add_Layer(g_eCurLevel, _szTemp);
		//동적해제 골때리네
		Load_Layers();
		//Safe_Delete_Array(_szTemp);
	}


	if (m_pLayers.size() == 0)
	{
	}
	else if (ImGui::BeginCombo("Layer", m_pLayers[m_iSelectLayer], 1))
	{
		for (int i = 0; i < m_pLayers.size(); ++i)
		{
			const bool isSelected = (m_iSelectLayer == i);
			if (ImGui::Selectable(m_pLayers[i], isSelected))
			{
				m_iSelectLayer = i;
			}
		}
		ImGui::EndCombo();
	}



	if (ImGui::Button("Layer Pick"))
	{
		_tchar* _pLayerTag = CImGui::Get_Instance()->ConvertCtoWC(m_pLayers[m_iSelectLayer]);
		m_pLayer = CGameInstance::Get_Instance()->Get_Layer(g_eCurLevel, _pLayerTag);
		Safe_Delete_Array(_pLayerTag);
	}

	ImGui::Separator();

	if (m_pLayer != nullptr)
	{

		if (ImGui::Button("Get_Model"))
		{
			m_pPickModel = static_cast<CStage_Test*>(CGameInstance::Get_Instance()->Get_Layer(g_eCurLevel, TEXT("Layer_Test"))->Get_ObjFromLayer(0));
		}

		if (ImGui::Button("Add Obj"))
		{
			if (m_pPick != nullptr)
			{
				static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_NONPICK);

			}
			XMStoreFloat3(&m_vScale, XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
			XMStoreFloat3(&m_vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f));
			XMStoreFloat3(&m_vAngle, XMVectorSet(0.f, 0.f, 0.f, 0.f));
			m_pPick = nullptr;
			m_bTool_Obj_Add = true;
			m_bTool_Obj_Pick = false;
		}

		ImGui::SliderInt("Layer Index", &m_iPickIndex, 0, (*m_pLayer->Get_ListFromLayer()).size() - 1);
		if (ImGui::Button("Pick"))
		{
			if (m_pPick != nullptr)
			{
				static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_NONPICK);
			}
			list<CGameObject*>* _list = m_pLayer->Get_ListFromLayer();
			auto& iter = (*_list).begin();
			for (int i = 0; i < m_iPickIndex; ++iter, ++i);
			m_pPick = *iter;
			static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_PICK);

			m_pPick_Trans = static_cast<CTransform*>(m_pPick->Get_ComponentPtr(TEXT("Com_Transform")));
			m_bTool_Obj_Pick = true;
			m_bTool_Obj_Add = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			if (m_pPick != nullptr)
			{
				static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_NONPICK);
			}
			list<CGameObject*>* _list = m_pLayer->Get_ListFromLayer();
			auto& iter = (*_list).begin();
			for (int i = 0; i < m_iPickIndex; ++iter, ++i);
			Safe_Release<CGameObject*>(*iter);
			(*_list).erase(iter);
			iter = (*_list).begin();
			m_iPickIndex = 0;
			m_pPick = nullptr;
		}
	}

	if (m_bTool_Obj_Pick)
	{
		Tool_Obj_Pick();
	}
	if (m_bTool_Obj_Add)
	{
		Tool_Obj_Add();
	}
	ImGui::Separator();
	if (ImGui::Button("Save Map"))
	{
		Save_Map();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Map"))
	{
		Load_Map();
	}



	return S_OK;
}

void CObj_Tool::ReserveLevel()
{
	AUTOINSTANCE(CGameInstance, _Instance);
	switch (m_iSelectLevel)
	{
	case 0:
		_Instance->Reserve_Level(LEVEL_STAGE_LOBBY);
		return;
	case 1:
		_Instance->Reserve_Level(LEVEL_GAMEPLAY);
		return;
	case 2:
		_Instance->Reserve_Level(LEVEL_STAGE_02_1);
		return;
	case 3:
		_Instance->Reserve_Level(LEVEL_STAGE_02);
		return;
	case 4:
		_Instance->Reserve_Level(LEVEL_STAGE_LAST);
		return;
	}
}


HRESULT CObj_Tool::Tool_Obj_Pick()
{

	//ImGui::Begin("Tool_Obj_Pick", &m_bTool_Obj_Pick);

	ImGui::Begin("Tool_Obj_Pick_", &m_bTool_Obj_Pick);

	m_vScale = m_pPick_Trans->Get_Scale();
	XMStoreFloat3(&m_vPos, m_pPick_Trans->Get_State(CTransform::STATE_POSITION));
	m_vAngle = m_pPick_Trans->Get_Rotation();
	Key_Input();

	ImGui::InputFloat("Pos.x", &m_vPos.x);
	ImGui::InputFloat("Pos.y", &m_vPos.y);
	ImGui::InputFloat("Pos.z", &m_vPos.z);

	ImGui::InputFloat("Scale.x", &m_vScale.x);
	if (m_vScale.x <= 0.f) { m_vScale.x = 0.01f; }
	ImGui::InputFloat("Scale.y", &m_vScale.y);
	if (m_vScale.y <= 0.f) { m_vScale.y = 0.01f; }
	ImGui::InputFloat("Scale.z", &m_vScale.z);
	if (m_vScale.z <= 0.f) { m_vScale.z = 0.01f; }

	ImGui::InputFloat("Angle.x", &m_vAngle.x);
	ImGui::InputFloat("Angle.y", &m_vAngle.y);
	ImGui::InputFloat("Angle.z", &m_vAngle.z);

	m_pPick_Trans->Set_Scale(XMVectorSet(m_vScale.x, m_vScale.y, m_vScale.z, 0.f));

	m_pPick_Trans->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_vAngle.z));
	m_pPick_Trans->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.x));
	m_pPick_Trans->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.y));
	m_pPick_Trans->Set_Rotation(XMLoadFloat3(&m_vAngle));
	m_pPick_Trans->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.x, m_vPos.y, m_vPos.z, 1.f));

	if (m_bModels)
	{
		if (ImGui::BeginCombo("Obj", m_pModels[m_iSelectModel], 1))
		{
			for (int i = 0; i < m_pModels.size(); ++i)
			{
				const bool isSelected = (m_iSelectModel == i);
				if (ImGui::Selectable(m_pModels[i], isSelected))
				{
					m_iSelectModel = i;
				}
			}
			ImGui::EndCombo();
		}
	}

	if (ImGui::Button("Set_Info")) // 셋인포(?)
	{
		_matrix		Matrix = XMMatrixIdentity();
		Matrix = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z) *
			XMMatrixRotationZ(XMConvertToRadians(m_vAngle.z)) *
			XMMatrixRotationX(XMConvertToRadians(m_vAngle.x)) *
			XMMatrixRotationY(XMConvertToRadians(m_vAngle.y)) *

			XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

		XMStoreFloat4x4(&m_tObj_Desc.matWorld, Matrix);
		m_tObj_Desc.eLevel = g_eCurLevel;
		lstrcpy(m_tObj_Desc.szModelTag, CImGui::Get_Instance()->ConvertCtoWC(m_pModels[m_iSelectModel]));
		if (m_pPick)
			static_cast<CObj_Plus*>(m_pPick)->Set_Info(m_tObj_Desc);
	}

	if (ImGui::Button("AnimModel_Save"))
	{
		AnimModel_Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("NonAnimModel_Save"))
	{
		NonAnimModel_Save();
	}

	if (ImGui::RadioButton("Player", CAnim_Tool::Get_Instance()->Anim_Type() == CAnim_Tool::TYPE_PLAYER)) { CAnim_Tool::Get_Instance()->Anim_Type(CAnim_Tool::TYPE_PLAYER); }
	ImGui::SameLine();
	if (ImGui::RadioButton("NonPlayer", CAnim_Tool::Get_Instance()->Anim_Type() == CAnim_Tool::TYPE_NONPLAYER)) { CAnim_Tool::Get_Instance()->Anim_Type(CAnim_Tool::TYPE_NONPLAYER); }
	if (ImGui::Button("Anim_Tool"))
	{

		CAnim_Tool::Get_Instance()->Get_AnimList(m_pPick);
		m_bAnim = true;
	}

	if (m_bAnim)
	{
		CAnim_Tool::Get_Instance()->Tick();
	}

	ImGui::SameLine();
	if (ImGui::Button("Close"))
	{
		static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_NONPICK);
		m_bTool_Obj_Pick = false;
	}

	ImGui::End();

	return S_OK;
}

HRESULT CObj_Tool::Tool_Obj_Add()
{
	ImGui::Begin("Tool_Obj_Add", &m_bTool_Obj_Add);


	if (!m_bSetting)
	{
		if (ImGui::RadioButton("Anim", m_bAddType == TYPE_ANIM)) { m_bAddType = TYPE_ANIM; }
		ImGui::SameLine();
		if (ImGui::RadioButton("NonAnim", m_bAddType == TYPE_NONANIM)) { m_bAddType = TYPE_NONANIM; }

		if (m_bModels)
		{
			if (ImGui::BeginCombo("Obj", m_pModels[m_iSelectModel], 1))
			{
				for (int i = 0; i < m_pModels.size(); ++i)
				{
					const bool isSelected = (m_iSelectModel == i);
					if (ImGui::Selectable(m_pModels[i], isSelected))
					{
						m_iSelectModel = i;
					}
				}
				ImGui::EndCombo();
			}
		}
	}
	else
	{
		if (m_pPickModel == nullptr)
			CTerrainMgr::Get_Instance()->Get_Terrain(g_eCurLevel)->Picking(m_vPos);
		else
			m_pPickModel->Picking(m_vPos);
		
		//m_vScale = m_pPick_Trans->Get_Scale();
		//_float3 _vPos;
		//XMStoreFloat3(&_vPos, m_pPick_Trans->Get_State(CTransform::STATE_POSITION));
		m_vAngle = m_pPick_Trans->Get_Rotation();
		_vector _Scale, _Rot, _Pos;
		_matrix world = m_pPick_Trans->Get_WorldMatrix();
		XMMatrixDecompose(&_Scale, &_Rot, &_Pos, world);

		_float4 _vRot;
		XMStoreFloat3(&m_vScale, _Scale);
		XMStoreFloat4(&_vRot, _Rot);
		//XMStoreFloat3(&m_vPos, _Pos);

		/*_vRot.x = XMConvertToDegrees(_vRot.x);
		_vRot.y = XMConvertToDegrees(_vRot.y);
		_vRot.z = XMConvertToDegrees(_vRot.z);*/

		m_vAngle.x = XMConvertToDegrees(m_vAngle.x);
		m_vAngle.y = XMConvertToDegrees(m_vAngle.y);
		m_vAngle.z = XMConvertToDegrees(m_vAngle.z);

		//m_vPos.y = XMVectorGetY(_Pos);
		Key_Input();


		ImGui::InputFloat("Pos.x", &m_vPos.x);
		ImGui::InputFloat("Pos.y", &m_vPos.y);
		ImGui::InputFloat("Pos.z", &m_vPos.z);

		ImGui::InputFloat("Scale.x", &m_vScale.x);
		if (m_vScale.x <= 0.f) { m_vScale.x = 0.01f; }
		ImGui::InputFloat("Scale.y", &m_vScale.y);
		if (m_vScale.y <= 0.f) { m_vScale.y = 0.01f; }
		ImGui::InputFloat("Scale.z", &m_vScale.z);
		if (m_vScale.z <= 0.f) { m_vScale.z = 0.01f; }

		ImGui::InputFloat("Angle.x", &m_vAngle.x);
		ImGui::InputFloat("Angle.y", &m_vAngle.y);
		ImGui::InputFloat("Angle.z", &m_vAngle.z);

		m_vAngle.x = XMConvertToRadians(m_vAngle.x);
		m_vAngle.y = XMConvertToRadians(m_vAngle.y);
		m_vAngle.z = XMConvertToRadians(m_vAngle.z);

		/*world = XMMatrixAffineTransformation(XMLoadFloat3(&m_vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&_vRot), XMVectorSetW(XMLoadFloat3(&m_vPos), 1.f));
		_float4x4 _world4x4;
		XMStoreFloat4x4(&_world4x4, world);
		m_pPick_Trans->Set_WorldFloat4x4(_world4x4);*/
		m_pPick_Trans->Set_Scale(XMVectorSet(m_vScale.x, m_vScale.y, m_vScale.z, 0.f));

		//m_pPick_Trans->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_vAngle.z));
		m_pPick_Trans->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), (m_vAngle.x));
		m_pPick_Trans->Turn_Angle(XMVectorSet(0.f, 1.f, 0.f, 0.f), (m_vAngle.y));
		m_pPick_Trans->Turn_Angle(XMVectorSet(0.f, 0.f, 1.f, 0.f), (m_vAngle.z));

		m_pPick_Trans->Set_Rotation(XMLoadFloat3(&m_vAngle));
		m_pPick_Trans->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.x, m_vPos.y, m_vPos.z, 1.f));
		m_pPick->Tick(0.f);
		m_pPick->LateTick(0.f);
	}

	if (ImGui::Button("Setting"))
	{
		_tchar* _szTemp = CImGui::Get_Instance()->ConvertCtoWC(m_pModels[m_iSelectModel]);
		//CReleaseMgr::Get_Instance()->Add_Tchar(_szTemp);
		auto& iter = find_if(m_mapToolProto.begin(), m_mapToolProto.end(), CTag_Finder(_szTemp));
		CObj_Plus*	_pTemp = nullptr;
		if (iter == m_mapToolProto.end())
		{
			if (m_bAddType == TYPE_ANIM)
				_pTemp = static_cast<CObj_Plus*>(CGameInstance::Get_Instance()->Clone_GameObject(TEXT("Prototype_GameObject_Obj_Anim")));
			else if (m_bAddType == TYPE_NONANIM)
				_pTemp = static_cast<CObj_Plus*>(CGameInstance::Get_Instance()->Clone_GameObject(TEXT("Prototype_GameObject_Obj_NonAnim")));
			m_mapToolProto.emplace(_szTemp,
				_pTemp
			);
		}
		else
		{
			_pTemp = iter->second;
		}

		_matrix		Matrix = XMMatrixIdentity();

		Matrix = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z) *
			XMMatrixRotationX(XMConvertToRadians(m_vAngle.x)) *
			XMMatrixRotationY(XMConvertToRadians(m_vAngle.y)) *
			XMMatrixRotationZ(XMConvertToRadians(m_vAngle.z)) *
			XMMatrixTranslation(0.f, 0.f, 0.f);

		XMStoreFloat4x4(&m_tObj_Desc.matWorld, Matrix);

		m_tObj_Desc.eLevel = g_eCurLevel;
		lstrcpy(m_tObj_Desc.szModelTag, _szTemp);
		_pTemp->Set_Info(m_tObj_Desc);

		m_pPick = _pTemp;
		static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_ADD);

		m_pPick_Trans = static_cast<CTransform*>(m_pPick->Get_ComponentPtr(TEXT("Com_Transform")));
		m_bSetting = true;
		Safe_Delete_Array(_szTemp);
	}

	if (m_bSetting && (CGameInstance::Get_Instance()->MouseDown(DIMK_LBUTTON) && CGameInstance::Get_Instance()->KeyPressing(DIK_LSHIFT)))
	{
		_matrix		Matrix = XMMatrixIdentity();

		Matrix = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z) *
			XMMatrixRotationX((m_vAngle.x)) *
			XMMatrixRotationY((m_vAngle.y)) *
			XMMatrixRotationZ((m_vAngle.z)) *
			XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

		XMStoreFloat4x4(&m_tObj_Desc.matWorld, Matrix);

		m_tObj_Desc.eLevel = g_eCurLevel;

		_tchar* szTemp = nullptr; // 딜리트해야하는 곳 : 오비제이, 여기
		szTemp = CImGui::Get_Instance()->ConvertCtoWC(m_pModels[m_iSelectModel]);
		lstrcpy(m_tObj_Desc.szModelTag, szTemp);
		m_pPick_Trans->Set_Rotation(XMLoadFloat3(&m_vAngle));
		_tchar* szLayer = nullptr;
		szLayer = CImGui::Get_Instance()->ConvertCtoWC(m_pLayers[m_iSelectLayer]);
		
		switch (m_bAddType)
		{
		case TYPE_ANIM:
			if (FAILED(CGameInstance::Get_Instance()->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Obj_Anim"), g_eCurLevel, szLayer, &m_tObj_Desc)))
				return E_FAIL;
			break;
		case TYPE_NONANIM:
			if (FAILED(CGameInstance::Get_Instance()->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Obj_NonAnim"), g_eCurLevel, szLayer, &m_tObj_Desc)))
				return E_FAIL;
			break;
		}
		static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_ADD);
		/*m_pPick = nullptr;
		m_pPick_Trans = nullptr;
		m_bSetting = false;*/
		Safe_Delete_Array(szLayer);
		Safe_Delete_Array(szTemp);
	}

	if (m_bSetting && CGameInstance::Get_Instance()->KeyDown(DIK_ESCAPE))
	{
		static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_NONPICK);
		m_pPick = nullptr;
		m_pPick_Trans = nullptr;
		m_bSetting = false;
		m_bTool_Obj_Add = false;
	}

	ImGui::SameLine();
	if (ImGui::Button("Close"))
	{
		if (m_pPick)
			static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_NONPICK);
		m_pPick = nullptr;
		m_pPick_Trans = nullptr;
		m_bSetting = false;
		m_bTool_Obj_Add = false;
	}
	ImGui::End();

	return S_OK;
}

void CObj_Tool::Load_Layers()
{
	for (auto& iter : m_pLayers)
	{
		Safe_Delete_Array(iter);
	}
	m_pLayers.clear();
	map<const _tchar*, class CLayer*> _mapTemp;
	_mapTemp = CGameInstance::Get_Instance()->Get_Layers(g_eCurLevel);

	for (auto& iter : _mapTemp)
	{
		char* szTemp = CImGui::Get_Instance()->ConvertWCtoC(iter.first);
		m_pLayers.push_back(szTemp);
	}
	m_iSelectLayer = 0;
}



HRESULT CObj_Tool::Key_Input()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	ImGui::InputFloat("PressingSpeed", &_PressingSpeed);

	_long	_lMouseMove = 0;
	if (_lMouseMove = pGameInstance->Get_DIMMoveState(DIMM_WHEEL))
	{
		_float MouseMove = (_float)_lMouseMove / (abs(_lMouseMove)*10.f);
		if (pGameInstance->KeyPressing(DIK_LCONTROL))
		{
			if (pGameInstance->KeyPressing(DIK_Z))
			{
				m_vAngle.z += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_X))
			{
				m_vAngle.x += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_Y))
			{
				m_vAngle.y += _PressingSpeed * (_float)MouseMove;
			}
		}
		else if (pGameInstance->KeyPressing(DIK_LSHIFT))
		{
			if (pGameInstance->KeyPressing(DIK_Z))
			{
				m_vScale.z += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_X))
			{
				m_vScale.x += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_Y))
			{
				m_vScale.y += _PressingSpeed * (_float)MouseMove;
			}
		}
		else
		{
			if (pGameInstance->KeyPressing(DIK_Z))
			{
				m_vPos.z += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_X))
			{
				m_vPos.x += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_Y))
			{
				m_vPos.y += _PressingSpeed * (_float)MouseMove;
			}
		}
	}

	Safe_Release<CGameInstance*>(pGameInstance);
	return S_OK;
}



HRESULT CObj_Tool::Load_Model()
{
	for (auto& iter : m_pModels)
	{
		Safe_Delete_Array(iter);
	}
	m_pModels.clear();
	CGameInstance* _pGameInstance = GET_INSTANCE(CGameInstance);
	map<const _tchar*, class CComponent*> _mapTemp = _pGameInstance->Get_Map(g_eCurLevel);

	auto& iter = _mapTemp.begin();
	_uint _iTemp = 0;
	char* _szTemp = nullptr;

	for (_uint i = 0; i < _mapTemp.size(); ++iter, ++i)
	{
		_szTemp = CImGui::Get_Instance()->ConvertWCtoC(iter->first);
		if (strstr(_szTemp, "Component_Model"))
		{
			m_pModels.push_back(_szTemp);
		}
		else
		{
			Safe_Delete_Array(_szTemp);
		}
	}

	m_bModels = true;
	Safe_Release(_pGameInstance);
	return S_OK;
}

void CObj_Tool::AnimModel_Save()
{
	TANIMMODEL _tModel = static_cast<CAnimModel*>(m_pPick->Get_ComponentPtr(TEXT("Com_Model")))->Get_ForSaveAnim();

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef<CGameInstance*>(pGameInstance);
	_tchar*		_szFileName = nullptr;

	char szFullPath[MAX_PATH] = ""; //여기에 넣을 예정
	char szFileName[MAX_PATH] = ""; //파일 이름
	char szExt[MAX_PATH] = ""; //확장자

	_splitpath_s(_tModel.Name, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
	//strPath에 파일 경로 떼고 파일 이름이랑 확장자 뺀다
	// ex) abc / .png

	strcpy_s(szFullPath, _tModel.Path);
	//파일 경로 넣고
	strcat_s(szFullPath, szFileName);
	//파일 이름 넣고
	strcat_s(szFullPath, ".dat");
	//경로 넣고

	_tchar			szWideFullPath[MAX_PATH] = TEXT("");

	MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);
	//_tchar로 변환 _ szFullPath만큼의 사이즈인 szFullPath를 szWideFullPath에 담는다



	// 2. 파일 쓰기
	//레벨별로 리스트를 얻어온다


	HANDLE		hFile = CreateFile(szWideFullPath,			// 파일 경로와 이름 명시
		GENERIC_WRITE,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		CREATE_ALWAYS,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	DWORD		dwByte = 0;
	WriteFile(hFile, &(_tModel.bAnim), sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &(_tModel.Path), sizeof(char) * 260, &dwByte, nullptr);
	WriteFile(hFile, &(_tModel.Name), sizeof(char) * 260, &dwByte, nullptr);
	WriteFile(hFile, &(_tModel.Pivot), sizeof(_float4x4), &dwByte, nullptr);

	WriteFile(hFile, &(_tModel.NumMeshes), sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < _tModel.NumMeshes; ++i)
	{
		WriteFile(hFile, &(_tModel.tMeshes[i].szName), sizeof(char) * 260, &dwByte, nullptr);
		WriteFile(hFile, &(_tModel.tMeshes[i].iIndex), sizeof(int), &dwByte, nullptr);
		WriteFile(hFile, &(_tModel.tMeshes[i].NumVertices), sizeof(int), &dwByte, nullptr);
		for (int j = 0; j < _tModel.tMeshes[i].NumVertices; ++j)
		{
			WriteFile(hFile, &(_tModel.tMeshes[i].pVertices[j]), sizeof(VTXANIMMODEL), &dwByte, nullptr);
		}
		WriteFile(hFile, &(_tModel.tMeshes[i].NumFaces), sizeof(int), &dwByte, nullptr);
		for (int j = 0; j < _tModel.tMeshes[i].NumFaces; ++j)
		{
			WriteFile(hFile, &(_tModel.tMeshes[i].pIndices[j]), sizeof(FACEINDICES32), &dwByte, nullptr);
		}
		WriteFile(hFile, &(_tModel.tMeshes[i].NumBones), sizeof(int), &dwByte, nullptr);
		for (int j = 0; j < _tModel.tMeshes[i].NumBones; ++j)
		{
			WriteFile(hFile, &(_tModel.tMeshes[i].BoneIndecis[j]), sizeof(int), &dwByte, nullptr);
		}
	}
	WriteFile(hFile, &(_tModel.AllMaterials.NumMaterials), sizeof(int), &dwByte, nullptr);
	for (int i = 0; i < _tModel.AllMaterials.NumMaterials; ++i)
	{
		WriteFile(hFile, &(_tModel.AllMaterials.tMaterial[i]), sizeof(TMATERIAL), &dwByte, nullptr);
	}
	WriteFile(hFile, &(_tModel.NumHierarcky), sizeof(int), &dwByte, nullptr);
	for (int i = 0; i < _tModel.NumHierarcky; ++i)
	{
		WriteFile(hFile, &(_tModel.tHierarcky[i]), sizeof(THIERARCKY), &dwByte, nullptr);
	}

	WriteFile(hFile, &(_tModel.NumAnim), sizeof(int), &dwByte, nullptr);
	for (int i = 0; i < _tModel.NumAnim; ++i)
	{
		WriteFile(hFile, &(_tModel.tAnim[i].szName), sizeof(char) * 260, &dwByte, nullptr);
		WriteFile(hFile, &(_tModel.tAnim[i].fDuration), sizeof(float), &dwByte, nullptr);
		WriteFile(hFile, &(_tModel.tAnim[i].fTickPerSecond), sizeof(float), &dwByte, nullptr);
		WriteFile(hFile, &(_tModel.tAnim[i].fLimitTime), sizeof(float), &dwByte, nullptr);
		WriteFile(hFile, &(_tModel.tAnim[i].fMaxTermTime), sizeof(float), &dwByte, nullptr);
		WriteFile(hFile, &(_tModel.tAnim[i].NumChannel), sizeof(int), &dwByte, nullptr);
		for (int j = 0; j < _tModel.tAnim[i].NumChannel; ++j)
		{
			WriteFile(hFile, &(_tModel.tAnim[i].Bones[j]), sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &(_tModel.tAnim[i].Channels[j].NumKeyFrame), sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &(_tModel.tAnim[i].Channels[j].szName), sizeof(char) * 260, &dwByte, nullptr);
			for (int k = 0; k < _tModel.tAnim[i].Channels[j].NumKeyFrame; ++k)
			{
				WriteFile(hFile, &(_tModel.tAnim[i].Channels[j].KeyFrames[k]), sizeof(KEYFRAME), &dwByte, nullptr);
			}
		}
	}

	CloseHandle(hFile);

	Safe_Release<CGameInstance*>(pGameInstance);
}

void CObj_Tool::NonAnimModel_Save()
{
	TMODEL _tModel = static_cast<CNonAnimModel*>(m_pPick->Get_ComponentPtr(TEXT("Com_Model")))->Get_ForSave();


	_tchar*		_szFileName = nullptr;

	char szFullPath[MAX_PATH] = ""; //여기에 넣을 예정
	char szFileName[MAX_PATH] = ""; //파일 이름
	char szExt[MAX_PATH] = ""; //확장자

	_splitpath_s(_tModel.Name, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
	//strPath에 파일 경로 떼고 파일 이름이랑 확장자 뺀다
	// ex) abc / .png

	strcpy_s(szFullPath, _tModel.Path);
	//파일 경로 넣고
	strcat_s(szFullPath, szFileName);
	//파일 이름 넣고
	strcat_s(szFullPath, ".dat");
	//경로 넣고

	_tchar			szWideFullPath[MAX_PATH] = TEXT("");

	MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);
	//_tchar로 변환 _ szFullPath만큼의 사이즈인 szFullPath를 szWideFullPath에 담는다



	// 2. 파일 쓰기
	//레벨별로 리스트를 얻어온다


	HANDLE		hFile = CreateFile(szWideFullPath,			// 파일 경로와 이름 명시
		GENERIC_WRITE,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		CREATE_ALWAYS,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	DWORD		dwByte = 0;
	WriteFile(hFile, &(_tModel.bAnim), sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &(_tModel.Path), sizeof(char) * 260, &dwByte, nullptr);
	WriteFile(hFile, &(_tModel.Name), sizeof(char) * 260, &dwByte, nullptr);
	WriteFile(hFile, &(_tModel.NumMeshes), sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < _tModel.NumMeshes; ++i)
	{
		WriteFile(hFile, &(_tModel.tMeshes[i].szName), sizeof(char) * 260, &dwByte, nullptr);
		WriteFile(hFile, &(_tModel.tMeshes[i].iIndex), sizeof(int), &dwByte, nullptr);
		WriteFile(hFile, &(_tModel.tMeshes[i].NumVertices), sizeof(int), &dwByte, nullptr);
		for (int j = 0; j < _tModel.tMeshes[i].NumVertices; ++j)
		{
			WriteFile(hFile, &(_tModel.tMeshes[i].pVertices[j]), sizeof(VTXMODEL), &dwByte, nullptr);
		}
		WriteFile(hFile, &(_tModel.tMeshes[i].NumFaces), sizeof(int), &dwByte, nullptr);
		for (int j = 0; j < _tModel.tMeshes[i].NumFaces; ++j)
		{
			WriteFile(hFile, &(_tModel.tMeshes[i].pIndices[j]), sizeof(FACEINDICES32), &dwByte, nullptr);
		}
	}
	WriteFile(hFile, &(_tModel.AllMaterials.NumMaterials), sizeof(int), &dwByte, nullptr);
	for (int i = 0; i < _tModel.AllMaterials.NumMaterials; ++i)
	{
		WriteFile(hFile, &(_tModel.AllMaterials.tMaterial[i]), sizeof(TMATERIAL), &dwByte, nullptr);
	}

	CloseHandle(hFile);

}
