#pragma once
#include "Engine_Defines.h"
// 모델이랑 툴만 포함하면 된다
namespace Engine
{
	typedef struct tagChannel
	{
		int			NumKeyFrame;
		KEYFRAME*	 KeyFrames;
	}TCHANNEL;
	typedef struct tagAnimation
	{
		char*		szName[260];
		float		fDuration;
		float		fTickPerSecond;
		int			iNumChannel;
		int*		ChannelIndecix;
		
	}TANIM;

	typedef struct tagMaterial
	{
		char		szPath[19][260];//첫번째 널떨어지면 없는거임
	}TMATERIAL;

	typedef struct tagContainer
	{
		char		szName[260];
		int			iIndex;
		int			iNumVertices;
		int			iNumFaces;
		int			iNumBones;
		int*		BoneIndices;//인덱스 배열
	}TCONTAINER;
	typedef struct tagHierarcky
	{
		int			iParentIndex;
		_float4x4	Transformaiont;
	}THIERARCKY;
	typedef struct tagModel
	{
		bool			bAnim;

		int				NumHierarckies;
		THIERARCKY*		tHierarcky;
		//int				iNumMaterial;
		int				NumMeshes;
		TCONTAINER*		tContainer;
		TMATERIAL*		tMaterial;

		int				NumAnim;
		tagAnimation*	tAnimations;
	}TMODEL;
}
