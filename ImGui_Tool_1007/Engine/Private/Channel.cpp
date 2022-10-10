#include "..\Public\Channel.h"

#include "AnimModel.h"
#include "Animation.h"
#include "HierarchyNode.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel, TCHANNEL* _pOut)
{
	//이름을 왜? 채널과 같은 이름의 하이어라이키 가져오려구
	strcpy_s(m_szName, pAIChannel->mNodeName.data);
	strcpy_s(_pOut->szName, m_szName);
	//m_pHierarchyNode = pModel->Get_HierarchyNode(m_szName);

	//if (nullptr == m_pHierarchyNode)
	//	return E_FAIL;

	//Safe_AddRef(m_pHierarchyNode);

	//키프레임 로드시작

	//스케일, 로테이션, 포지션 갯수중에 가장 큰게 프레임 갯수와 같음
	m_iNumKeyFrames = max(pAIChannel->mNumPositionKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumScalingKeys);
	_pOut->NumKeyFrame = m_iNumKeyFrames;
	_pOut->KeyFrames = new KEYFRAME[m_iNumKeyFrames];
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = pAIChannel->mScalingKeys[i].mTime;
		}
		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTime = pAIChannel->mRotationKeys[i].mTime;
		}
		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
		_pOut->KeyFrames[i] = KeyFrame;
		//memcpy(&(_pOut->KeyFrames[i]), &KeyFrame, sizeof(KEYFRAME));
	}

	return S_OK;
}

HRESULT CChannel::Initialize(TCHANNEL _tIn)
{
	strcpy_s(m_szName, _tIn.szName);
	
	m_iNumKeyFrames = _tIn.NumKeyFrame;
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));
		KeyFrame.fTime = _tIn.KeyFrames[i].fTime;
		KeyFrame.vPosition = _tIn.KeyFrames[i].vPosition;
		KeyFrame.vRotation = _tIn.KeyFrames[i].vRotation;
		KeyFrame.vScale = _tIn.KeyFrames[i].vScale;
		//memcpy(&KeyFrame, &(_tIn.KeyFrames[i]), sizeof(KEYFRAME));
		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

_uint CChannel::Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode* pNode, KEYFRAME* _CurrenKeyFrame, _bool* pOut)
{
	//_float3			vScale;
	//_float4			vRotation;
	//_float3			vPosition;

	if (fPlayTime >= m_KeyFrames.back().fTime)
	{
		//_uint _iFront = 0;
		//while (fPlayTime - m_KeyFrames.back().fTime >= m_KeyFrames[_iFront + 1].fTime)
		//	++_iFront;

		//// 다음 키프레임까지의 사이 시간에서 현재 진행되고 있는 시간이 얼마만큼 갔는지 비율을 구한다
		//_float fRatio = 0.f;

		////현재의 키프레임과 다음의 키프레임을 구하여 그 보간값을 구한다.
		//_float3		vSourScale, vDestScale;
		//_float4		vSourRotation, vDestRotation;
		//_float3		vSourPosition, vDestPosition;
		//_uint		iNextKeyFrame = 0;

		//fRatio = (fPlayTime - m_KeyFrames.back().fTime) /
		//	(20.0f);

		//vSourScale = m_KeyFrames.back().vScale;
		//vSourRotation = m_KeyFrames.back().vRotation;
		//vSourPosition = m_KeyFrames.back().vPosition;

		//vDestScale = m_KeyFrames[_iFront].vScale;
		//vDestRotation = m_KeyFrames[_iFront].vRotation;
		//vDestPosition = m_KeyFrames[_iFront].vPosition;

		//XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		//XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		//XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
		//
		//if (fRatio >= 1.f)
		//{
		//	//*pOut = true;
		//	return _iFront;
		//}
		(*_CurrenKeyFrame).vScale = m_KeyFrames.back().vScale;
		(*_CurrenKeyFrame).vRotation = m_KeyFrames.back().vRotation;
		(*_CurrenKeyFrame).vPosition = m_KeyFrames.back().vPosition;
	}
	else
	{
		while (fPlayTime >= m_KeyFrames[iCurrentKeyFrame + 1].fTime)
			++iCurrentKeyFrame;

		_float		fRatio = (fPlayTime - m_KeyFrames[iCurrentKeyFrame].fTime) /
			(m_KeyFrames[iCurrentKeyFrame + 1].fTime - m_KeyFrames[iCurrentKeyFrame].fTime);

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
		vDestScale = m_KeyFrames[iCurrentKeyFrame + 1].vScale;

		vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
		vDestRotation = m_KeyFrames[iCurrentKeyFrame + 1].vRotation;

		vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;
		vDestPosition = m_KeyFrames[iCurrentKeyFrame + 1].vPosition;

		XMStoreFloat3(&((*_CurrenKeyFrame).vScale), XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&((*_CurrenKeyFrame).vRotation), XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&((*_CurrenKeyFrame).vPosition), XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&(*_CurrenKeyFrame).vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&(*_CurrenKeyFrame).vRotation), XMVectorSetW(XMLoadFloat3(&(*_CurrenKeyFrame).vPosition), 1.f));

	if (nullptr != pNode)
		pNode->Set_Transformation(TransformationMatrix);

	//*pOut = false;
	return iCurrentKeyFrame;
}

