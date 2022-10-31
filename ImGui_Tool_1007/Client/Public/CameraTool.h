#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Camera_Free.h"
#include "LookAtCube.h"
#include "BaseTile.h"
#include "MoveCube.h"

BEGIN(Client)
class CPosCube;
class CLookCube;

class CCameraTool final : public CBase
{
public:
	CCameraTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCameraTool() = default;

public:
	void Initalize();
#pragma region Camera
public:
	void ShowCameraWindow(_float fTimeDelta);
	void SetCameraFree(CCamera_Free* pCamera) { m_pCamera = pCamera; Safe_AddRef(m_pCamera); }
	void SetLookAtCube(CLookAtCube* pLookAt) { m_pLookAt = pLookAt; Safe_AddRef(m_pLookAt); }
	void AddMoveCube(CMoveCube* m_MoveCube) { m_pMoveCube = m_MoveCube; Safe_AddRef(m_MoveCube); }
	void CameraKeyInput();
	void SaveScene(char* FileName);
	void LoadScene(char* FileName);
	void SaveSceneList();
	void LoadSceneList();
	bool Find_SceneList(const char* ListName);
	void FixMoveCube();
	void Set_MoveCubePos(_float3 vPos);
private:
	_float m_fMoveCubeX = 0.f;
	_float m_fMoveCubeY = 0.f;
	_float m_fMoveCubeZ = 0.f;
	string m_sNowScene;
	vector<const char*> m_SceneList;
	int m_iSelectedSceneList = 0;


	CCamera_Free* m_pCamera = nullptr;
	CMoveCube* m_pMoveCube = nullptr;
	CLookAtCube* m_pLookAt = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	
#pragma endregion Camera

#pragma region CameraEdit
public:
	void ShowCameraEditWindow(_float fTimeDelta);
	void AddPosCube(CPosCube* pPos) { m_PosCube.push_back(pPos); }
	void AddLookCube(CLookCube* pLook) { m_LookCube.push_back(pLook); }
	int GetPosIndex() { return m_iSelectedPosCube; }
	int GetLookIndex() { return m_iSelectedLookCube; }
	bool GetPosEdit() { return m_bPosEdit; }
	bool GetLookEdit() { return m_bLookEdit; }
	bool GetPlay() { return m_bPlay; }
	void DeleteCube();
	void SortCube();

private:
	void PlayScene(_float fTimeDelta);
	int m_iSelectedPosCube = 0;
	int m_iSelectedLookCube = 0;
	int m_iPosCubeIndex = 0;
	int m_iLookCubeIndex = 0;
	int m_iNowPosCube = 0;
	int m_iNowLookCube = 0;
	bool m_bPosEdit = true;
	bool m_bLookEdit = false;
	bool m_bPlay = false;
	bool m_bLookPlay = false;
	bool m_bStop = true;
	void DataFix();

	vector<const char*> m_PosCubeName;
	vector<const char*> m_LookCubeName;
	vector<CPosCube*> m_PosCube;
	vector<CLookCube*> m_LookCube;
#pragma endregion CameraEdit
public:
	virtual void Free() override;
};

END

