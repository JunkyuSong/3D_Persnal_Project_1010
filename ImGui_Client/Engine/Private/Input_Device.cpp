#include "..\Public\Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
{
}

HRESULT CInput_Device::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	/* m_pInputSDK */
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;


	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

void CInput_Device::Update()
{
	memcpy(&m_byPreKeyState, &m_byCurKeyState, sizeof(_char) * 256);
	m_pKeyboard->GetDeviceState(256, m_byCurKeyState);

	memcpy(&m_PreMouse, &m_MouseState, sizeof(DIMOUSESTATE));
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
}

void CInput_Device::Free()
{
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pMouse);

	Safe_Release(m_pInputSDK);
}
