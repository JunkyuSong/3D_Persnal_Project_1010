#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())	
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pCollision_Mgr(CCollisionMgr::Get_Instance())
	, m_pRand_Mgr(CRandMgr::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pFrustum(CFrustum::Get_Instance())
{	
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pCollision_Mgr);
	Safe_AddRef(m_pRand_Mgr);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustum);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* 엔진을 구동하기위한 초기화작업을 거친다. */
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 그래픽디바이스 초기화. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext)))
		return E_FAIL;

	/* 입력 초기화. */
	if (FAILED(m_pInput_Device->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 사운드 초기화. */


	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	CPicking::Get_Instance()->Initialize(GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, *ppDevice, *ppContext);

	return S_OK;
}

void CGameInstance::Tick_Engine( _float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pInput_Device)
		return;


	m_pCollision_Mgr->Clear_CollisoinList();

	m_pInput_Device->Update();
	m_pLevel_Manager->Tick(fTimeDelta);
	m_pObject_Manager->Tick(fTimeDelta);
	m_pFrustum->Tick();
	m_pCollision_Mgr->Tick();
	//m_pPipeLine->Update();
	CPicking::Get_Instance()->Tick();
	m_pObject_Manager->LateTick(fTimeDelta);


}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();	
}

HRESULT CGameInstance::Loading_Level(const _uint & _iLoadingLevel, CLevel * _pLoadingLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Loading_Level(_iLoadingLevel, _pLoadingLevel);
}

HRESULT CGameInstance::Reserve_Level(const _uint & _iReserveLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Reserve_Level(_iReserveLevel);
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObjectToLayer(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void * pArg, CGameObject** pOut)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(pPrototypeTag, iLevelIndex, pLayerTag, pArg, pOut);
}

CLayer * CGameInstance::Get_Layer(_uint iLevelIndex, _tchar * _pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Get_Layer(iLevelIndex, _pLayerTag);
}

map<const _tchar*, class CLayer*> CGameInstance::Get_Layers(_uint iLevelIndex)
{
	return m_pObject_Manager->Get_Layers(iLevelIndex);
}

HRESULT CGameInstance::Add_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	return m_pObject_Manager->Add_Layer(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);	
}

map<const _tchar*, class CComponent*> CGameInstance::Get_Map(_uint _iLevel)
{
	return m_pComponent_Manager->Get_Map(_iLevel);
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Update_Timer(pTimerTag);
}

HRESULT CGameInstance::Set_TimeSpeed(const _tchar * pTimerTag, const _float & _fTimeSpeed)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Set_TimeSpeed(pTimerTag, _fTimeSpeed);
}

_float CGameInstance::Get_TimeSpeed(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Get_TimeSpeed(pTimerTag);
}

_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKState(eKeyID);	
}

_char CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

_bool CGameInstance::KeyDown(const _uchar & _eKeyID)
{
	return m_pInput_Device->KeyDown(_eKeyID);
}

_bool CGameInstance::KeyPressing(const _uchar & _eKeyID)
{
	return m_pInput_Device->KeyPressing(_eKeyID);
}

_bool CGameInstance::KeyUp(const _uchar & _eKeyID)
{
	return m_pInput_Device->KeyUp(_eKeyID);
}

_bool CGameInstance::MouseDown(const DIMK & _eMouseKeyID)
{
	return m_pInput_Device->MouseDown(_eMouseKeyID);
}

_bool CGameInstance::MousePressing(const DIMK & _eMouseKeyID)
{
	return m_pInput_Device->MousePressing(_eMouseKeyID);
}

_bool CGameInstance::MouseUp(const DIMK & _eMouseKeyID)
{
	return m_pInput_Device->MouseUp(_eMouseKeyID);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eTransformState);	
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);
	
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eTransformState);
}

_float4 CGameInstance::Get_CamPosition() const
{
	if (nullptr == m_pPipeLine)
		return _float4();
	return m_pPipeLine->Get_CamPosition();
}

HRESULT CGameInstance::Set_Player(CGameObject * _pPlayer)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;
	m_pPipeLine->Set_Player(_pPlayer);
	return S_OK;
}

CGameObject * CGameInstance::Get_Player()
{
	if (nullptr == m_pPipeLine)
		return nullptr;
	return m_pPipeLine->Get_Player();
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc);
}

const _float & CGameInstance::Rand_Float(const _float & _fMin, const _float & _fMax)
{
	return m_pRand_Mgr->Rand_Float(_fMin, _fMax);
}

const _int & CGameInstance::Rand_Int(const _int & _iMin, const _int & _iMax)
{
	return m_pRand_Mgr->Rand_Int(_iMin, _iMax);
}

_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vWorldPos, float fRadius)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRadius);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CCollisionMgr::Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CTarget_Manager::Get_Instance()->Destroy_Instance();

	CPipeLine::Get_Instance()->Destroy_Instance();

	CLight_Manager::Destroy_Instance();

	CFrustum::Destroy_Instance();

	CInput_Device::Destroy_Instance();

	CPicking::Destroy_Instance();
	
	CGraphic_Device::Get_Instance()->Destroy_Instance();

	CRandMgr::Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pCollision_Mgr);
	Safe_Release(m_pRand_Mgr);
}
