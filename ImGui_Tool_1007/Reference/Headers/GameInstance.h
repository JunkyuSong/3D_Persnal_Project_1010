#pragma once
#include "AutoInstance.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "CollisionMgr.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "Picking.h"
#include "RandMgr.h"
#include "Target_Manager.h"
#include "Frustum.h"

/* 클라이언트로 보여주기위한 가장 대표적인 클래스이다. */
/* 각종 매니져클래스들의 주요함수를 클라로 보여준다.  */
/* 엔진초기화. */
/* 엔진정리. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(_uint iNumLevel, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Tick_Engine( _float fTimeDelta);
	void Clear(_uint iLevelIndex);
	

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();


public: /* For.Level_Manager */				
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	HRESULT Render_Level();
	HRESULT Loading_Level(const _uint & _iLoadingLevel, CLevel * _pLoadingLevel);
	HRESULT	Reserve_Level(const _uint& _iReserveLevel);

public: /* For.Object_Manager */
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr, class CGameObject** pOut = nullptr);
	class CLayer*	Get_Layer(_uint iLevelIndex, _tchar* _pLayerTag);
	map<const _tchar*, class CLayer*>	Get_Layers(_uint iLevelIndex);
	HRESULT Add_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public: /*For.Component_Manager*/
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	map<const _tchar*, class CComponent*>				Get_Map(_uint _iLevel);

public: /* for.Timer_Manager */
	_float Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT Add_Timer(const _tchar* pTimerTag);
	HRESULT Update_Timer(const _tchar* pTimerTag);
	HRESULT Set_TimeSpeed(const _tchar* pTimerTag, const _float& _fTimeSpeed);
	_float Get_TimeSpeed(const _tchar* pTimerTag);

public: /* For.Input_Device */
	_char Get_DIKState(_uchar eKeyID);
	_char Get_DIMKeyState(DIMK eMouseKeyID);
	_long Get_DIMMoveState(DIMM eMouseMoveID);
	_bool KeyDown(const _uchar& _eKeyID);
	_bool KeyPressing(const _uchar& _eKeyID);
	_bool KeyUp(const _uchar& _eKeyID);
	_bool MouseDown(const DIMK&	_eMouseKeyID);
	_bool MousePressing(const DIMK&	_eMouseKeyID);
	_bool MouseUp(const DIMK& _eMouseKeyID);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const;		
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4 Get_CamPosition() const;
	HRESULT Set_Player(CGameObject* _pPlayer);
	CGameObject* Get_Player();

public: /* For.Light_Manager */
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);

public:/* For. Random_Manager*/
	const _float &	Rand_Float(const _float & _fMin, const _float & _fMax);
	const _int &	Rand_Int(const _int & _iMin, const _int & _iMax);

public: /* For.Frustum */
	_bool isIn_Frustum_WorldSpace(_fvector vWorldPos, float fRadius = 0.f);

private:
	CGraphic_Device*				m_pGraphic_Device = nullptr;
	CInput_Device*					m_pInput_Device = nullptr;
	CLevel_Manager*					m_pLevel_Manager = nullptr;
	CObject_Manager*				m_pObject_Manager = nullptr;
	CComponent_Manager*				m_pComponent_Manager = nullptr;
	CTimer_Manager*					m_pTimer_Manager = nullptr;
	CPipeLine*						m_pPipeLine = nullptr;
	CLight_Manager*					m_pLight_Manager = nullptr;
	CCollisionMgr*					m_pCollision_Mgr = nullptr;
	CRandMgr*						m_pRand_Mgr = nullptr;
	CTarget_Manager*				m_pTarget_Manager = nullptr;
	CFrustum*						m_pFrustum = nullptr;

public:
	static void Release_Engine();
	virtual void Free() override;
};

END