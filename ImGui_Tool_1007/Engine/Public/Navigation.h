#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int			iCurrentIndex = -1;
	}NAVIGATIONDESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	_bool isMove(_fvector vPosition, _vector* vSlide);
	_float	Get_PosY(_vector vPos);
	void	Set_Index(_int _iIndex) { m_NavigationDesc.iCurrentIndex = _iIndex; }
	_int	Get_Index() { return m_NavigationDesc.iCurrentIndex; }
	_float	Get_DistanceToCell(_fvector vPosition);

#ifdef _DEBUG
public:
	HRESULT				Render();
	void				MakeCell(class CPointInCell* _p1, class CPointInCell* _p2, class CPointInCell* _p3);
	void				MakePoint(CPointInCell* _pPoint);
	void				DeleteCell(_uint _iIndex);
	void				DeletePoint(CPointInCell* _pPoint);
	class CPointInCell*	PickingPoint();
	class CCell*		PickingCell(_vector& _vPos);
	class CPointInCell*	FindPoint(_float3 _fPoints);
#endif // _DEBUG

private:
	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;

	NAVIGATIONDESC					m_NavigationDesc;

public:
	HRESULT Ready_Neighbor();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	vector<class CCell*>*	Get_CellsPtr() { return &m_Cells; }

#ifdef _DEBUG
private:
	list<class CPointInCell*>		m_Points;
#endif // _DEBUG
};

END 