//_uint CChannel::Update_Transformation(_float _fCurPlayTime, class CHierarchyNode* pNode)
//{
//	_float3			vScale;
//	_float4			vRotation;
//	_float3			vPosition;
//	//현재 재생하고 있는 애니메이션 있고 -> 거기서 바로 다음 애니메이션을 진행하고 싶다면.
//	
//	//if (_fCurPlayTime >= m_KeyFrames.back().fTime)
//	if (_fCurPlayTime >= m_KeyFrames.back().fTime)
//	{
//		_uint _iFront = 0;
//		while (_fCurPlayTime - m_KeyFrames.back().fTime >= m_KeyFrames[_iFront + 1].fTime)
//			++_iFront;
//
//		// 다음 키프레임까지의 사이 시간에서 현재 진행되고 있는 시간이 얼마만큼 갔는지 비율을 구한다
//		_float fRatio = 0.f;
//
//		//현재의 키프레임과 다음의 키프레임을 구하여 그 보간값을 구한다.
//		_float3		vSourScale, vDestScale;
//		_float4		vSourRotation, vDestRotation;
//		_float3		vSourPosition, vDestPosition;
//		_uint		iNextKeyFrame = 0;
//
//		fRatio = (_fCurPlayTime - m_KeyFrames.back().fTime) /
//			(20.0f);
//
//		vSourScale = m_KeyFrames.back().vScale;
//		vSourRotation = m_KeyFrames.back().vRotation;
//		vSourPosition = m_KeyFrames.back().vPosition;
//		
//		vDestScale = m_KeyFrames[_iFront].vScale;
//		vDestRotation = m_KeyFrames[_iFront].vRotation;
//		vDestPosition = m_KeyFrames[_iFront].vPosition;
//
//		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
//		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
//		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
//		if (fRatio >= 1.f)
//		{
//			return true;
//		}	
//	}
//	else
//	{
//		// 진행된 시간에 따라 키프레임을 갱신한다.
//		// 만일 프레임이 밀려 키프레임보다 더 큰 시간이 들어오면 애니메이션에 문제가 생기기 때문에
//		// 그만큼 더 갱신시킨다
//		while (_fCurPlayTime >= m_KeyFrames[m_iCurrentKeyFrame + 1].fTime)
//			++m_iCurrentKeyFrame;
//
//		// 다음 키프레임까지의 사이 시간에서 현재 진행되고 있는 시간이 얼마만큼 갔는지 비율을 구한다
//		_float fRatio = (_fCurPlayTime - m_KeyFrames[m_iCurrentKeyFrame].fTime) /
//			(m_KeyFrames[m_iCurrentKeyFrame + 1].fTime - m_KeyFrames[m_iCurrentKeyFrame].fTime);
//
//		//현재의 키프레임과 다음의 키프레임을 구하여 그 보간값을 구한다.
//		_float3		vSourScale, vDestScale;
//		_float4		vSourRotation, vDestRotation;
//		_float3		vSourPosition, vDestPosition;
//
//		vSourScale = m_KeyFrames[m_iCurrentKeyFrame].vScale;
//		vDestScale = m_KeyFrames[m_iCurrentKeyFrame + 1].vScale;
//
//		vSourRotation = m_KeyFrames[m_iCurrentKeyFrame].vRotation;
//		vDestRotation = m_KeyFrames[m_iCurrentKeyFrame + 1].vRotation;
//
//		vSourPosition = m_KeyFrames[m_iCurrentKeyFrame].vPosition;
//		vDestPosition = m_KeyFrames[m_iCurrentKeyFrame + 1].vPosition;
//
//		// 보간값 구하는 함수 : Lerp
//		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
//		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
//		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
//	}
//
//	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
//	
//	pNode->Set_Transformation(TransformationMatrix);
//
//	return false;
//}

