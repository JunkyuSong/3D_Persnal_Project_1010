#include "..\Public\Animation.h"
#include "AnimModel.h"
#include "Channel.h"

#include "HierarchyNode.h"

CAnimation::CAnimation()
	: m_iAnimIndex(0)
	, m_bAnimEnd(false)
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_vecChannels(rhs.m_vecChannels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
	, m_iAnimIndex(rhs.m_iAnimIndex)
	, m_bAnimEnd(false)
	, m_HierarchyNodes(nullptr)
	, m_fLimitTime(rhs.m_fLimitTime)
	, m_ChannelKeyFrames(rhs.m_ChannelKeyFrames)
	, m_tAnim(rhs.m_tAnim)
	, m_ChannelOldKeyFrames(rhs.m_ChannelOldKeyFrames)
	, m_ChannelIndex(rhs.m_ChannelIndex)
{
	for (auto& pChannel : m_vecChannels)
		Safe_AddRef(pChannel);
	strcpy_s(m_Name, rhs.m_Name);
	strcpy_s(m_tAnim.szName, m_Name);
}

HRESULT CAnimation::Initialize_Prototype(aiAnimation * pAIAnimation, CAnimModel * pModel, _uint _iAnimIndex)
{
	m_isClone = false;
	ZeroMemory(&m_tAnim, sizeof(TANIM));
	strcpy_s(m_Name, pAIAnimation->mName.data);
	/*int str = 0;
	for (int i = 0; i < 260; ++i)
	{
	if (m_Name[i] == '|')
	{
	str = i;
	break;
	}
	}*/

	char* s1 = "";  // 크기가 30인 char형 배열을 선언하고 문자열 할당

	char* ptr = strtok_s(m_Name, "|", &s1);      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
												 //ptr = strtok_s(m_Name, "|", &s1);
	while (strcmp(s1, ""))               // 자른 문자열이 나오지 않을 때까지 반복
	{
		ptr = strtok_s(s1, "|", &s1);      // 다음 문자열을 잘라서 포인터를 반환
										   /*if (ptr == NULL)
										   break;
										   strcpy_s(m_Name, ptr);*/
	}
	strcpy_s(m_Name, ptr);


	strcpy_s(m_tAnim.szName, m_Name);
	//전체 시간
	m_fDuration = pAIAnimation->mDuration;
	m_tAnim.fDuration = m_fDuration;
	//걸리는 속도
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;//한 프레임키마다 제어할 수 있도록 만들면?
	m_tAnim.fTickPerSecond = m_fTickPerSecond;
	m_tAnim.fLimitTime = m_fLimitTime;
	//이 애니메이션이 영향을 끼치는 뼈의 개수
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_tAnim.NumChannel = m_iNumChannels;
	m_tAnim.Channels = new TCHANNEL[m_iNumChannels];
	m_tAnim.Bones = new int[m_iNumChannels];
	//

	m_iAnimIndex = _iAnimIndex;

	vector<CHierarchyNode*> vecBones = *pModel->Get_HierarchyNodeVector();
	int NumBone = vecBones.size();
	int Bone = 0;

	for (CHierarchyNode* _Bone : vecBones)
	{
		_bool bIs = false;
		for (int channel = 0; channel < m_iNumChannels; ++channel)
		{
			if (!strcmp(pAIAnimation->mChannels[channel]->mNodeName.data, _Bone->Get_Name()))
			{
				CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[channel], pModel, &(m_tAnim.Channels[Bone]));
				if (pChannel == nullptr)
				{
					MSG_BOX(TEXT("FAILED Create Animation"));
					return E_FAIL;
				}
				m_ChannelIndex.push_back(_Bone->Get_Index());
				m_tAnim.Bones[Bone] = _Bone->Get_Index();
				m_vecChannels.push_back(pChannel);
				bIs = true;
				++Bone;
			}
		}
		if (!bIs)
		{
			m_vecChannels.push_back(nullptr);
		}
		m_ChannelOldKeyFrames.push_back(_Bone->Get_DefaultKeyFrame());
		m_ChannelKeyFrames.push_back(0);

	}

	/*for (_uint i = 0; i < m_iNumChannels; ++i)
	{
	CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel, &(m_tAnim.Channels[i]));
	if (pChannel == nullptr)
	{
	MSG_BOX(TEXT("FAILED Create Animation"));
	return E_FAIL;
	}
	m_vecChannels.push_back(pChannel);

	}*/


	return S_OK;
}

