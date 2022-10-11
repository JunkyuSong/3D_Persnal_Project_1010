#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCameraMgr :
	public CBase
{
	DECLARE_SINGLETON(CCameraMgr)
public:
	enum CAMERATYPE { CAMERA_PLAYER, CAMERA_FREE, CAMERA_END };

private:
	CCameraMgr();
	virtual ~CCameraMgr();

public:
	void Initialize();
	void Tick(_float fTimedelta);

public:
	void		Change_Camera(CAMERATYPE _eCameraType);
	CAMERATYPE	Get_CameraType() { return m_eCameraType; }
	

public:
	//void		Slow_Change();
	//void		Fast_Change();

private:
	CAMERATYPE			m_eCameraType = CAMERA_PLAYER;
	class CCamera*		m_pCameras[CAMERA_END];

	// CBase을(를) 통해 상속됨
	virtual void Free() override;
};

END