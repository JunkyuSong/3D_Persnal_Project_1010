#pragma once
#include "Base.h"

//넣을 기능 : 텍스쳐 이름 받아오고
// 그럼 텍스쳐가 어디에 들어가있는지 알 수 있겠지
// 텍스쳐 1번에 가있는 애들 0번으로 바꿀 수 있도록
// 얘는 OBJ툴에서 pick을 가져오고
// 


BEGIN(Engine)
class CTexture_Tool final :
	public CBase
{
	DECLARE_SINGLETON(CTexture_Tool)
private:
	CTexture_Tool();
	virtual ~CTexture_Tool();



	// CBase을(를) 통해 상속됨
public:
	virtual void Free() override;

};

END