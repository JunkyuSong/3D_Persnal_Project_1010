#pragma once
#include "Base.h"
#include "Cell.h"
BEGIN(Engine)
class ENGINE_DLL CPointInCell :
	public CBase
{
private:
	CPointInCell();
	virtual ~CPointInCell() = default;

public:
	HRESULT				Initialize(_float3	_vPoint);
	HRESULT				Render();

public:
	_float3 Get_Point() { return m_vPoint; }

	//자기 자신을 공유하는 셀을 싹 돌아서 포인트를 바꿔주는 작업 <- 해당 셀의 abc를 체크? 그거 음. ㅇㅋ
	void RepairPoint(_float3 _vPoint);

	//셀포인터 넣어주는 작업
	void Add_Cell(CCell* _pCell, CCell::POINT _ePoint);

	//프리할때 자기 자신을 사용하는 셀들을 삭제하는 작업<-네비게이션 받아와서 처리
	// -> 그때 자신을 사용하는 이웃셀에서도 이웃을 취소해주는 작업? 아니면 셋 네이버 하면 그때 다시 이웃을 전부 세팅?
	// 같은 인덱스인 애 -1로 만들어주자
	void Delete_Point(class CNavigation* _pNavi);
	void Delete_Cell(CCell* _pCell);

	_bool Picking();

	void Set_Index(_int _iIndex)	{ m_iIndex = _iIndex; }
	_int Get_Index() { return m_iIndex; }
	
private:
	//본인을 공유하는 셀들의 인덱스를...
	//이 포인터 리스트는 네비게이션에서 들고있는다? 
	vector<pair<class CCell*, CCell::POINT>>		m_CellIs;
	//자기 자신의 데이터
	_float3											m_vPoint;
	//자신의 충돌체
	class CCollider*								m_pCollider = nullptr;

	_int											m_iIndex = -1;

public:
	static CPointInCell* Create(_float3	_vPoint);
	virtual void Free();
};

END