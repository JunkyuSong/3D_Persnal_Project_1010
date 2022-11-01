#include "Client_Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;

float		g_HpPer, g_PreHpPer;

sampler DefaultSampler = sampler_state {

	filter = min_mag_mip_linear;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
};

sampler PointSampler = sampler_state {

	filter = min_mag_mip_point;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
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

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (0 == Out.vColor.a)
		discard;
	return Out;	
}

PS_OUT PS_MAIN_HP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (In.vTexUV.x > g_PreHpPer)
	{
		Out.vColor.a = 0.f;
	}
		
	else if (In.vTexUV.x > g_HpPer)
	{
		Out.vColor.r = 1.f;
	}

	if (0 >= Out.vColor.a)
		discard;
	return Out;
}

PS_OUT PS_MAIN_MONSTERHP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (In.vTexUV.x > g_HpPer)
	{
		discard;
	}

	if (0 >= Out.vColor.a)
		discard;
	
	return Out;
}

PS_OUT PS_Select(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if(In.vTexUV.x >= 0.f && In.vTexUV.x <= 0.01f)
		Out.vColor = float4(0.f, 1.f, 0.f, 1.f);
	else if (In.vTexUV.y >= 0.f && In.vTexUV.y <= 0.01f)
		Out.vColor = float4(0.f, 1.f, 0.f, 1.f);
	else if (In.vTexUV.y <= 1.f && In.vTexUV.y >= 0.99f)
		Out.vColor = float4(0.f, 1.f, 0.f, 1.f);
	else if (In.vTexUV.x <= 1.f && In.vTexUV.x >= 0.99f)
		Out.vColor = float4(0.f, 1.f, 0.f, 1.f);

	if (0 == Out.vColor.a)
		discard;

	return Out;
}

PS_OUT PS_NonSelect(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (In.vTexUV.x >= 0.f && In.vTexUV.x <= 0.005f)
		Out.vColor = float4(1.f, 0.f,  0.f, 1.f);
	else if (In.vTexUV.y >= 0.f && In.vTexUV.y <= 0.005f)
		Out.vColor = float4(1.f, 0.f, 0.f, 1.f);
	else if (In.vTexUV.y <= 1.f && In.vTexUV.y >= 0.995f)
		Out.vColor = float4(1.f, 0.f, 0.f, 1.f);
	else if (In.vTexUV.x <= 1.f && In.vTexUV.x >= 0.995f)
		Out.vColor = float4(1.f, 0.f, 0.f, 1.f);

	if (0 == Out.vColor.a)
		discard;

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

	pass SelectPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Select();
	}

	pass NonSelectPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NonSelect();
	}

	pass HpBarPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HP();
	}

	pass MonsterHpBarPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MONSTERHP();
	}
}