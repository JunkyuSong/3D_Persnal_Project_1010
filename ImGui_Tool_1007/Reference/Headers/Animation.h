#pragma once
#include "Base.h"

#include "Engine_Binary.h"

BEGIN(Engine)
class ENGINE_DLL CAnimation :
	public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(aiAnimation* pAIAnimation, class CAnimModel * pModel, _uint _iAnimIndex);
	HRESULT Initialize_Prototype(class CAnimModel * pModel, TANIM	_tIn, _uint _iAnimIndex);
	HRESULT Initialize(class CAnimModel* pModel);

	_bool	Play_Animation(_float fTimeDelta, _float* _pOut);
	_bool	Play_Animation(_float fTimeDelta, CAnimation* pNextAnim, set<_uint>& _TotalChannel);

	void   Update_Transformation(KEYFRAME& Sour, KEYFRAME& Dest, const _uint& _iHierarchyNodeIndex);

public:
	class CChannel*		Get_Channel(const char* szChannel);
	class CChannel*		Get_Channel_Index(_uint _iIndex);
	_uint				Get_Id() { return m_iAnimIndex; }
	vector<_uint>		Get_AllChannel() { return m_ChannelIndex; }
	vector<class CChannel*>		Get_vecChannel() {
		return m_vecChannels;
	}

	const _float&		Get_Limit() {
		return m_fLimitTime;
	}

	void Reset_KeyFrame();




public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CAnimModel* pModel, _uint _iAnimIndex);
	static CAnimation* Create(class CAnimModel * pModel, TANIM	_tIn, _uint _iAnimIndex);
	CAnimation* Clone(class CAnimModel* pModel);
	virtual void Free() override;

public:
	//_bool Update_Channel(_uint _iChannelIndex, _uint _iFrameIndex);
	//_bool Update_Channel(_uint _iChannelIndex, _uint _iFrameIndex, CAnimation* pNextAnim);

	KEYFRAME						Get_OldKeyFrame(_uint _iIndex) { return  m_ChannelOldKeyFrames[_iIndex]; }

private:
	_bool							m_isClone;
	TANIM							m_tAnim;
	char							m_Name[260];
	_uint							m_iAnimIndex;
	_float							m_fDuration = 0.f;
	_float							m_fTickPerSecond = 0.f;

	_float							m_fLimitTime = 20.f;
	_float							m_fPlayTime = 0.f;
	_float							m_fTermTime = 0.f;
	_float							m_fMaxTermTime = 0.2f;

	vector<_uint>					m_ChannelKeyFrames;
	vector<class CChannel*>			m_vecChannels;

	vector<_uint>					m_ChannelIndex;//ª¿ ¿Œµ¶Ω∫ µÈ∞Ì¿÷¿Ω
	vector<class CHierarchyNode*>*	m_HierarchyNodes = nullptr;

	_uint							m_iNumChannels;


	_bool							m_bAnimEnd;

	vector<KEYFRAME>				m_ChannelOldKeyFrames;
public://≈¯øÎ
	TANIM	Get_ForSave();
	void	KeyFrameSave();

	char*							Get_Name() { return m_Name; }
	void							Set_Name(char* _Name) { strcpy_s(m_Name, _Name); }
	vector<_uint>*					Get_KeyFrames() {
		return &m_ChannelKeyFrames;
	}

	_float*							Get_Duration() { return &m_fDuration; }
	_float*							Get_PlayTime() { return &m_fPlayTime; }
	_float*							Get_TickPerSecond() { return &m_fTickPerSecond; }
	_float*							Get_LimitTime() { return &m_fLimitTime; }
};

END