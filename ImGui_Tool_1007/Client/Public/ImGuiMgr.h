#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Layer.h"
#include "UI_Plus.h"

BEGIN(Client)
class CImGui :
	public CBase
{
	DECLARE_SINGLETON(CImGui)
	
private:
	enum TOOL_TYPE { TOOL_UI, TOOL_OBJ, TOOL_NAVI, TOOL_END };

private:
	CImGui();
	virtual ~CImGui();

public:
	HRESULT				Initailize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;

	_uint				Tick();
	HRESULT				Render();

	void				ClientTick();

	_tchar*				ConvertCtoWC(const char * str);
	char*				ConvertWCtoC(const _tchar * _tchar);

public:
	void				Begin(const char* Label);
	void				Intcheck(_int* _iInt, const char* Label);
	void				floatcheck(_float* _fFloat, const char* Label);
	_bool				Button(const char* Label);
	void				End();

private:
	HRESULT				Key_Input();

	HRESULT				Save_Tag(const _uint _iLevel,const _tchar * _pLayerTag);
	HRESULT				Load_Tag(const _uint _iLevel, const _tchar * _pLayerTag);

	HRESULT				Load_Texture();

	HRESULT				Tool_UI();
	HRESULT				Tool_UI_Pick();
	HRESULT				Tool_UI_Add();

	_bool				OnMouse();

private:
	TOOL_TYPE			m_eCurToolType;
	_bool				m_bToolOn;
	_bool				show_demo_window;

private: // Whole
	_float3				m_vPos;
	_float3				m_vScale;
	CLayer*				m_pLayer;
	CGameObject*		m_pPick;
	CTransform*			m_pPick_Trans;
	_int				m_iPickIndex;
	_float				_PressingSpeed = 0.2f;
	_float				_DownSpeed = 0.1f;

	vector<const char*> m_pTextures;
	_uint				m_iSelectTex;

	_bool				m_bTexture;

private: // UI_Tool
	CUI_Plus::UI_DESC	m_tUI_Desc;
	_bool				m_bTool_UI_Pick;
	_bool				m_bTool_UI_Add;
	char				m_szTemp[256];
	char				m_szSour[256];


};

END