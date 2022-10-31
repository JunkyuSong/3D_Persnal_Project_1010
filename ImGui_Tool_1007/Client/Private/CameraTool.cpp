#include "stdafx.h"
#include "..\Public\CameraTool.h"
#include "GameInstance.h"
#include "PosCube.h"
#include "MoveCube.h"
#include "LookCube.h"
#include "LookAtCube.h"

CCameraTool::CCameraTool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CCameraTool::Initalize()
{
	LoadSceneList();
}

void CCameraTool::ShowCameraWindow(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	if (m_pMoveCube == nullptr)
	{
		CMoveCube::COLORCUBEDESC ColorCubeDesc;
		ZeroMemory(&ColorCubeDesc, sizeof(CMoveCube::COLORCUBEDESC));
		ColorCubeDesc.vColor = _float4(1.f, 0.f, 0.f, 1.f);

		_pInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MoveCube"), LEVEL_GAMEPLAY, L"Layer_Cube", &ColorCubeDesc);
	}

	if (m_pLookAt == nullptr)
	{
		CLookAtCube::COLORCUBEDESC ColorCubeDesc;
		ZeroMemory(&ColorCubeDesc, sizeof(CMoveCube::COLORCUBEDESC));
		ColorCubeDesc.vColor = _float4(64.f, 0.f, 128.f, 1.f);

		_pInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_LookAtCube"), LEVEL_GAMEPLAY, L"Layer_Cube", &ColorCubeDesc);
	}

	ImGui::PushItemWidth(200.f);
	if (ImGui::BeginCombo(" ", m_SceneList[m_iSelectedSceneList], ImGuiComboFlags_HeightLargest)) {
		for (int i = 0; i < m_SceneList.size(); ++i) {
			const bool isSelected = (m_iSelectedSceneList == i);
			if (ImGui::Selectable(m_SceneList[i], isSelected)) {
				m_iSelectedSceneList = i;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();

	if (ImGui::Button("LoadScene"))
	{
		if (m_SceneList.size() > 1)
		{
			char temp[256];
			strcpy_s(temp, sizeof(temp), m_SceneList[m_iSelectedSceneList]);
			LoadScene(temp);
		}
	}

	if (m_SceneList.size() > 1)
	{
		string PosNum = to_string(m_PosCube.size());
		string PosCubeNum = "PosCubeNum : " + PosNum;
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, PosCubeNum.c_str());

		ImGui::SameLine();

		string LookNum = to_string(m_LookCube.size());
		string LookCubeNum = "LookCubeNum : " + LookNum;
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, LookCubeNum.c_str());
	}

	FixMoveCube();

	string NowScene = "NowScene Name : " + m_sNowScene;
	ImGui::TextColored(ImVec4{ 0.f,1.f,1.f,1.f }, NowScene.c_str());

	ImGui::PushItemWidth(200.f);
	ImGui::Text("");
	static char NaviName[256] = "";
	ImGui::InputText("SceneName", NaviName, IM_ARRAYSIZE(NaviName));

	if (ImGui::Button("SaveScene"))
	{
		if (strcmp(NaviName, ""))
			SaveScene(NaviName);
	}
	ImGui::PopItemWidth();

}

void CCameraTool::CameraKeyInput()
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	if (_pInstance->KeyUp(DIK_Z))
	{
		CPosCube::POSCUBEDESC PosCubeDesc;
		ZeroMemory(&PosCubeDesc, sizeof(CMoveCube::COLORCUBEDESC));
		PosCubeDesc.vPos = m_pMoveCube->Get_Pos();
		PosCubeDesc.vColor = _float4(0.f, 1.f, 0.f, 1.f);

		char Temp[30];
		_itoa_s(m_iPosCubeIndex, Temp, 10);

		string Name = "PosCube";
		string Index = Temp;
		string FullName = Name + Index;
		const char* temp = FullName.c_str();
		char* RealName = new char[30];
		for (int i = 0; i < 30; ++i)
		{
			RealName[i] = temp[i];
		}
		++m_iPosCubeIndex;
		m_PosCubeName.push_back(RealName);


		_pInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PosCube"), LEVEL_GAMEPLAY, L"Layer_Cube", &PosCubeDesc);
	}

	if (_pInstance->KeyUp(DIK_X))
	{
		CLookCube::LOOKCUBEDESC LookCubeDesc;
		ZeroMemory(&LookCubeDesc, sizeof(CMoveCube::COLORCUBEDESC));
		LookCubeDesc.vPos = m_pMoveCube->Get_Pos();
		LookCubeDesc.vColor = _float4(1.f, 1.f, 0.f, 1.f);

		char Temp[30];
		_itoa_s(m_iLookCubeIndex, Temp, 10);

		string Name = "LookCube";
		string Index = Temp;
		string FullName = Name + Index;
		const char* temp = FullName.c_str();
		char* RealName = new char[30];
		for (int i = 0; i < 30; ++i)
		{
			RealName[i] = temp[i];
		}
		++m_iLookCubeIndex;
		m_LookCubeName.push_back(RealName);

		_pInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_LookCube"), LEVEL_GAMEPLAY, L"Layer_Cube", &LookCubeDesc);
	}
}

