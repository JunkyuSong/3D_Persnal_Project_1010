#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "Saber.h"
#include "Dagger.h"
#include "BackGround.h"
#include "Terrain.h"
//#include "Monster.h"
//#include "Player.h"
//#include "Effect.h"
//#include "Sky.h"
#include "UI_Plus.h"
#include "Obj_Anim.h"
#include "Obj_NonAnim.h"
#include "Player.h"
#include "Trail.h"
#include "Magician.h"
#include "Cane.h"
#include "Cane_Sword.h"
#include "Axe.h"
#include "Knife.h"
#include "UI_PlayerHpBar.h"
#include "Card.h"
#include "Navigation.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CS());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다."));

	//CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	//Safe_AddRef(pGameInstance);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 개ㅑㄱ체원형 로드한다. */	

	/* For.Prototype_GameObject_BackGround */ 
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;	

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Plus"), CUI_Plus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* 텍스쳐를 로드한다. */

	/* For.Prototype_Component_Texture_Default */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Default.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Skill.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Inven"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven.png")))))
		return E_FAIL;

	//실제 UI 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Dialog"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/Dialog.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ItemBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/TexUI_ItemBackground.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SquareFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/TexUI_SquareFrame_Hover.png")))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Slot_Gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlagueWeapon/TexUI_PW_DefaultSlotFrame_02.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Slot_Green"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlagueWeapon/TexUI_PW_Frame_Active.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Slot_Claw"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlagueWeapon/TexUI_PW_PlunderSlotFrame_02.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_MiniSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlagueWeapon/TexUI_PW_DefaultSlotFrame.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlayerHPBar/TexUI_HPBar_1Border.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlayerHPBar/TexUI_HPBar_2BG.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlayerHPBar/TexUI_HPBar_3MainBar.png")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));	
	/* 모델를 로드한다. */



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CAutoInstance<CGameInstance> pGameInstance(CGameInstance::Get_Instance());

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다."));

	/* 개ㅑㄱ체원형 로드한다. */
	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), 
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_UI*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
	//	CUI::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Player*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Monster */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Obj_Anim"),
		CObj_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Obj_NonAnim"),
		CObj_NonAnim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Magician"),
		CMagician::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Saber"),
		CSaber::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Dagger"),
		CDagger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Cane"),
		CCane::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Cane_Sword"),
		CCane_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Axe"),
		CAxe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Knife"),
		CKnife::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PlayerHpBar"),
		CUI_PlayerHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Card"),
		CCard::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	///* For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* 텍스쳐를 로드한다. */
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/T_WoodTiles01_BC.dds")))))//Grass_%d.dds
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));
	/* 모델를 로드한다. */

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Trail"),
		CTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Magician"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Monster/", "Magician.fbx"))))
	//	return E_FAIL;
	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Player */

	/*PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Test"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Test/", "RL_Lower.fbx", PivotMatrix))))
		return E_FAIL;*/

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/", "player_test.dat", PivotMatrix))))
		return E_FAIL;
	
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Magician2"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "magician2.dat", PivotMatrix))))
		return E_FAIL;

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Magician"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "magician1_All.dat", PivotMatrix))))
		return E_FAIL;
	
	/*PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/", "Anim_Etc.fbx", PivotMatrix))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Saber"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Item/Weapon/Saber/", "Saber.dat"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Dagger"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Item/Weapon/Dagger/", "Dagger.dat"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Cane"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "Cane.dat"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Cane_Sword"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "Cane_Sword.dat"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Axe"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/SkillWeapon/", "Axe.dat"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Knife"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/SkillWeapon/", "Knife.dat"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Card"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "Card.dat"))))
		return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXTRAIL_DECLARATION::Elements, VTXTRAIL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
	//	CShader::Create(m_pGraphic_Device, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl")))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("충돌체를 로딩중입니다. "));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CAABB::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		COBB::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CSphere::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Capsule */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Capsule"),
		CCapsule::Create(m_pDevice, m_pContext, CCollider::TYPE_CAPSULE))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));

	/* For.Prototype_Component_Navigation */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/NavigationData.dat")))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Ampty"),
		CNavigation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_GamePlay"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navi_GamePlay.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);


}
