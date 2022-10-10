#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	virtual void			Free();

public:
	_char						Get_DIKState(const _uchar&	_eKeyID)
	{
		return m_byCurKeyState[_eKeyID];
	}

	_bool						KeyDown(const _uchar& _eKeyID) // 이전에 안누르고 현재 눌렀을때 딱한번 실행
	{
		if (!(m_byPreKeyState[_eKeyID] & 0X80) && (m_byCurKeyState[_eKeyID] & 0X80))
		{
			return true;
		}
		return false;
	}

	_bool						KeyPressing(const _uchar& _eKeyID) // 연속 실행
	{
		if ((m_byPreKeyState[_eKeyID] & 0X80) && (m_byCurKeyState[_eKeyID] & 0X80))
		{
			return true;
		}
		return false;
	}



	_bool						KeyUp(const _uchar& _eKeyID)
	{
		if ((m_byPreKeyState[_eKeyID] & 0X80) && !(m_byCurKeyState[_eKeyID] & 0X80))
		{
			return true;
		}
		return false;
	}


	_bool						MouseDown(const DIMK&	_eMouseKeyID) // 이전에 안누르고 현재 눌렀을때 딱한번 실행
	{
		if (!(m_PreMouse.rgbButtons[_eMouseKeyID]) && (m_MouseState.rgbButtons[_eMouseKeyID]))
		{
			{
				return true;
			}
		}
		return false;
	}

	_bool						MousePressing(const DIMK&	_eMouseKeyID) // 연속 실행
	{
		if ((m_PreMouse.rgbButtons[_eMouseKeyID]) && (m_MouseState.rgbButtons[_eMouseKeyID]))
		{
			return true;
		}
		return false;
	}

	_bool						MouseUp(const DIMK&	_eMouseKeyID) //이전에 누르고 뗐을때 실행
	{
		if ((m_PreMouse.rgbButtons[_eMouseKeyID]) && !(m_MouseState.rgbButtons[_eMouseKeyID]))
		{
			return true;
		}
		return false;
	}

	_char						Get_DIMKeyState(const DIMK&	_eMouseKeyID)
	{
		return m_MouseState.rgbButtons[_eMouseKeyID];
	}

	_long						Get_DIMMoveState(const DIMM&	_eMouseMoveID)
	{
		return ((_long*)&m_MouseState)[_eMouseMoveID];
	}

public:
	HRESULT						Initialize(HINSTANCE _hInstance, HWND _hWnd);
	void						Update();

private:
	LPDIRECTINPUT8				m_pInputSDK;
	LPDIRECTINPUTDEVICE8		m_pKeyboard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

	_char						m_byPreKeyState[256];
	_char						m_byCurKeyState[256];
	
	DIMOUSESTATE				m_MouseState;
	DIMOUSESTATE				m_PreMouse;
};

END

/*
typedef struct _DIMOUSESTATE {
LONG    lX;
LONG    lY;
LONG    lZ;
BYTE    rgbButtons[4];
} DIMOUSESTATE, *LPDIMOUSESTATE;

enum DIMK { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_X, DIMK_END };
enum DIMM { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };
*/

/*
DIRECT INPUT은 DIK로 시작

DIK_ESCAPE  - ESC키
DIK_0 ~ 9       - 키보드 상단의 숫자키 0 부터 9 까지
DIK_NUMPAD0 ~ 9 - 키보드 오른쪽에 있는 숫자 키패드 0 부터 9 까지
DIK_A ~ Z      - A 부터 Z 까지의 문자 키
DIK_RETURN  - Enter 키
DIK_LCONTROL - 왼쪽 Ctrl 키
DIK_RCONTROL - 오른쪽 Ctrl 키
DIK_SPACE -  스페이스 바
DIK_F1 ~ F12 - 키보드 상단 F1 부터 F12까지 펑션 키
DIK_UP        - 위 화살표 키
DIK_DOWN  - 아래 화살표 키
DIK_LEFT    - 왼쪽 화살표 키
DIK_RIGHT  - 오른쪽 화살표 키
DIK_TAB     - Tab 키
DIK_NUMPADENTER -  숫자 키패드 Enter 키
DIK_PRIOR - Page Up 키
DIK_NEXT - Page Down 키
*/