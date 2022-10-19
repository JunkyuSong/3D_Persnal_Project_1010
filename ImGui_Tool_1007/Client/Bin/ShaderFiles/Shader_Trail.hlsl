#include "Client_Shader_Defines.hpp"
//struct tagData
//{
//	float3		Position;
//	float2		Texture;
//};
//
//struct tagPos
//{
//	tagData		Data[22];
//};
//
//tagPos		g_vPosition;

//struct tagTexture
//{
//	float2		Texture[22];
//};
//
//tagTexture		g_vTexture;

float4		g_Color;
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;

sampler DefaultSampler = sampler_state {

	filter = min_mag_mip_linear;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	//uint		iIndex : BLENDINDEX;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		/*matWV, */matWVP;

	//matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(g_ViewMatrix, g_ProjMatrix);
	
	/*Out.vPosition = mul(float4(g_vPosition.Data[In.iIndex].Position, 1.f), matWVP);
	Out.vTexUV = g_vPosition.Data[In.iIndex].Texture;*/

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a = (1.f - In.vTexUV.y) * 0.5f + In.vTexUV.x * 0.5f;

	Out.vColor.x = g_Color.x;
	Out.vColor.y = g_Color.y;
	Out.vColor.z = g_Color.z;

	/*if (In.vTexUV.y > 0.3f)
	{
		Out.vColor.x = g_Color.x + In.vTexUV.y;
		Out.vColor.y = g_Color.y + In.vTexUV.y;
		Out.vColor.z = g_Color.z + In.vTexUV.y;
	}*/

	return Out;	
}


technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_CullNone);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}