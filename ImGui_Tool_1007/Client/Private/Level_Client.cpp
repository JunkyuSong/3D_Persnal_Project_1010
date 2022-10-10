#include "stdafx.h"
#include "..\Public\Level_Client.h"

#include "GameInstance.h"
#include "GameObject.h"

CLevel_Client::CLevel_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{	
}
void CLevel_Client::NowLevel(_uint _CurLevel)
{
	g_eCurLevel = LEVEL(_CurLevel);
	
	return;
}
HRESULT CLevel_Client::Loading(const LEVEL& _eLevel)
{
	//CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	//_tchar* _szLevel = nullptr;
	//switch (_eLevel)
	//{
	//case LEVEL_GAMEPLAY:
	//	//_szLevel = TEXT("../Bin/Data/Gameplay.dat");
	//	break;
	//default:
	//	break;
	//}

	//HANDLE		hFile = CreateFile(_szLevel,
	//	GENERIC_READ,
	//	NULL,
	//	NULL,
	//	OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL,
	//	NULL);


	//// 2. 파일 쓰기

	//DWORD		dwByte = 0;

	//CGameObj::GAMEOBJ_INFO _tInfo;
	//_uint _iNum = 0;
	//while (true)
	//{
	//	_tchar _szTemp[128] = {};
	//	memset(&_szTemp, NULL, sizeof(_tchar) * 128);

	//	ReadFile(hFile, &_szTemp, 128, &dwByte, nullptr);

	//	_tchar* _szProto;
	//	
	//	_szProto = _szTemp;
	//	ReadFile(hFile, &(_tInfo.iLevel), sizeof(_int), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.iStageNum), sizeof(_int), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.fReverse), sizeof(_float), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.fScaleX), sizeof(_float), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.fScaleY), sizeof(_float), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.fScaleZ), sizeof(_float), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.vPos), sizeof(_float3), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.iTextureNumber), sizeof(_int), &dwByte, nullptr);
	//	
	//	if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
	//	{
	//		break;
	//	}

	//	_tInfo.vWorldPos = _tInfo.vPos + CStageMgr::Get_Instance()->Get_StageInfo(_eLevel, _tInfo.iStageNum);



	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(_szProto, _eLevel, TEXT("Layer_MapObj"), &_tInfo)))
	//	{
	//		MSG_BOX(TEXT("Load Error"));
	//		return E_FAIL;
	//	}

	//	++_iNum;
	//}

	//// 3. 파일 소멸
	//CloseHandle(hFile);
	return S_OK;
}

