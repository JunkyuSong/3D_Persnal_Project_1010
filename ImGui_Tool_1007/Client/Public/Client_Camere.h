#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CClient_Camere abstract:
	public CCamera
{
public:
	enum CAMERATYPE {CAMERA_PLAYER, CAMERA_FREE, CAMERA_END};
protected:
	CClient_Camere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CClient_Camere(const CClient_Camere& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CClient_Camere() = default;

public:
	void		Set_CameraType(CAMERATYPE _eCameraType) { m_eCameraType = _eCameraType; }
	CAMERATYPE	Get_CameraType()						{ return m_eCameraType; }

public:
	virtual void Free() override;

protected:
	static CAMERATYPE m_eCameraType;

	//카메라 배열만들어서 현재 인덱스의 카메라만 틱 돌리게 만들면? 그럼 카메라 매니저가 훨씬 나은거같은데
	
};

END