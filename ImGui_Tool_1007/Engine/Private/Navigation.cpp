#include "..\Public\Navigation.h"
#include "Cell.h"
#include "PointInCell.h"

#include "Picking.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
	, m_NavigationDesc(rhs.m_NavigationDesc)
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const _tchar * pNavigationDataFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3			vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*			pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(Ready_Neighbor()))
		return E_FAIL;

	// m_Cells[1]->Set_NeighborIndex(CCell::LINE_AB, nullptr);

	return S_OK;
}

HRESULT CNavigation::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	
	if(nullptr != pArg)
		memcpy(&m_NavigationDesc, pArg, sizeof(NAVIGATIONDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	return S_OK;
}

_bool CNavigation::isMove(_fvector vPosition, _vector* vSlide)
{
	_int	iNeighborIndex = -1;

	/* 현재 쎌 안에서 움직였다. */
	/* 나간방향에 이웃이 있다면. 이웃의 인ㄷ게스를 받아오고. 
	이웃이 없다면 안채워온다. */
	if (true == m_Cells[m_NavigationDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex, vSlide))
		return true;

	/* 현재 셀을 나갔다. */
	else
	{
		/* 나간방향에 이웃이 있었다면. */
		if (0 <= iNeighborIndex)
		{
			while (true)
			{
				if (0 > iNeighborIndex)
					return false;	


				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex, vSlide))
				{
					/* 커런트 인덱스를 이웃의 인덱스로 바꿔준다. */
					m_NavigationDesc.iCurrentIndex = iNeighborIndex;

					return true;
				}
			}
			
		}

		/* 나간방향에 이웃이 없었다면. */
		else
			return false;


	}
	

	return true;
}

_float CNavigation::Get_PosY(_vector vPos)
{
	_vector PointA = XMVectorSetW(XMLoadFloat3(&m_Cells[m_NavigationDesc.iCurrentIndex]->Get_Point(CCell::POINT_A)), 1);
	_vector PointB = XMVectorSetW(XMLoadFloat3(&m_Cells[m_NavigationDesc.iCurrentIndex]->Get_Point(CCell::POINT_B)), 1);
	_vector PointC = XMVectorSetW(XMLoadFloat3(&m_Cells[m_NavigationDesc.iCurrentIndex]->Get_Point(CCell::POINT_C)), 1);

	_vector Plane = XMPlaneFromPoints(PointA, PointB, PointC);

	_float x = XMVectorGetX(vPos);
	_float z = XMVectorGetZ(vPos);

	_float a = XMVectorGetX(Plane);
	_float b = XMVectorGetY(Plane);
	_float c = XMVectorGetZ(Plane);
	_float d = XMVectorGetW(Plane);

	/* y = (-ax - cz - d) / b */

	return (-a * x - c * z - d) / b;
}

_float CNavigation::Get_DistanceToCell(_fvector vPosition)
{
	if (isMove(vPosition, nullptr))
	{
		
		_vector PointA = XMVectorSetW(XMLoadFloat3(&m_Cells[m_NavigationDesc.iCurrentIndex]->Get_Point(CCell::POINT_A)), 1);
		_vector PointB = XMVectorSetW(XMLoadFloat3(&m_Cells[m_NavigationDesc.iCurrentIndex]->Get_Point(CCell::POINT_B)), 1);
		_vector PointC = XMVectorSetW(XMLoadFloat3(&m_Cells[m_NavigationDesc.iCurrentIndex]->Get_Point(CCell::POINT_C)), 1);

		_vector Plane = XMPlaneFromPoints(PointA, PointB, PointC);
		
		_float x = XMVectorGetX(vPosition);
		_float y = XMVectorGetY(vPosition);
		_float z = XMVectorGetZ(vPosition);
		
		_float a = XMVectorGetX(Plane);
		_float b = XMVectorGetY(Plane);
		_float c = XMVectorGetZ(Plane);
		_float d = -(a*x + y*b + c*z);
		//XMVectorGetW(Plane);

		/* y = (-ax - cz - d) / b */
		//0 = ax + yb + cz + d;

		return d;
	}
	return 1.f;
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (-1 == m_NavigationDesc.iCurrentIndex)
	{
		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render_Cell();
		}
	}
	else
		m_Cells[m_NavigationDesc.iCurrentIndex]->Render_Cell(0.05f, _float4(1.f, 0.f, 0.f, 1.f));
	
	for (auto& _pPoint : m_Points)
	{
		_pPoint->Render();
	}
	return S_OK;
}

