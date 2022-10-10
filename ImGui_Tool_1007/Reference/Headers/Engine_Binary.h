#pragma once

#include "Engine_Defines.h"

namespace Engine
{
	struct TCHANNEL
	{
		int			NumKeyFrame;//
		char		szName[260];
		KEYFRAME*	KeyFrames;//
	};
	struct TANIM
	{
		char		szName[260];
		float		fDuration;
		float		fTickPerSecond;
		float		fLimitTime;
		float		fMaxTermTime;
		int			NumChannel;
		int*		Bones;
		TCHANNEL*	Channels;
	};
	struct TMATERIAL
	{
		_tchar		szPath[AI_TEXTURE_TYPE_MAX][260];
	};
	struct TMATERIALS
	{
		int				NumMaterials;
		TMATERIAL*		tMaterial;
	};
	struct TCONTAINER
	{
		char		szName[260];
		int			iIndex;
		int			NumVertices;
		VTXMODEL*	pVertices;
		int			NumFaces;
		FACEINDICES32* pIndices;
	};
	struct TANIMCONTAINER
	{
		char		szName[260];
		int			iIndex;
		int			NumVertices;
		VTXANIMMODEL*	pVertices;
		int			NumFaces;
		FACEINDICES32* pIndices;
		int			NumBones;
		int*		BoneIndecis;
	};
	struct THIERARCKY
	{
		char			szName[260];
		int				ParentIndex;
		int				iDepth;
		_float4x4		Transformation;
		_float4x4		Offset;
	};
	struct TMODEL
	{
		_bool			bAnim;

		char			Path[260];
		char			Name[260];

		int				NumMeshes;
		TCONTAINER*		tMeshes;
		TMATERIALS		AllMaterials;
	};

	struct TANIMMODEL
	{
		_bool			bAnim;

		char			Path[260];
		char			Name[260];

		_float4x4		Pivot;

		int				NumMeshes;
		TANIMCONTAINER*		tMeshes;
		TMATERIALS		AllMaterials;

		int				NumHierarcky;
		THIERARCKY*		tHierarcky;

		int				NumAnim;
		TANIM*			tAnim;
	};
}
