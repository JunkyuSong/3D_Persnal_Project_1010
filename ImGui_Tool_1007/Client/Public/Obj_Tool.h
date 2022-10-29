#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Layer.h"
#include "Obj_Plus.h"
#include "Obj_NonAnim.h"
#include "Stage_01.h"
#include "Stage_Test.h"

class CObj_Tool :
	public CBase
{
	DECLARE_SINGLETON(CObj_Tool)

private:
	enum ADDTYPE {TYPE_ANIM, TYPE_NONANIM, TYPE_END};



public:
	CObj_Tool();
	virtual ~CObj_Tool();

	HRESULT Tool_Obj_Pick();
	HRESULT Tool_Obj_Add();

	void	Load_Layers();

	HRESULT	Key_Input();
	HRESULT Load_Model();

	void AnimModel_Save();
	void NonAnimModel_Save();


	virtual void Free() override;

	void Set_Anim_Tool(_bool _bAnim) { m_bAnim = _bAnim; }

	void Save_Map();
	void Load_Map();

public:
	HRESULT Tool_Obj();

private:
	void	ReserveLevel();

private:
	_float4x4			m_matWorld;
	_float3				m_vPos;
	_float3				m_vAngle;
	_float3				m_vScale;
	CLayer*				m_pLayer;
	CGameObject*		m_pPick;
	CTransform*			m_pPick_Trans;
	_int				m_iPickIndex;
	_float				_PressingSpeed = 0.2f;
	_float				_DownSpeed = 0.1f;

	vector<const char*> m_pModels;
	_uint				m_iSelectModel;

	_bool				m_bModels;

	_bool				m_bAddType;

private:
	vector<const char*> m_pLayers; 
	_uint				m_iSelectLayer;

	_bool				m_bLayer;

	_bool				m_bAnim = false;

private:
	CObj_Plus::OBJ_DESC		m_tObj_Desc;
	_bool					m_bTool_Obj_Pick;
	_bool					m_bTool_Obj_Add;
	char					m_szModelTag[256];
	char					m_szLayer[256];

	vector<const char*>		m_pLevels;
	_uint					m_iSelectLevel = 0;
		
private:
	map<_tchar*, CObj_Plus*>	m_mapToolProto;
	_bool							m_bSetting = false;

	CStage_Test*				m_pPickModel = nullptr;
};