void CCameraTool::SaveScene(char * FileName)
{
	if (m_PosCube.size() < 1)
	{
		MSG_BOX(TEXT("Don't have Data"));
		return;
	}
	string FileSave = FileName;

	string temp = "../Data/SceneData/";

	FileSave = temp + FileSave + ".dat";

	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileSave.size(); i++)
	{
		FilePath[i] = FileSave[i];
	}

	HANDLE		hFile = CreateFile(FilePath,			// 파일 경로와 이름 명시
		GENERIC_WRITE,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		CREATE_ALWAYS,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, _T("Save File"), _T("Fail"), MB_OK);
		return;
	}

	// 2. 파일 쓰기

	DWORD		dwByte = 0;

	int PosCubeSize = m_PosCube.size();
	WriteFile(hFile, &PosCubeSize, sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < PosCubeSize; ++i)
	{
		WriteFile(hFile, &m_PosCube[i]->Get_Pos(), sizeof(_float3), &dwByte, nullptr);
		_float CamSpeed = m_PosCube[i]->Get_CamSpeed();
		WriteFile(hFile, &CamSpeed, sizeof(_float), &dwByte, nullptr);
		_float StopLimit = m_PosCube[i]->Get_StopLimit();
		WriteFile(hFile, &StopLimit, sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &m_PosCube[i]->Get_RGB(), sizeof(_float4), &dwByte, nullptr);
	}

	int LookCubeSize = m_LookCube.size();
	WriteFile(hFile, &LookCubeSize, sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < LookCubeSize; ++i)
	{
		WriteFile(hFile, &m_LookCube[i]->Get_Pos(), sizeof(_float3), &dwByte, nullptr);
		_float CamSpeed = m_LookCube[i]->Get_CamSpeed();
		WriteFile(hFile, &CamSpeed, sizeof(_float), &dwByte, nullptr);
		_float StopLimit = m_LookCube[i]->Get_StopLimit();
		WriteFile(hFile, &StopLimit, sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &m_LookCube[i]->Get_RGB(), sizeof(_float4), &dwByte, nullptr);
	}

	MSG_BOX(TEXT("Succeed Save"));
	char* ListTemp = new char[256];
	strcpy_s(ListTemp, sizeof(char) * 256, FileName);
	if (!Find_SceneList(ListTemp))
		m_SceneList.push_back(ListTemp);
	else
		Safe_Delete(ListTemp);

	SaveSceneList();
	// 3. 파일 소멸
	CloseHandle(hFile);

	
}

