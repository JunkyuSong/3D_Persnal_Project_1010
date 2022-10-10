#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"
#include "AnimModel.h"
#include "NonAnimModel.h"

BEGIN(Engine)
class CAnimation;
class CHierarchyNode;
class CChannel;
END

BEGIN(Client)
class CAnim_Tool :
	public CBase
{
	DECLARE_SINGLETON(CAnim_Tool)
public:
	enum ANIM_TYPE { TYPE_PLAYER, TYPE_NONPLAYER };

private:
	enum HIERARCKY { NAME_ROOT = 3 };
public:
	CAnim_Tool();
	virtual ~CAnim_Tool();

public:
	void Tick();

	void Get_AnimList(CGameObject* _pPick);
	void Update_AnimList();

	void Anim_Type(ANIM_TYPE _eType) { m_eType = _eType; }
	ANIM_TYPE Anim_Type() { return m_eType; }
	
	void AnimSave();

	void AddAnim();

	void LoadAnim(TANIM* _tAnim, char* _szName);

	void Get_KeyFrame();

	// CBase을(를) 통해 상속됨
	virtual void Free() override;

private:
	vector<char*>				m_pAnimList;
	_int						m_iSelectAnimList = 0;

	vector<const char*>				m_Channels;
	_int						m_iSelectChannelList = 0;

	_int						m_iSwap=0;

	_bool						m_bLoadChannel = false;

private:
	CGameObject*				m_pPick = nullptr;
	CAnimModel*					m_pModel = nullptr;
	vector<CAnimation*>*		m_vecAnim = nullptr;
	_float						m_fDuration = 0.f;
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;
	vector<_uint>				m_ChannelKeyFrames;

	_bool						m_bSetName = false;
	char						m_szName[260];

	_bool						m_bAdd = false;
	map<char*,CAnimation*>	m_AnimProto ;


	vector<KEYFRAME>*	m_vecKeyFrame = nullptr;
	_int				m_iKeyFrameSelect = -1;
	vector<CHierarchyNode*>* m_vecBones = nullptr;

	_float						m_AllKeyFrameChage_Pos[3] = {0.f};
	_float						m_AllKeyFrameChage_Scale[3] = { 0.f };
	_float						m_AllKeyFrameChage_Rot[4] = { 0.f };

	ANIM_TYPE					m_eType = TYPE_PLAYER;
};

END