HRESULT CAnimation::Initialize_Prototype(class CAnimModel * pModel, TANIM _tIn, _uint _iAnimIndex)
{
	m_isClone = false;

	m_iAnimIndex = _iAnimIndex;
	ZeroMemory(&m_tAnim, sizeof(TANIM));
	strcpy_s(m_Name, _tIn.szName);
	strcpy_s(m_tAnim.szName, _tIn.szName);
	//전체 시간
	m_fDuration = _tIn.fDuration;
	m_tAnim.fDuration = m_fDuration;
	//걸리는 속도
	m_fTickPerSecond = _tIn.fTickPerSecond;//한 프레임키마다 제어할 수 있도록 만들면?
	m_fLimitTime = _tIn.fLimitTime;
	m_tAnim.fTickPerSecond = m_fTickPerSecond;
	m_tAnim.fLimitTime = m_fLimitTime;
	//이 애니메이션이 영향을 끼치는 뼈의 개수
	m_iNumChannels = _tIn.NumChannel;
	m_tAnim.NumChannel = m_iNumChannels;

	m_tAnim.Channels = _tIn.Channels;
	m_tAnim.Bones = _tIn.Bones;



	vector<CHierarchyNode*> vecBones = *pModel->Get_HierarchyNodeVector();
	int NumBone = vecBones.size();
	int Bone = 0;

	for (CHierarchyNode* _Bone : vecBones)
	{
		m_vecChannels.push_back(nullptr);
		m_ChannelOldKeyFrames.push_back(_Bone->Get_DefaultKeyFrame());
		m_ChannelKeyFrames.push_back(0);
	}

	for (int i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelIndex.push_back(m_tAnim.Bones[i]);
		CChannel* pChannel = CChannel::Create(m_tAnim.Channels[i]);

		if (pChannel == nullptr)
		{
			MSG_BOX(TEXT("FAILED Create Animation"));
			return E_FAIL;
		}
		m_vecChannels[m_tAnim.Bones[i]] = pChannel;
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(CAnimModel * pModel)
{
	m_HierarchyNodes = pModel->Get_HierarchyNodeVector();
	m_isClone = true;
	//m_vecChannels


	//for (_uint i = 0; i < m_iNumChannels; ++i)
	//{
	//	m_ChannelKeyFrames.push_back(0);
	//	m_ChannelOldKeyFrames.push_back(0);
	//	CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_vecChannels[i]->Get_Name());

	//	if (nullptr == pNode)
	//		return E_FAIL;
	//	m_vecChannels[i]->Set_Index(pNode->Get_Index());
	//	m_ChannelIndex.push_back(pNode->Get_Index());

	//	//Safe_AddRef(pNode);
	//}

	return S_OK;
}

_bool CAnimation::Play_Animation(_float fTimeDelta, _float* _pOut)//같을때
{

	m_fPlayTime += m_fTickPerSecond * fTimeDelta;
	//더할 타임델타에 재생 속도를 적용해준다


	if (m_fPlayTime >= m_fDuration)// 같은 애니메이션으로 보간 시작
								   //if(m_bAnimEnd)
								   //if (m_fPlayTime >= m_fLimitTime)
	{
		m_bAnimEnd = false;
		m_fPlayTime = 0.f;
		//m_fTermTime = m_fPlayTime;
		//for (auto& pChannel : m_vecChannels)

		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = 0;

		*_pOut = m_fPlayTime;
		return true;
	}
	*_pOut = m_fPlayTime;
	//else if (m_fPlayTime >= m_fLimitTime && _Reserve)
	//{
	//	*_pOut = true;
	//	m_bAnimEnd = false;
	//	m_fPlayTime = 0.f;
	//	//m_fTermTime = m_fPlayTime;
	//	//for (auto& pChannel : m_vecChannels)
	//	{
	//		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
	//			iCurrentKeyFrame = 0;
	//	}
	//	return true;
	//}

	_uint		iChannelIndex = 0;
	for (auto& iIndex : m_ChannelIndex)
	{
		//키프레임에도 
		m_ChannelKeyFrames[iIndex] = m_vecChannels[iIndex]->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iIndex], (*m_HierarchyNodes)[iIndex], &(m_ChannelOldKeyFrames[iIndex]), &m_bAnimEnd);
	}
	//for (auto& pChannel : m_vecChannels)
	//{
	//	m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], (*m_HierarchyNodes)[m_ChannelIndex[iChannelIndex]], &m_bAnimEnd);
	//	m_ChannelOldKeyFrames[iChannelIndex] = m_ChannelKeyFrames[iChannelIndex];
	//	++iChannelIndex;
	//}

	return false;
}