void CCameraTool::LoadScene(char * FileName)
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	string FileSave = FileName;

	string temp = "../Data/SceneData/";

	FileSave = temp + FileSave + ".dat";

	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileSave.size(); i++)
	{
		FilePath[i] = FileSave[i];
	}

	HANDLE		hFile = CreateFile(FilePath,			// 파일 경로와 이름 명시
		GENERIC_READ,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		OPEN_EXISTING,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, _T("Load File"), _T("Fail"), MB_OK);
		return;
	}

	// 2. 파일 쓰기

	for (auto& iter : m_PosCube)
		iter->Set_Dead();
	m_PosCube.clear();

	for (auto& iter : m_LookCube)
		iter->Set_Dead();
	m_LookCube.clear();

	for (auto& iter : m_PosCubeName)
		Safe_Delete(iter);
	m_PosCubeName.clear();

	for (auto& iter : m_LookCubeName)
		Safe_Delete(iter);
	m_LookCubeName.clear();

	m_iPosCubeIndex = 0;
	m_iLookCubeIndex = 0;


	DWORD		dwByte = 0;

	int PosCubeSize;
	ReadFile(hFile, &PosCubeSize, sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < PosCubeSize; ++i)
	{
		_float3 Pos;
		ReadFile(hFile, &Pos, sizeof(_float3), &dwByte, nullptr);
		
		_float CamSpeed;
		ReadFile(hFile, &CamSpeed, sizeof(_float), &dwByte, nullptr);

		_float StopLimit;
		ReadFile(hFile, &StopLimit, sizeof(_float), &dwByte, nullptr);

		_float4 RGB;
		ReadFile(hFile, &RGB, sizeof(_float4), &dwByte, nullptr);

		if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
		{
			break;
		}


		CPosCube::POSCUBEDESC PosCubeDesc;
		ZeroMemory(&PosCubeDesc, sizeof(CMoveCube::COLORCUBEDESC));
		PosCubeDesc.vPos = Pos;
		PosCubeDesc.vColor = RGB;
		PosCubeDesc.fCamSpeed = CamSpeed;
		PosCubeDesc.fStopLimit = StopLimit;

		char Temp[30];
		_itoa_s(m_iPosCubeIndex, Temp, 10);

		string Name = "PosCube";
		string Index = Temp;
		string FullName = Name + Index;
		const char* temp = FullName.c_str();
		char* RealName = new char[30];
		for (int i = 0; i < 30; ++i)
		{
			RealName[i] = temp[i];
		}
		++m_iPosCubeIndex;
		m_PosCubeName.push_back(RealName);


		_pInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PosCube"), LEVEL_GAMEPLAY, L"Layer_Cube", &PosCubeDesc);
	}

	int LookCubeSize;
	ReadFile(hFile, &LookCubeSize, sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < LookCubeSize; ++i)
	{
		_float3 Pos;
		ReadFile(hFile, &Pos, sizeof(_float3), &dwByte, nullptr);

		_float CamSpeed;
		ReadFile(hFile, &CamSpeed, sizeof(_float), &dwByte, nullptr);

		_float StopLimit;
		ReadFile(hFile, &StopLimit, sizeof(_float), &dwByte, nullptr);

		_float4 RGB;
		ReadFile(hFile, &RGB, sizeof(_float4), &dwByte, nullptr);

		if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
		{
			break;
		}


		CLookCube::LOOKCUBEDESC LookCubeDesc;
		ZeroMemory(&LookCubeDesc, sizeof(CMoveCube::COLORCUBEDESC));
		LookCubeDesc.vPos = Pos;
		LookCubeDesc.vColor = RGB;
		LookCubeDesc.fCamSpeed = CamSpeed;
		LookCubeDesc.fStopLimit = StopLimit;

		char Temp[30];
		_itoa_s(m_iLookCubeIndex, Temp, 10);

		string Name = "LookCube";
		string Index = Temp;
		string FullName = Name + Index;
		const char* temp = FullName.c_str();
		char* RealName = new char[30];
		for (int i = 0; i < 30; ++i)
		{
			RealName[i] = temp[i];
		}
		++m_iLookCubeIndex;
		m_LookCubeName.push_back(RealName);


		_pInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_LookCube"), LEVEL_GAMEPLAY, L"Layer_Cube", &LookCubeDesc);
	}
	
	m_iSelectedPosCube = 0;
	m_iSelectedLookCube = 0;

	m_sNowScene = FileName;

	// 3. 파일 소멸
	CloseHandle(hFile);

	

	
}

void CCameraTool::SaveSceneList()
{
	string FileSave = "../Data/ListData/SceneList.dat";

	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileSave.size(); i++)
	{
		FilePath[i] = FileSave[i];
	}

	HANDLE		hFile = CreateFile(FilePath,			// 파일 경로와 이름 명시
		GENERIC_WRITE,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		CREATE_ALWAYS,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		// 팝업 창을 출력해주는 기능의 함수
		// 1. 핸들 2. 팝업 창에 띄우고자하는 메시지 3. 팝업 창 이름 4. 버튼 속성
		MessageBox(g_hWnd, _T("Save File"), _T("Fail"), MB_OK);
		return;
	}

	DWORD		dwByte = 0;

	for (auto& Data : m_SceneList)
	{

		char ListName[256] = { 0 };
		strcpy_s(ListName, sizeof(char) * 256, Data);

		WriteFile(hFile, ListName, sizeof(char) * 256, &dwByte, nullptr);
	}

	CloseHandle(hFile);
}

