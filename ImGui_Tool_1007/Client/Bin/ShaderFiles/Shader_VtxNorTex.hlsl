#include "Client_Shader_Defines.hpp"
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;

vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;

vector		g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);


sampler DefaultSampler = sampler_state
{

	filter = min_mag_mip_linear;
	AddressU = WRAP;
	AddressV = WRAP;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float		fShade : COLOR0;
	float2		vTexUV : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	float4		vNormal = mul(float4(In.vNormal, 1.f), g_WorldMatrix);

	//노멀을 반대로 만들어서 빛의 세기를 구한다
	Out.fShade = max(dot(normalize(g_vLightDir.xyz) * -1.f,
		normalize(vNormal.xyz)), 0.f);
	
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float		fShade : COLOR0;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV * 20.0f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient));

	return Out;	
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	
}