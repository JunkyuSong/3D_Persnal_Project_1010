#pragma once
#include "Component.h"
#include "Client_Defines.h"
BEGIN(Client)

class CStatus final:
	public CComponent
{
public:
	typedef struct tagStatus // 플레이어, 몬스터, 무기 중복되는 요소들
	{
		_float	fMaxHp;
		_float	fHp;
		_float	fAttack;
	}STATUS;

protected:
	CStatus() = delete;
	CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatus(const CStatus& _rhs);
	virtual ~CStatus() = default;

public:
	static CStatus*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*			Clone(void* _pArg);
	virtual void				Free() override;

	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

//기능 시작
public:
	inline _bool				Damage(const _float _iTargetAtt)
	{
		m_fHp -= _iTargetAtt;
		if (m_fHp < 0)
		{
			m_fHp = 0;
			return false;
		}
		return true;
	}
	inline _bool				Heal(const _float _iHeal)
	{
		m_fHp += _iHeal;
		if (m_fHp > m_fMaxHp)
		{
			m_fHp = m_fMaxHp;
			return false;
		}
		return true;
	}
	inline const _float&			Get_Hp() { return m_fHp; };
	inline void						Set_MaxHp(const _float _fHp)
	{
		m_fMaxHp = _fHp;
		m_fHp = m_fMaxHp;
	}
	inline  _float					Get_Attack() 						{ return m_fAttack; }
	inline void						Set_Attack(const _float _fAttack)	{ m_fAttack = _fAttack; }

	inline const _float&			Get_MaxHp() const					{ return m_fMaxHp; }
	inline const _float&			Get_Hp() const						{ return m_fHp; }
	
protected:
	_float	m_fHp;
	_float	m_fAttack;
	_float	m_fMaxHp;
};

END