void CCameraTool::LoadSceneList()
{
	string FileSave = "../Data/ListData/SceneList.dat";

	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileSave.size(); i++)
	{
		FilePath[i] = FileSave[i];
	}

	HANDLE		hFile = CreateFile(FilePath,			// 파일 경로와 이름 명시
		GENERIC_READ,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		OPEN_EXISTING,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		// 팝업 창을 출력해주는 기능의 함수
		// 1. 핸들 2. 팝업 창에 띄우고자하는 메시지 3. 팝업 창 이름 4. 버튼 속성
		MessageBox(g_hWnd, _T("Load File"), _T("Fail"), MB_OK);
		return;
	}

	DWORD		dwByte = 0;

	while (true)
	{

		char* ListName = new char[256];
		ReadFile(hFile, ListName, sizeof(char) * 256, &dwByte, nullptr);

		if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
		{
			Safe_Delete_Array(ListName);
			break;
		}

		string FileSave = "../Data/SceneData/";

		string FullPath = FileSave + ListName + ".dat";

		wchar_t FilePath[256] = { 0 };

		for (int i = 0; i < FullPath.size(); i++)
		{
			FilePath[i] = FullPath[i];
		}


		HANDLE		hFileSearch = CreateFile(FilePath,			// 파일 경로와 이름 명시
			GENERIC_READ,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
			NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
			NULL,						// 보안 속성, 기본값	
			OPEN_EXISTING,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
			FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
			NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

		if (INVALID_HANDLE_VALUE != hFileSearch)
			m_SceneList.push_back(ListName);
		else
			Safe_Delete_Array(ListName);
		CloseHandle(hFileSearch);
	}

	// 3. 파일 소멸
	CloseHandle(hFile);
}

bool CCameraTool::Find_SceneList(const char * ListName)
{
	for (auto& iter : m_SceneList)
	{
		if (!strcmp(iter, ListName))
			return true;
	}
	return false;
}

void CCameraTool::FixMoveCube()
{
	_float3 Pos = m_pMoveCube->Get_Pos();
	m_fMoveCubeX = Pos.x;
	ImGui::DragFloat("PosX", &m_fMoveCubeX, 0.01f, -1000.0f, 1000.0f);

	m_fMoveCubeY = Pos.y;
	ImGui::DragFloat("PosY", &m_fMoveCubeY, 0.01f, -1000.0f, 1000.0f);

	m_fMoveCubeZ = Pos.z;
	ImGui::DragFloat("PosZ", &m_fMoveCubeZ, 0.01f, -1000.0f, 1000.0f);
	m_pMoveCube->Set_Pos(_float3{ m_fMoveCubeX, m_fMoveCubeY, m_fMoveCubeZ });
}

void CCameraTool::Set_MoveCubePos(_float3 vPos)
{
	m_pMoveCube->Set_Pos(vPos);
}

void CCameraTool::ShowCameraEditWindow(_float fTimeDelta)
{
	ImGui::Begin("CameraEdit");                          // Create a window called "Hello, world!" and append into it.

	ImGui::SetWindowSize(ImVec2{ 400.f, 500.f });

	if (ImGui::Button("PosEdit"))
	{
		m_bLookEdit = false;
		m_bPosEdit = true;
	}
	ImGui::SameLine();

	if (ImGui::Button("LookEdit"))
	{
		m_bPosEdit = false;
		m_bLookEdit = true;
	}

	ImGui::SameLine();

	if (m_bPosEdit)
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, "PosEdit");
	else if (m_bLookEdit)
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, "LookEdit");

	if (m_bPosEdit)
	{
		if (ImGui::BeginListBox("PosCubeList", ImVec2(200, 150)))
		{

			for (int i = 0; i < m_PosCubeName.size(); i++)
			{
				const bool is_selected = (m_iSelectedPosCube == i);
				if (ImGui::Selectable(m_PosCubeName[i], is_selected))
					m_iSelectedPosCube = i;

				if (is_selected)
					ImGui::SetItemDefaultFocus();

			}
			ImGui::EndListBox();
		}
	}

	//ImGui::SameLine();

	if (m_bLookEdit)
	{
		if (ImGui::BeginListBox("LookCubeList", ImVec2(200, 150)))
		{
			for (int i = 0; i < m_LookCubeName.size(); i++)
			{
				const bool is_selected = (m_iSelectedLookCube == i);
				if (ImGui::Selectable(m_LookCubeName[i], is_selected))
					m_iSelectedLookCube = i;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}
	if (!m_bPlay)
		DataFix();

	if (ImGui::Button("Delete Cube"))
	{
		DeleteCube();
	}

	if (ImGui::Button("Play"))
	{
		if (!m_bPlay)
			m_pCamera->Set_Pos(m_PosCube[0]->Get_Pos());
		m_pLookAt->Set_Pos(m_LookCube[0]->Get_Pos());
		m_bPlay = true;
		m_bLookPlay = true;
		m_bStop = false;
	}

	ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		m_bStop = true;
	}

	if (ImGui::Button("Reset"))
	{
		m_bPlay = false;
		m_bLookPlay = false;
		m_bStop = true;

		for (auto& iter : m_PosCube)
			iter->Set_Stop();

		for (auto& iter : m_LookCube)
			iter->Set_Stop();

		m_pCamera->Set_Pos(m_PosCube[0]->Get_Pos());
		m_pLookAt->Set_Pos(m_LookCube[0]->Get_Pos());
		m_iNowPosCube = 0;
		m_iNowLookCube = 0;
	}

	if (m_bPlay && !m_bStop)
		PlayScene(fTimeDelta);


	ImGui::End();
}

