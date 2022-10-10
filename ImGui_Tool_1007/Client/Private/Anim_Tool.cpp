#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "stdafx.h"
#include "..\Public\Anim_Tool.h"

#include "Obj_Tool.h"

#include "Animation.h"
#include "HierarchyNode.h"
#include "Player.h"
#include "Obj_Anim.h"
#include "Channel.h"

IMPLEMENT_SINGLETON(CAnim_Tool)

CAnim_Tool::CAnim_Tool()
{
}


CAnim_Tool::~CAnim_Tool()
{
}

void CAnim_Tool::Tick()
{
	
	ImGui::Begin("AnimTool");
	ImGui::Text("Animation");
	//m_iSelectAnimList = *static_cast<CObj_Anim*>(m_pPick)->Get_AnimState();
	switch (m_eType)
	{
	case Client::CAnim_Tool::TYPE_PLAYER:
		m_iSelectAnimList = *static_cast<CPlayer*>(m_pPick)->Get_AnimState();
		break;
	case Client::CAnim_Tool::TYPE_NONPLAYER:
		m_iSelectAnimList = *static_cast<CObj_Anim*>(m_pPick)->Get_AnimState();
		break;
	}
	if (ImGui::BeginListBox("Animation List", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int i = 0; i < m_pAnimList.size(); ++i)
		{
			const bool isSelected = (m_iSelectAnimList == i);
			if (ImGui::Selectable(m_pAnimList[i], isSelected))
			{
				//
				switch (m_eType)
				{
				case Client::CAnim_Tool::TYPE_PLAYER:
					*static_cast<CPlayer*>(m_pPick)->Get_AnimState() = (CPlayer::STATE)i;
					break;
				case Client::CAnim_Tool::TYPE_NONPLAYER:
					*static_cast<CObj_Anim*>(m_pPick)->Get_AnimState() = (CObj_Anim::STATE)i;
					break;
				}
				
				m_iSelectAnimList = i;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("Set Name"))
	{
		m_bSetName = true;
		
		switch (m_eType)
		{
		case Client::CAnim_Tool::TYPE_PLAYER:
			static_cast<CPlayer*>(m_pPick)->Set_Stop(true);
			break;
		case Client::CAnim_Tool::TYPE_NONPLAYER:
			static_cast<CObj_Anim*>(m_pPick)->Set_Stop(true);
			break;
		}
		
		strcpy_s(m_szName, (*m_vecAnim)[m_iSelectAnimList]->Get_Name());
	}

	if (ImGui::Button("Add"))
	{
		m_bAdd = true;
		strcpy_s(m_szName, "");
	}
	if (m_bAdd)
	{
		ImGui::Begin("Add Anim");
		ImGui::InputText("Name", m_szName, 260);
		if (ImGui::Button("Enter"))
		{
			AddAnim();
			
			Update_AnimList();
			m_bAdd = false;
		}
		ImGui::End();
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		if (m_iSelectAnimList > 0)
		{
			Safe_Release((*m_vecAnim)[m_iSelectAnimList]);
			
			auto& iter = (*m_vecAnim).begin();
			for (int i = 0; i != m_iSelectAnimList; ++iter, ++i);

			(*m_vecAnim).erase(iter);

			Update_AnimList();
			switch (m_eType)
			{
			case Client::CAnim_Tool::TYPE_PLAYER:
				*static_cast<CPlayer*>(m_pPick)->Get_AnimState() = CPlayer::STATE(m_iSelectAnimList - 1);
				break;
			case Client::CAnim_Tool::TYPE_NONPLAYER:
				*static_cast<CObj_Anim*>(m_pPick)->Get_AnimState() = CObj_Anim::STATE(m_iSelectAnimList - 1);
				break;
			}
			//*static_cast<CObj_Anim*>(m_pPick)->Get_AnimState() = CObj_Anim::STATE(m_iSelectAnimList - 1);
		}

	}

	if (ImGui::Button("Up"))
	{
		if (m_iSelectAnimList > 0)
		{
			CAnimation* _Temp = (*m_vecAnim)[m_iSelectAnimList];
			(*m_vecAnim)[m_iSelectAnimList] = (*m_vecAnim)[m_iSelectAnimList - 1];
			(*m_vecAnim)[m_iSelectAnimList - 1] = _Temp;
			Update_AnimList();
			//*static_cast<CObj_Anim*>(m_pPick)->Get_AnimState() = CObj_Anim::STATE(m_iSelectAnimList - 1);
			
			switch (m_eType)
			{
			case Client::CAnim_Tool::TYPE_PLAYER:
				*static_cast<CPlayer*>(m_pPick)->Get_AnimState() = CPlayer::STATE(m_iSelectAnimList - 1);
				break;
			case Client::CAnim_Tool::TYPE_NONPLAYER:
				*static_cast<CObj_Anim*>(m_pPick)->Get_AnimState() = CObj_Anim::STATE(m_iSelectAnimList - 1);
				break;
			}
		}
		
	}
	ImGui::SameLine();
	if (ImGui::Button("Down"))
	{
		if (m_iSelectAnimList < (*m_vecAnim).size() - 1)
		{
			CAnimation* _Temp = (*m_vecAnim)[m_iSelectAnimList];
			(*m_vecAnim)[m_iSelectAnimList] = (*m_vecAnim)[m_iSelectAnimList + 1];
			(*m_vecAnim)[m_iSelectAnimList + 1] = _Temp;
			Update_AnimList();
			//*static_cast<CObj_Anim*>(m_pPick)->Get_AnimState() = CObj_Anim::STATE(m_iSelectAnimList + 1);
			switch (m_eType)
			{
			case Client::CAnim_Tool::TYPE_PLAYER:
				*static_cast<CPlayer*>(m_pPick)->Get_AnimState() = CPlayer::STATE(m_iSelectAnimList + 1);
				break;
			case Client::CAnim_Tool::TYPE_NONPLAYER:
				*static_cast<CObj_Anim*>(m_pPick)->Get_AnimState() = CObj_Anim::STATE(m_iSelectAnimList + 1);
				break;
			}
		}
	}

	if (ImGui::Button("Play"))
	{
		//static_cast<CObj_Anim*>(m_pPick)->Set_Stop(false);
		
		switch (m_eType)
		{
		case Client::CAnim_Tool::TYPE_PLAYER:
			static_cast<CPlayer*>(m_pPick)->Set_Stop(false);
			break;
		case Client::CAnim_Tool::TYPE_NONPLAYER:
			static_cast<CObj_Anim*>(m_pPick)->Set_Stop(false);
			break;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		//static_cast<CObj_Anim*>(m_pPick)->Set_Stop(true);
		switch (m_eType)
		{
		case Client::CAnim_Tool::TYPE_PLAYER:
			static_cast<CPlayer*>(m_pPick)->Set_Stop(true);
			break;
		case Client::CAnim_Tool::TYPE_NONPLAYER:
			static_cast<CObj_Anim*>(m_pPick)->Set_Stop(true);
			break;
		}
	}

	ImGui::InputInt("SwapNum", &m_iSwap);
	if (ImGui::Button("Swap"))
	{
		if (m_iSwap > -1 && m_iSwap < (*m_vecAnim).size())
		{
			CAnimation* _Temp = (*m_vecAnim)[m_iSelectAnimList];
			(*m_vecAnim)[m_iSelectAnimList] = (*m_vecAnim)[m_iSwap];
			(*m_vecAnim)[m_iSwap] = _Temp;
			Update_AnimList();
			//*static_cast<CObj_Anim*>(m_pPick)->Get_AnimState() = CObj_Anim::STATE(m_iSwap);
			
			switch (m_eType)
			{
			case Client::CAnim_Tool::TYPE_PLAYER:
				*static_cast<CPlayer*>(m_pPick)->Get_AnimState() = CPlayer::STATE(m_iSwap);
				break;
			case Client::CAnim_Tool::TYPE_NONPLAYER:
				*static_cast<CObj_Anim*>(m_pPick)->Get_AnimState() = CObj_Anim::STATE(m_iSwap);
				break;
			}
		}
	}

	ImGui::InputFloat("Duration", (*m_vecAnim)[m_iSelectAnimList]->Get_Duration());
	ImGui::InputFloat("PlayTime",(*m_vecAnim)[m_iSelectAnimList]->Get_PlayTime());
	ImGui::InputFloat("TickPerSecond", (*m_vecAnim)[m_iSelectAnimList]->Get_TickPerSecond());
	ImGui::InputFloat("LimitTime", (*m_vecAnim)[m_iSelectAnimList]->Get_LimitTime());

	/*ImGui::Text("MaxState : %d", static_cast<CObj_Anim*>(m_pPick)->Get_MaxState());

	ImGui::InputInt("CurrenState", (int*)static_cast<CObj_Anim*>(m_pPick)->Get_AnimState());
	ImGui::InputInt("ReserveState", (int*)static_cast<CObj_Anim*>(m_pPick)->Get_AnimReserveState());*/

	switch (m_eType)
	{
	case Client::CAnim_Tool::TYPE_PLAYER:
		ImGui::Text("MaxState : %d", static_cast<CPlayer*>(m_pPick)->Get_MaxState());

		ImGui::InputInt("CurrenState", (int*)static_cast<CPlayer*>(m_pPick)->Get_AnimState());
		ImGui::InputInt("ReserveState", (int*)static_cast<CPlayer*>(m_pPick)->Get_AnimReserveState());
		break;
	case Client::CAnim_Tool::TYPE_NONPLAYER:
		ImGui::Text("MaxState : %d", static_cast<CObj_Anim*>(m_pPick)->Get_MaxState());

		ImGui::InputInt("CurrenState", (int*)static_cast<CObj_Anim*>(m_pPick)->Get_AnimState());
		ImGui::InputInt("ReserveState", (int*)static_cast<CObj_Anim*>(m_pPick)->Get_AnimReserveState());
		break;
	}

	
	


	if (m_bSetName)
	{
		ImGui::Begin("Set Name");
		ImGui::InputText("Name", m_szName, 260);
		if (ImGui::Button("Enter"))
		{
			(*m_vecAnim)[m_iSelectAnimList]->Set_Name(m_szName);
			Update_AnimList();
			m_bSetName = false;
		}
		ImGui::End();
	}
	
	if (ImGui::Button("Save"))
	{
		AnimSave();
	}
	ImGui::Separator();

	if (ImGui::Button("Load Channel"))
	{
		m_Channels.clear();
		m_ChannelKeyFrames = (*m_vecAnim)[m_iSelectAnimList]->Get_AllChannel();
		for (_uint i : m_ChannelKeyFrames)
		{
			m_Channels.push_back((*m_vecBones)[i]->Get_Name());
		}
		m_bLoadChannel = true;
	}

	if (m_bLoadChannel)
	{
		Get_KeyFrame();
	}
	if (ImGui::Button("Close"))
	{
		CObj_Tool::Get_Instance()->Set_Anim_Tool(false);
	}

	ImGui::End();
}

void CAnim_Tool::Get_AnimList(CGameObject * _pPick)
{
	//여기서 해당 모델의 애니메이션이나 뭐 등등 다 얻어오자
	m_pPick = _pPick;
	m_pModel = static_cast<CAnimModel*>(_pPick->Get_ComponentPtr(TEXT("Com_Model")));
	m_vecBones = m_pModel->Get_HierarchyNodeVector();
	m_vecAnim = m_pModel->Get_AllAnimation();
	m_pAnimList.clear();
	m_iSelectAnimList = 0;
	for (auto& _Anim : *m_vecAnim)
	{
		m_pAnimList.push_back(_Anim->Get_Name());
	}
	
	
}

void CAnim_Tool::Update_AnimList()
{
	m_pAnimList.clear();

	for (auto& _Anim : *m_vecAnim)
	{
		m_pAnimList.push_back(_Anim->Get_Name());
	}
}

void CAnim_Tool::AnimSave()
{
	TANIMMODEL _tModel = static_cast<CAnimModel*>(m_pPick->Get_ComponentPtr(TEXT("Com_Model")))->Get_ModelInfo();

	
	for (auto& _Anim : *m_vecAnim)
	{
		TANIM _tAnim = _Anim->Get_ForSave();
		_tchar*		_szFileName = nullptr;

		char szFullPath[MAX_PATH] = ""; //여기에 넣을 예정
		char szFileName[MAX_PATH] = ""; //파일 이름
		char szExt[MAX_PATH] = ""; //확장자

		_splitpath_s(_tModel.Name, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
		//strPath에 파일 경로 떼고 파일 이름이랑 확장자 뺀다
		// ex) abc / .png

		strcpy_s(szFullPath, _tModel.Path);
		//파일 경로 넣고
		strcat_s(szFullPath, "AnimList/");
		//파일 경로 넣고
		strcat_s(szFullPath, _tAnim.szName);
		//파일 이름 넣고
		strcat_s(szFullPath, ".dat");
		//경로 넣고

		_tchar			szWideFullPath[MAX_PATH] = TEXT("");

		MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);
		HANDLE		hFile = CreateFile(szWideFullPath,			// 파일 경로와 이름 명시
			GENERIC_WRITE,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
			NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
			NULL,						// 보안 속성, 기본값	
			CREATE_ALWAYS,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
			FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
			NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL
		
		DWORD		dwByte = 0;

		WriteFile(hFile, &(_tAnim.szName), sizeof(char) * 260, &dwByte, nullptr);
		WriteFile(hFile, &(_tAnim.fDuration), sizeof(float), &dwByte, nullptr);
		WriteFile(hFile, &(_tAnim.fTickPerSecond), sizeof(float), &dwByte, nullptr);
		WriteFile(hFile, &(_tAnim.fLimitTime), sizeof(float), &dwByte, nullptr);
		WriteFile(hFile, &(_tAnim.fMaxTermTime), sizeof(float), &dwByte, nullptr);
		WriteFile(hFile, &(_tAnim.NumChannel), sizeof(int), &dwByte, nullptr);
		for (int j = 0; j < _tAnim.NumChannel; ++j)
		{
			WriteFile(hFile, &(_tAnim.Bones[j]), sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &(_tAnim.Channels[j].NumKeyFrame), sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &(_tAnim.Channels[j].szName), sizeof(char) * 260, &dwByte, nullptr);
			for (int k = 0; k < _tAnim.Channels[j].NumKeyFrame; ++k)
			{
				WriteFile(hFile, &(_tAnim.Channels[j].KeyFrames[k]), sizeof(KEYFRAME), &dwByte, nullptr);
			}
		}
		CloseHandle(hFile);
	}
}

void CAnim_Tool::AddAnim()
{
	//이름 검색해서 먼저 찾고 그거 찾으면 그걸로 클론, 없으면 맵에 크리에이트 해서 추가하고 그걸로 클론
	CAnimModel* _pModel = static_cast<CAnimModel*>(m_pPick->Get_ComponentPtr(TEXT("Com_Model")));
	ImGui::Begin("AnimAdd");
	CAnimation* pAnimProto = nullptr;
	for (auto& iter : m_AnimProto)
	{
		if (strcmp(iter.first, m_szName))
		{
			pAnimProto = iter.second;
			break;
		}
	}
	if (pAnimProto == nullptr)
	{
		//이제 크리에이트
		TANIM _tAnim; //static CAnimation* Create(class CAnimModel * pModel, TANIM	_tIn, _uint _iAnimIndex);

		LoadAnim(&_tAnim, m_szName);

		pAnimProto = CAnimation::Create(_pModel,_tAnim, (*m_vecAnim).size());
		m_AnimProto.emplace(m_szName, pAnimProto);
	}

	CAnimation* clone = pAnimProto->Clone(_pModel);
	(*m_vecAnim).push_back(clone);
	

	ImGui::End();
}

void CAnim_Tool::LoadAnim(TANIM* _tAnim, char* _szName)
{
	TANIMMODEL _tModel = static_cast<CAnimModel*>(m_pPick->Get_ComponentPtr(TEXT("Com_Model")))->Get_ModelInfo();

	_tchar*		_szFileName = nullptr;

	char szFullPath[MAX_PATH] = ""; //여기에 넣을 예정
	char szExt[MAX_PATH] = ""; //확장자
	//strPath에 파일 경로 떼고 파일 이름이랑 확장자 뺀다
	// ex) abc / .png

	strcpy_s(szFullPath, _tModel.Path);
	//파일 경로 넣고
	strcat_s(szFullPath, "AnimList/");
	//파일 경로 넣고
	strcat_s(szFullPath, _szName);
	//파일 이름 넣고
	strcat_s(szFullPath, ".dat");

	_tchar			szWideFullPath[MAX_PATH] = TEXT("");

	MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);

	HANDLE		hFile = CreateFile(szWideFullPath,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	DWORD		dwByte = 0;
	ReadFile(hFile, &(_tAnim->szName), sizeof(char) * 260, &dwByte, nullptr);
	ReadFile(hFile, &(_tAnim->fDuration), sizeof(float), &dwByte, nullptr);
	ReadFile(hFile, &(_tAnim->fTickPerSecond), sizeof(float), &dwByte, nullptr);
	ReadFile(hFile, &(_tAnim->fLimitTime), sizeof(float), &dwByte, nullptr);
	ReadFile(hFile, &(_tAnim->fMaxTermTime), sizeof(float), &dwByte, nullptr);
	ReadFile(hFile, &(_tAnim->NumChannel), sizeof(int), &dwByte, nullptr);
	_tAnim->Bones = new int[_tAnim->NumChannel];
	_tAnim->Channels = new TCHANNEL[_tAnim->NumChannel];
	for (int j = 0; j < _tAnim->NumChannel; ++j)
	{
		ReadFile(hFile, &(_tAnim->Bones[j]), sizeof(int), &dwByte, nullptr);
		ReadFile(hFile, &(_tAnim->Channels[j].NumKeyFrame), sizeof(int), &dwByte, nullptr);
		ReadFile(hFile, &(_tAnim->Channels[j].szName), sizeof(char) * 260, &dwByte, nullptr);
		_tAnim->Channels[j].KeyFrames = new KEYFRAME[_tAnim->Channels[j].NumKeyFrame];
		for (int k = 0; k < _tAnim->Channels[j].NumKeyFrame; ++k)
		{
			ReadFile(hFile, &(_tAnim->Channels[j].KeyFrames[k]), sizeof(KEYFRAME), &dwByte, nullptr);
		}
	}
	CloseHandle(hFile);
}

void CAnim_Tool::Get_KeyFrame()
{
	if (ImGui::BeginCombo("Channel", m_Channels[m_iSelectChannelList], 1))
	{
		for (int i = 0; i < m_Channels.size(); ++i)
		{
			const bool isSelected = (m_iSelectChannelList == i);
			if (ImGui::Selectable(m_Channels[i], isSelected))
			{
				m_iSelectChannelList = i;
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Load KeyFrame"))
	{
		m_iKeyFrameSelect = 0;
		
		m_vecKeyFrame = ((*m_vecAnim)[m_iSelectAnimList]->Get_vecChannel())[m_ChannelKeyFrames[m_iSelectChannelList]]->Get_KeyFrameVector();
	}

	ImGui::InputFloat3("AllChange_+Pos", m_AllKeyFrameChage_Pos);
	ImGui::InputFloat3("AllChange_+Scale", m_AllKeyFrameChage_Scale);
	ImGui::InputFloat4("AllChange_+Rot", m_AllKeyFrameChage_Rot);

	if (ImGui::Button("All Change"))
	{
		_float3 _PlusPos, _PlusScale;
		_float4 _PlusRot;
		_PlusPos.x = m_AllKeyFrameChage_Pos[0];
		_PlusPos.y = m_AllKeyFrameChage_Pos[1];
		_PlusPos.z = m_AllKeyFrameChage_Pos[2];

		_PlusScale.x = m_AllKeyFrameChage_Scale[0];
		_PlusScale.y = m_AllKeyFrameChage_Scale[1];
		_PlusScale.z = m_AllKeyFrameChage_Scale[2];

		_PlusRot.x = m_AllKeyFrameChage_Rot[0];
		_PlusRot.y = m_AllKeyFrameChage_Rot[1];
		_PlusRot.z = m_AllKeyFrameChage_Rot[2];
		_PlusRot.w = m_AllKeyFrameChage_Rot[3];

		for (int i = 0 ; i < (*m_vecKeyFrame).size();++i)
		{
			XMStoreFloat3(&((*m_vecKeyFrame)[i].vPosition), XMLoadFloat3(&((*m_vecKeyFrame)[i].vPosition)) + XMLoadFloat3(&_PlusPos));
			XMStoreFloat3(&((*m_vecKeyFrame)[i].vScale), XMLoadFloat3(&((*m_vecKeyFrame)[i].vScale)) + XMLoadFloat3(&_PlusScale));
			XMStoreFloat4(&((*m_vecKeyFrame)[i].vRotation), XMLoadFloat4(&((*m_vecKeyFrame)[i].vRotation)) + XMLoadFloat4(&_PlusRot));
		}
		ZeroMemory(m_AllKeyFrameChage_Pos, sizeof(_float) * 3);
		ZeroMemory(m_AllKeyFrameChage_Scale, sizeof(_float) * 3);
		ZeroMemory(m_AllKeyFrameChage_Rot, sizeof(_float) * 4);
	}
	if (m_vecKeyFrame)
		ImGui::SliderInt("KeyFrame Index", &m_iKeyFrameSelect, 0, ((*m_vecKeyFrame).size() - 1));

	if (m_iKeyFrameSelect >= 0)
	{
		ImGui::InputFloat("Time", &((*m_vecKeyFrame)[m_iKeyFrameSelect].fTime));
		_float _vPos[3] = { (*m_vecKeyFrame)[m_iKeyFrameSelect].vPosition.x,
			(*m_vecKeyFrame)[m_iKeyFrameSelect].vPosition.y,
			(*m_vecKeyFrame)[m_iKeyFrameSelect].vPosition.z };
		ImGui::InputFloat3("Pos", _vPos);
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vPosition.x = _vPos[0];
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vPosition.y = _vPos[1];
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vPosition.z = _vPos[2];

		_float _vScale[3] = { (*m_vecKeyFrame)[m_iKeyFrameSelect].vScale.x,
			(*m_vecKeyFrame)[m_iKeyFrameSelect].vScale.y,
			(*m_vecKeyFrame)[m_iKeyFrameSelect].vScale.z };
		ImGui::InputFloat3("Scale", _vScale);
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vScale.x = _vScale[0];
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vScale.y = _vScale[1];
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vScale.z = _vScale[2];

		_float _vRot[4] = { (*m_vecKeyFrame)[m_iKeyFrameSelect].vRotation.x ,
			(*m_vecKeyFrame)[m_iKeyFrameSelect].vRotation.y,
			(*m_vecKeyFrame)[m_iKeyFrameSelect].vRotation.z,
			(*m_vecKeyFrame)[m_iKeyFrameSelect].vRotation.w
		};
		ImGui::InputFloat4("Rotation", _vRot);
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vRotation.x = _vRot[0];
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vRotation.y = _vRot[1];
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vRotation.z = _vRot[2];
		(*m_vecKeyFrame)[m_iKeyFrameSelect].vRotation.w = _vRot[3];
	}//KeyFrameSave()
	if (ImGui::Button("Save_KeyFrame"))
	{
		(*m_vecAnim)[m_iSelectAnimList]->KeyFrameSave();
	}
}

void CAnim_Tool::Free()
{
	for (auto iter : m_AnimProto)
	{
		Safe_Release(iter.second);
	}
	m_AnimProto.clear();
}
