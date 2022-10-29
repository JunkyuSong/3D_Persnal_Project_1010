#include "..\Public\PointInCell.h"
#include "Navigation.h"
#include "GameInstance.h"


CPointInCell::CPointInCell()
{
}

HRESULT CPointInCell::Initialize(_float3 _vPoint)
{
	m_vPoint = _vPoint;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(0.1f, 0.1f, 0.1f);
	ColliderDesc.vCenter = _vPoint/*_float3(0.f, 0.f, 0.f)*/;
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	AUTOINSTANCE(CGameInstance, _pInstance);
	m_pCollider = static_cast<CCollider*>(_pInstance->Clone_Component(3, TEXT("Prototype_Component_Collider_Sphere"), &ColliderDesc));

	if (m_pCollider == nullptr)
	{
		MSG_BOX(TEXT("failed point"));
	}
	

	return S_OK;
}

HRESULT CPointInCell::Render()
{
	m_pCollider->Render();
	return S_OK;
}

void CPointInCell::RepairPoint(_float3 _vPoint)
{
	m_vPoint = _vPoint;
	static_cast<CSphere*>(m_pCollider)->Get_Sphere()->Center = m_vPoint;
	//자기 자신을 공유하는 셀을 싹 돌아서 포인트를 바꿔주는 작업 <- 해당 셀의 abc를 체크? 그거 음. ㅇㅋ
	for (auto& _pCell : m_CellIs)
	{
		
		*(_pCell.first->Get_Points(_pCell.second)) = m_vPoint;
		_pCell.first->Set_Normal();
		_pCell.first->Mapping_Point();
	}
}

void CPointInCell::Add_Cell(CCell * _pCell, CCell::POINT _ePoint)
{
	pair<CCell *, CCell::POINT> _pair(_pCell, _ePoint);
	m_CellIs.push_back(_pair);
	Safe_AddRef(_pCell);
}

void CPointInCell::Delete_Point(CNavigation* _pNavi)
{
	//본인을 공유하는 셀들을 모두 삭제해야한다 -> 네비게이션 받아와야한다
	vector<class CCell*>* NaviCell = _pNavi->Get_CellsPtr();

	for (vector<class CCell*>::iterator& _pCellinNavi = (*NaviCell).begin(); _pCellinNavi != (*NaviCell).end();)
	{
		for (vector<pair<class CCell*, CCell::POINT>>::iterator& _pCell = m_CellIs.begin(); _pCell != m_CellIs.end();)
		{
			if (*_pCellinNavi == (_pCell->first))
			{
				Safe_Release(*_pCellinNavi);
				Safe_Release(_pCell->first);

				_pCellinNavi = (*NaviCell).erase(_pCellinNavi);
				_pCell = m_CellIs.erase(_pCell);
			}
			else
			{
				
				++_pCell;
			}
		}
		if (_pCellinNavi != (*NaviCell).end())
			++_pCellinNavi;
	}
}

void CPointInCell::Delete_Cell(CCell * _pCell)
{
	for (vector<pair<class CCell*, CCell::POINT>>::iterator& _Cell = m_CellIs.begin(); _Cell != m_CellIs.end();)
	{
		if (_pCell == (_Cell->first))
		{
			Safe_Release(_Cell->first);
			_Cell = m_CellIs.erase(_Cell);
		}
		else
		{
			++_Cell;
		}
	}
}

_bool CPointInCell::Picking()
{

	if(static_cast<CSphere*>(m_pCollider)->Picking())
		return true;
	return false;
}

CPointInCell * CPointInCell::Create(_float3 _vPoint)
{
	CPointInCell*			pInstance = new CPointInCell;

	if (FAILED(pInstance->Initialize(_vPoint)))
	{
		MSG_BOX(TEXT("Failed To Created : CCell"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPointInCell::Free()
{
	for (auto& _pCell : m_CellIs)
	{
		Safe_Release(_pCell.first);
	}
	Safe_Release(m_pCollider);
}