void CCameraTool::DataFix()
{
	if (m_bPosEdit)
	{
		if (m_PosCube.size() >= 1)
		{
			_float3 fPos = m_PosCube[m_iSelectedPosCube]->Get_Pos();
			float PosX = fPos.x;
			ImGui::DragFloat("PosX", &PosX, 0.1f, -100.f, 300.f, "%.2f", 0);

			float PosY = fPos.y;
			ImGui::DragFloat("PosY", &PosY, 0.1f, -100.f, 300.f, "%.2f", 0);

			float PosZ = fPos.z;
			ImGui::DragFloat("PosZ", &PosZ, 0.1f, -100.f, 300.f, "%.2f", 0);
			_float3 temp = { PosX,PosY,PosZ };
			m_PosCube[m_iSelectedPosCube]->Set_Pos(temp);

			float Speed = m_PosCube[m_iSelectedPosCube]->Get_CamSpeed();
			ImGui::DragFloat("Speed", &Speed, 0.1f, 0.f, 100.f, "%.2f", 0);
			m_PosCube[m_iSelectedPosCube]->Set_CamSpeed(Speed);

			float StopLimit = m_PosCube[m_iSelectedPosCube]->Get_StopLimit();
			ImGui::DragFloat("StopLimit", &StopLimit, 0.f, 0.f, 10.f, "%.2f", 0);
			m_PosCube[m_iSelectedPosCube]->Set_StopLimit(StopLimit);
			if (StopLimit >= 1.f)
				m_PosCube[m_iSelectedPosCube]->Set_Stop();



		}
	}

	if (m_bLookEdit)
	{
		if (m_LookCube.size() >= 1)
		{
			_float3 fPos = m_LookCube[m_iSelectedLookCube]->Get_Pos();
			float PosX = fPos.x;
			ImGui::DragFloat("PosX", &PosX, 0.1f, -100.f, 300.f, "%.2f", 0);

			float PosY = fPos.y;
			ImGui::DragFloat("PosY", &PosY, 0.1f, -100.f, 300.f, "%.2f", 0);

			float PosZ = fPos.z;
			ImGui::DragFloat("PosZ", &PosZ, 0.1f, -100.f, 300.f, "%.2f", 0);
			_float3 temp = { PosX,PosY,PosZ };
			m_LookCube[m_iSelectedLookCube]->Set_Pos(temp);

			float Speed = m_LookCube[m_iSelectedLookCube]->Get_CamSpeed();
			ImGui::DragFloat("Speed", &Speed, 0.1f, 0.f, 100.f, "%.2f", 0);
			m_LookCube[m_iSelectedLookCube]->Set_CamSpeed(Speed);

			float StopLimit = m_LookCube[m_iSelectedLookCube]->Get_StopLimit();
			ImGui::DragFloat("StopLimit", &StopLimit, 0.f, 0.f, 10.f, "%.2f", 0);
			m_LookCube[m_iSelectedLookCube]->Set_StopLimit(StopLimit);
			if (StopLimit >= 1.f)
				m_LookCube[m_iSelectedLookCube]->Set_Stop();
		}
	}
}

void CCameraTool::DeleteCube()
{
	if (m_bPosEdit)
	{
		if (m_PosCube.size() > 0)
		{
			m_PosCube[m_iSelectedPosCube]->Set_Dead();
			m_PosCube.erase(m_PosCube.begin() + m_iSelectedPosCube);
			if (m_iSelectedPosCube > 0)
				--m_iSelectedPosCube;
			SortCube();
		}
	}

	else
	{
		if (m_LookCube.size() > 0)
		{
			m_LookCube[m_iSelectedLookCube]->Set_Dead();
			m_LookCube.erase(m_LookCube.begin() + m_iSelectedLookCube);
			if (m_iSelectedLookCube > 0)
				--m_iSelectedLookCube;
			SortCube();
		}
	}
}

