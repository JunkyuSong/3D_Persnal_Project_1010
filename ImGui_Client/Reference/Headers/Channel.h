#pragma once
#include "Base.h"
#include "Engine_Binary.h"
BEGIN(Engine)

class ENGINE_DLL CChannel :
	public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(aiNodeAnim*	pAIChannel, TCHANNEL* _pOut);
	HRESULT Initialize(TCHANNEL _tIn);

	

public:
	//_bool Update_Transformation(_float _fCurPlayTime, class CAnimation* pNextAnim);
	//_bool Update_Transformation(_float _fCurPlayTime, class CHierarchyNode* pNode, _bool pOut);

	void Set_Index(_uint _iIndex) { m_iHierarchyNodeIndex = _iIndex; }
	_uint Get_Index() { return m_iHierarchyNodeIndex; }

	_uint Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode, KEYFRAME* _CurrenKeyFrame,_bool* pOut);
	
	//void Reset_KeyFrameIndex() { m_iCurrentKeyFrame = 0; }

	char* Get_Name() {
		return m_szName;
	}

	KEYFRAME& Get_KeyFrame(_uint Index) {
		return m_KeyFrames[Index];
	}

	KEYFRAME& Get_KeyFrameBack() {
		return m_KeyFrames.back();
	}

	KEYFRAME& Get_KeyFrameBackOne() {
		return m_KeyFrames[m_KeyFrames.size()-2];
	}

	/*KEYFRAME& Get_KeyFrameCurrent() {
		return m_iCurrentKeyFrame;
	}*/

public:
	static CChannel*	Create(aiNodeAnim*	pAIChannel, class CAnimModel* pModel, TCHANNEL* _pOut);
	static CChannel*	Create(TCHANNEL _tIn);
	virtual void Free() override;

private:
	char	m_szName[MAX_PATH];

	//class CHierarchyNode*			m_pHierarchyNode = nullptr;

	/* 키프레임 정보들를 로드한다. */
	/* 키프레임 : 전체애니메이션 동작 중, 특정 시간대에 이뼈가 표현해야할 동작의 상태 행렬정보이다. */
	_uint							m_iNumKeyFrames;
	vector<KEYFRAME>				m_KeyFrames;
	//KEYFRAME						m_iCurrentKeyFrame;

	_uint							m_iHierarchyNodeIndex=0;

public:
	vector<KEYFRAME>*				Get_KeyFrameVector() { return &m_KeyFrames; }
};

END