_bool CAnimation::Play_Animation(_float fTimeDelta, CAnimation * pNextAnim, set<_uint>& _TotalChannel)//다를때
{
	m_fTermTime += m_fTickPerSecond * fTimeDelta;
	//m_fPlayTime += m_fTickPerSecond * fTimeDelta;
	if (m_fTermTime >= m_fMaxTermTime*m_fTickPerSecond)
	{
		m_fPlayTime = 0.f;
		m_fTermTime = 0.f;
		Reset_KeyFrame();
		return true;
	}


	for (int i = 0; i < (*m_HierarchyNodes).size(); ++i)
	{
		KEYFRAME Sour, Dest;

		Sour = m_ChannelOldKeyFrames[i];

		vector<CChannel*> NextAnimChannels = pNextAnim->Get_vecChannel();

		if (NextAnimChannels[i] == nullptr)
		{
			Dest = (*m_HierarchyNodes)[i]->Get_DefaultKeyFrame();
		}
		else
		{
			Dest = NextAnimChannels[i]->Get_KeyFrame(0);
		}

		Update_Transformation(Sour, Dest, i);
	}
	return false;
}

void CAnimation::Update_Transformation(KEYFRAME& Sour, KEYFRAME& Dest, const _uint& _iHierarchyNodeIndex)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	_float3		vSourScale, vDestScale;
	_float4		vSourRotation, vDestRotation;
	_float3		vSourPosition, vDestPosition;

	_float		fRatio = m_fTermTime / (m_fMaxTermTime*m_fTickPerSecond);

	vSourScale = Sour.vScale;
	vSourRotation = Sour.vRotation;
	vSourPosition = Sour.vPosition;

	vDestScale = Dest.vScale;
	vDestRotation = Dest.vRotation;
	vDestPosition = Dest.vPosition;

	XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
	XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
	XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));

	CHierarchyNode* pNode = (*m_HierarchyNodes)[_iHierarchyNodeIndex];

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

	if (nullptr != pNode)
		pNode->Set_Transformation(TransformationMatrix);
}

CChannel * CAnimation::Get_Channel(const char * szChannel)
{
	CChannel* _Instance = nullptr;
	for (CChannel*& pChannel : m_vecChannels)
	{
		if (!strcmp(szChannel, pChannel->Get_Name()))
		{
			_Instance = pChannel;
			break;
		}
	}
	return _Instance;
}


CChannel * CAnimation::Get_Channel_Index(_uint _iIndex)
{
	return m_vecChannels[_iIndex];
}

TANIM CAnimation::Get_ForSave()
{
	m_tAnim.fLimitTime = m_fLimitTime;
	m_tAnim.fMaxTermTime = m_fMaxTermTime;
	m_tAnim.fDuration = m_fDuration;
	m_tAnim.fTickPerSecond = m_fTickPerSecond;
	m_tAnim.NumChannel = m_ChannelIndex.size();
	strcpy_s(m_tAnim.szName, m_Name);

	return m_tAnim;
}

void CAnimation::KeyFrameSave()
{
	_int iTemp = 0;
	for (_uint i : m_ChannelIndex)
	{
		vector<KEYFRAME>* _vecTemp = m_vecChannels[i]->Get_KeyFrameVector();

		for (int j = 0; j < m_tAnim.Channels[iTemp].NumKeyFrame; ++j)
		{
			m_tAnim.Channels[iTemp].KeyFrames[j] = (*_vecTemp)[j];
		}
		++iTemp;
	}
}

void CAnimation::Reset_KeyFrame()
{
	m_fPlayTime = 0.f;
	for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
		iCurrentKeyFrame = 0;
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation, CAnimModel * pModel, _uint _iAnimIndex)
{
	CAnimation* _instance = new CAnimation();

	if (FAILED(_instance->Initialize_Prototype(pAIAnimation, pModel, _iAnimIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(_instance);
	}

	return _instance;
}

CAnimation * CAnimation::Create(class CAnimModel * pModel, TANIM _tIn, _uint _iAnimIndex)
{
	CAnimation* _instance = new CAnimation();

	if (FAILED(_instance->Initialize_Prototype(pModel, _tIn, _iAnimIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(_instance);
	}

	return _instance;
}

CAnimation * CAnimation::Clone(CAnimModel * pModel)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_vecChannels)
		Safe_Release(pChannel);

	if (m_isClone == false)
	{
		for (int j = 0; j < m_tAnim.NumChannel; ++j)
		{
			Safe_Delete_Array(m_tAnim.Channels[j].KeyFrames);
		}
		Safe_Delete_Array(m_tAnim.Channels);
		Safe_Delete_Array(m_tAnim.Bones);

	}
}