void CCameraTool::SortCube()
{
	if (m_bPosEdit)
	{
		for (auto& iter : m_PosCubeName)
		{
			Safe_Delete(iter);
		}
		m_PosCubeName.clear();
		m_iPosCubeIndex = 0;

		if (m_PosCube.size() > 0)
		{
			for (m_iPosCubeIndex; m_iPosCubeIndex < m_PosCube.size(); ++m_iPosCubeIndex)
			{
				char Temp[30];
				_itoa_s(m_iPosCubeIndex, Temp, 10);

				string Name = "PosCube";
				string Index = Temp;
				string FullName = Name + Index;
				const char* temp = FullName.c_str();
				char* RealName = new char[30];
				for (int i = 0; i < 30; ++i)
				{
					RealName[i] = temp[i];
				}
				m_PosCubeName.push_back(RealName);
			}		
		}
	}

	else
	{
		for (auto& iter : m_LookCubeName)
		{
			Safe_Delete(iter);
		}
		m_LookCubeName.clear();
		m_iLookCubeIndex = 0;

		if (m_LookCube.size() > 0)
		{
		
			for (m_iLookCubeIndex; m_iLookCubeIndex < m_LookCube.size(); ++m_iLookCubeIndex)
			{
				char Temp[30];
				_itoa_s(m_iLookCubeIndex, Temp, 10);

				string Name = "LookCube";
				string Index = Temp;
				string FullName = Name + Index;
				const char* temp = FullName.c_str();
				char* RealName = new char[30];
				for (int i = 0; i < 30; ++i)
				{
					RealName[i] = temp[i];
				}
				m_LookCubeName.push_back(RealName);
			}
		}
	}
}

void CCameraTool::PlayScene(_float fTimeDelta)
{
	m_pCamera->LookAt(m_pLookAt->Get_Pos());
	if (!m_PosCube[m_iNowPosCube]->Get_Stop())
	{
		_vector vTempPos = XMLoadFloat3(&m_PosCube[m_iNowPosCube]->Get_Pos());
		vTempPos = XMVectorSetW(vTempPos, 1.f);
		if (m_pCamera->Move(vTempPos, m_PosCube[m_iNowPosCube]->Get_CamSpeed(), fTimeDelta, m_PosCube[m_iNowPosCube]->Get_CamSpeed() * 0.02f))
		{
			++m_iNowPosCube;
			if (m_iNowPosCube == m_PosCube.size())
			{
				m_bPlay = false;
				m_iNowPosCube = 0;
				for (auto& iter : m_PosCube)
					iter->PlayEnd();

				m_iNowLookCube = 0;
				for (auto& iter : m_LookCube)
					iter->PlayEnd();

				for (auto& iter : m_PosCube)
					iter->Set_Stop();

				for (auto& iter : m_LookCube)
					iter->Set_Stop();
			}
		}
	}
	else
		m_PosCube[m_iNowPosCube]->Stop(fTimeDelta);

	if (m_bLookPlay)
	{
		if (!m_LookCube[m_iNowLookCube]->Get_Stop())
		{
			_vector vTempPos = XMLoadFloat3(&m_LookCube[m_iNowLookCube]->Get_Pos());
			vTempPos = XMVectorSetW(vTempPos, 1.f);
			if (m_pLookAt->Move(vTempPos, m_LookCube[m_iNowLookCube]->Get_CamSpeed(), fTimeDelta, m_LookCube[m_iNowLookCube]->Get_CamSpeed() * 0.02f))
			{
				++m_iNowLookCube;
				if (m_iNowLookCube == m_LookCube.size())
				{
					m_bLookPlay = false;
				}
			}
		}
		else
			m_LookCube[m_iNowLookCube]->Stop(fTimeDelta);
	}
}


void CCameraTool::Free()
{
	Safe_Release(m_pCamera);
	//if (m_pMoveCube != nullptr)
	Safe_Release(m_pMoveCube);
	//if(m_pLookAt != nullptr)
	Safe_Release(m_pLookAt);

	for (auto& iter : m_PosCubeName)
	{
		Safe_Delete(iter);
	}

	for (auto& iter : m_LookCubeName)
	{
		Safe_Delete(iter);
	}

	for (auto& iter : m_SceneList)
		Safe_Delete(iter);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