//_bool CChannel::Update_Transformation(_float _fCurPlayTime, CAnimation* pNextAnim)
//{
//	_float3			vScale;
//	_float4			vRotation;
//	_float3			vPosition;
//
//	//현재 재생하고 있는 애니메이션 있고 -> 거기서 바로 다음 애니메이션을 진행하고 싶다면.
//
//	while (_fCurPlayTime >= m_KeyFrames[m_iCurrentKeyFrame + 1].fTime)
//		++m_iCurrentKeyFrame;
//
//	// 다음 키프레임까지의 사이 시간에서 현재 진행되고 있는 시간이 얼마만큼 갔는지 비율을 구한다
//	_float fRatio = 0.f;
//
//	//현재의 키프레임과 다음의 키프레임을 구하여 그 보간값을 구한다.
//	_float3		vSourScale, vDestScale;
//	_float4		vSourRotation, vDestRotation;
//	_float3		vSourPosition, vDestPosition;
//	_uint		iNextKeyFrame = 0;
//
//	vSourScale = m_KeyFrames[m_iCurrentKeyFrame].vScale;
//	vSourRotation = m_KeyFrames[m_iCurrentKeyFrame].vRotation;
//	vSourPosition = m_KeyFrames[m_iCurrentKeyFrame].vPosition;
//
//	CChannel* _NextState = nullptr;
//	_NextState = pNextAnim->Get_Channel(m_szName);
//	//만약 이름이 같은 채널이 있다면, 
//	if (_NextState != nullptr)
//	{
//		fRatio = (_fCurPlayTime - m_KeyFrames.back().fTime) /
//			(20.0f);
//		vDestScale = _NextState->Get_KeyFrame(iNextKeyFrame).vScale;
//		vDestRotation = _NextState->Get_KeyFrame(iNextKeyFrame).vRotation;
//		vDestPosition = _NextState->Get_KeyFrame(iNextKeyFrame).vPosition;
//	}
//	//만약 이름이 같은 채널이 없다면, 근데 여기서 문제점 두 애니메이션의 모든 뼈를 다 보간시켜야함
//	else
//	{
//		//fRatio = (_fCurPlayTime - m_KeyFrames.back().fTime) /
//		//	(2.f);
//	}
//	XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
//	XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
//	XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
//	if (fRatio >= 1.f)
//	{
//		return true;
//	}
//
//	/*
//	//if (_fCurPlayTime >= m_KeyFrames.back().fTime)
//	if (_fCurPlayTime >= m_KeyFrames.back().fTime)
//	{
//		//while (_fCurPlayTime >= m_KeyFrames[m_iCurrentKeyFrame + 1].fTime)
//		//	++m_iCurrentKeyFrame;
//
//		// 다음 키프레임까지의 사이 시간에서 현재 진행되고 있는 시간이 얼마만큼 갔는지 비율을 구한다
//		_float fRatio = 0.f;
//
//		//현재의 키프레임과 다음의 키프레임을 구하여 그 보간값을 구한다.
//		_float3		vSourScale, vDestScale;
//		_float4		vSourRotation, vDestRotation;
//		_float3		vSourPosition, vDestPosition;
//		_uint		iNextKeyFrame = 0;
//
//		vSourScale = m_KeyFrames.back().vScale;
//		vSourRotation = m_KeyFrames.back().vRotation;
//		vSourPosition = m_KeyFrames.back().vPosition;
//
//		CChannel* _NextState = nullptr;
//		_NextState = pNextAnim->Get_Channel(m_szName);
//		//만약 이름이 같은 채널이 있다면, 
//		if (_NextState != nullptr)
//		{
//			fRatio = (_fCurPlayTime - m_KeyFrames.back().fTime) /
//				(20.0f);
//			vDestScale = _NextState->Get_KeyFrame(iNextKeyFrame).vScale;
//			vDestRotation = _NextState->Get_KeyFrame(iNextKeyFrame).vRotation;
//			vDestPosition = _NextState->Get_KeyFrame(iNextKeyFrame).vPosition;
//		}
//		//만약 이름이 같은 채널이 없다면, 
//		else
//		{
//			//fRatio = (_fCurPlayTime - m_KeyFrames.back().fTime) /
//			//	(2.f);
//		}
//		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
//		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
//		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
//		if (fRatio >= 1.f)
//		{
//			return true;
//		}
//
//		//vSourScale = m_KeyFrames[m_iCurrentKeyFrame].vScale;
//		//vDestScale = m_KeyFrames[m_iCurrentKeyFrame + 1].vScale;
//
//		//vSourRotation = m_KeyFrames[m_iCurrentKeyFrame].vRotation;
//		//vDestRotation = m_KeyFrames[m_iCurrentKeyFrame + 1].vRotation;
//
//		//vSourPosition = m_KeyFrames[m_iCurrentKeyFrame].vPosition;
//		//vDestPosition = m_KeyFrames[m_iCurrentKeyFrame + 1].vPosition;
//
//		// 보간값 구하는 함수 : Lerp
//
//	}
//	else
//	{
//		// 진행된 시간에 따라 키프레임을 갱신한다.
//		// 만일 프레임이 밀려 키프레임보다 더 큰 시간이 들어오면 애니메이션에 문제가 생기기 때문에
//		// 그만큼 더 갱신시킨다
//		while (_fCurPlayTime >= m_KeyFrames[m_iCurrentKeyFrame + 1].fTime)
//			++m_iCurrentKeyFrame;
//
//		// 다음 키프레임까지의 사이 시간에서 현재 진행되고 있는 시간이 얼마만큼 갔는지 비율을 구한다
//		_float fRatio = (_fCurPlayTime - m_KeyFrames[m_iCurrentKeyFrame].fTime) /
//			(m_KeyFrames[m_iCurrentKeyFrame + 1].fTime - m_KeyFrames[m_iCurrentKeyFrame].fTime);
//
//		//현재의 키프레임과 다음의 키프레임을 구하여 그 보간값을 구한다.
//		_float3		vSourScale, vDestScale;
//		_float4		vSourRotation, vDestRotation;
//		_float3		vSourPosition, vDestPosition;
//
//		vSourScale = m_KeyFrames[m_iCurrentKeyFrame].vScale;
//		vDestScale = m_KeyFrames[m_iCurrentKeyFrame + 1].vScale;
//
//		vSourRotation = m_KeyFrames[m_iCurrentKeyFrame].vRotation;
//		vDestRotation = m_KeyFrames[m_iCurrentKeyFrame + 1].vRotation;
//
//		vSourPosition = m_KeyFrames[m_iCurrentKeyFrame].vPosition;
//		vDestPosition = m_KeyFrames[m_iCurrentKeyFrame + 1].vPosition;
//
//		// 보간값 구하는 함수 : Lerp
//		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
//		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
//		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
//	}
//	*/
//	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
//
//	if (nullptr != m_pHierarchyNode)
//		m_pHierarchyNode->Set_Transformation(TransformationMatrix);
//	return false;
//}

CChannel * CChannel::Create(aiNodeAnim * pAIChannel, CAnimModel * pModel, TCHANNEL* _pOut)
{
	CChannel* _Instance = new CChannel();

	if (FAILED(_Instance->Initialize(pAIChannel,_pOut)))
	{
		MSG_BOX(TEXT("Failed to Create Channel"));
		Safe_Release(_Instance);
	}
	return _Instance;
}

CChannel * CChannel::Create(TCHANNEL _tIn)
{
	CChannel* _Instance = new CChannel();

	if (FAILED(_Instance->Initialize(_tIn)))
	{
		MSG_BOX(TEXT("Failed to Create Channel"));
		Safe_Release(_Instance);
	}
	return _Instance;
}

void CChannel::Free()
{

}