void CNavigation::MakeCell(CPointInCell* _p1, CPointInCell* _p2, CPointInCell* _p3)
{
	vector<CPointInCell*> _Temp;
	_Temp.push_back(_p1);
	_Temp.push_back(_p2);
	_Temp.push_back(_p3);
	_float3 _vPoints[CCell::POINT_END];
	_vPoints[CCell::POINT_A] = _p1->Get_Point();
	_vPoints[CCell::POINT_B] = _p2->Get_Point();
	_vPoints[CCell::POINT_C] = _p3->Get_Point();

	CCell*			pCell = CCell::Create(m_pDevice, m_pContext, _vPoints, m_Cells.size());
	if (nullptr == pCell)
		return;
	
	//인덱스 맞는지 다시 확인
	_vPoints[CCell::POINT_A] = pCell->Get_Point(CCell::POINT_A);
	_vPoints[CCell::POINT_B] = pCell->Get_Point(CCell::POINT_B);
	_vPoints[CCell::POINT_C] = pCell->Get_Point(CCell::POINT_C);

	for (auto& _p : _Temp)
	{
		if (_vPoints[CCell::POINT_A].x == _p->Get_Point().x
			&&_vPoints[CCell::POINT_A].y == _p->Get_Point().y
			&&_vPoints[CCell::POINT_A].z == _p->Get_Point().z)
		{
			_p->Add_Cell(pCell, CCell::POINT_A);
			pCell->Set_PointIndex(CCell::POINT_A, _p->Get_Index());
		}
		else
		{
			if (_vPoints[CCell::POINT_B].x == _p->Get_Point().x
				&&_vPoints[CCell::POINT_B].y == _p->Get_Point().y
				&&_vPoints[CCell::POINT_B].z == _p->Get_Point().z)
			{
				_p->Add_Cell(pCell, CCell::POINT_B);
				pCell->Set_PointIndex(CCell::POINT_B, _p->Get_Index());
			}
			else
			{
				_p->Add_Cell(pCell, CCell::POINT_C);
				pCell->Set_PointIndex(CCell::POINT_C, _p->Get_Index());
			}
		}
	}

	m_Cells.push_back(pCell);
}

void CNavigation::MakePoint(CPointInCell* _pPoint)
{
	_pPoint->Set_Index(m_Points.size());
	m_Points.push_back(_pPoint);
}

void CNavigation::DeleteCell(_uint _iIndex)
{
	_int* _iIndiecs = m_Cells[_iIndex]->Get_PointIndex();

	for (_int i = 0; i < 3; ++i)
	{
		for (auto& _pPoint : m_Points)
		{
			if (_iIndiecs[i] == _pPoint->Get_Index())
			{
				_pPoint->Delete_Cell(m_Cells[_iIndex]);
			}
		}
	}
	auto& iter = m_Cells.begin();
	for (_int i = 0; i <= _iIndex; ++iter, ++i);
	Safe_Release(*iter);
	m_Cells.erase(iter);
}

void CNavigation::DeletePoint(CPointInCell * _pPoint)
{
	
	for (list<class CPointInCell*>::iterator& iter = m_Points.begin(); iter != m_Points.end();)
	{
		if (*iter == _pPoint)
		{
			_pPoint->Delete_Point(this);
			Safe_Release(*iter);
			m_Points.erase(iter);
			return;
		}
		else
		{
			++iter;
		}
	}
}

CPointInCell* CNavigation::PickingPoint()
{
	for (auto& _point : m_Points)
	{
		if (_point->Picking())
		{
			return _point; 
		}
	}
	return nullptr;
}

CCell * CNavigation::PickingCell(_vector& _vPos)
{
	CPicking*		pPicking = CPicking::Get_Instance();
	_bool _bPicking = false;
	Safe_AddRef(pPicking);

	//_matrix			WorldMatrixInv = pTransform->Get_WorldMatrixInverse();

	_vector			vRayDir, vRayPos;

	pPicking->Compute_LocalRayInfo(vRayDir, vRayPos, nullptr);
	vRayDir = XMVector3Normalize(vRayDir);

	_float		fDist;
	for (auto _Cell : m_Cells)
	{
		_float3	_vPoints[3];
		_vPoints[0] = _Cell->Get_Point(CCell::POINT_A);
		_vPoints[1] = _Cell->Get_Point(CCell::POINT_B);
		_vPoints[2] = _Cell->Get_Point(CCell::POINT_C);

		if (true == TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&(_vPoints[0])) , XMLoadFloat3(&(_vPoints[1])), XMLoadFloat3(&(_vPoints[2])), fDist))
		{
			_vector	vPickPos = vRayPos + (vRayDir * fDist);
			_vPos = vPickPos;
			Safe_Release(pPicking);
			return _Cell;
		}
	}
	Safe_Release(pPicking);
	return nullptr;
}

CPointInCell * CNavigation::FindPoint(_float3 _fPoints)
{
	for (auto& _Point : m_Points)
	{
		_float3 _fPointCell_Points = _Point->Get_Point();
		if (_fPointCell_Points.x == _fPoints.x)
		{
			if (_fPointCell_Points.y == _fPoints.y)
			{
				return _Point;
			}
		}
		else if (_fPointCell_Points.x == _fPoints.y)
		{
			if (_fPointCell_Points.z == _fPoints.x)
			{
				return _Point;
			}
		}
		else if (_fPointCell_Points.x == _fPoints.z)
		{
			if (_fPointCell_Points.y == _fPoints.x)
			{
				return _Point;
			}
		}
	}

	return nullptr;
}

#endif // _DEBUG

HRESULT CNavigation::Ready_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				// 너(Dest)는 내(Sour) AB이웃. 
				pSourCell->Set_NeighborIndex(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{				
				pSourCell->Set_NeighborIndex(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_CA, pDestCell);
			}
		}
	}


	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationDataFilePath)
{
	CNavigation*			pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFilePath)))
	{
		MSG_BOX(TEXT("Failed To Created : CNavigation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNavigation*			pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNavigation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*			pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNavigation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();

	for (auto& pPoint : m_Points)
		Safe_Release(pPoint);
	m_Points.clear();
}
