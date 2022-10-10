#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Layer.h"
#include "Obj_Plus.h"

class CUI_Tool :
	public CBase
{
	DECLARE_SINGLETON(CUI_Tool)
public:
	CUI_Tool();
	virtual ~CUI_Tool();


	virtual void Free() override;

private:

};