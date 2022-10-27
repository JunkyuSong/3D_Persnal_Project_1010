#include "Client_Shader_Defines.hpp"
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;

float		g_fAlpha;

sampler DefaultSampler = sampler_state {

	filter = min_mag_mip_linear;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler PointSampler = sampler_state {

	filter = min_mag_mip_point;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;


	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (0 == Out.vDiffuse.a)
		discard;

	// -1 ~ 1
	//  0 ~ 1
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	return Out;
}

PS_OUT PS_MAIN_2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (0 == Out.vDiffuse.a)
		discard;

	// -1 ~ 1
	//  0 ~ 1
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	return Out;
}

PS_OUT PS_Sky(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (0 == Out.vDiffuse.a)
		discard;

	// -1 ~ 1
	//  0 ~ 1
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	return Out;
}

PS_OUT PS_Stage(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (0 == Out.vDiffuse.a)
		discard;

	// -1 ~ 1
	//  0 ~ 1
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	return Out;
}

PS_OUT PS_MAIN_3(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.a *= g_fAlpha;

	if (0 == Out.vDiffuse.a)
		discard;
	
	return Out;
}
PS_OUT PS_MAIN_SEl(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);


	Out.vDiffuse = vMtrlDiffuse;
	//Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
	//	(g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular + 0.8f;
	Out.vDiffuse.g = 1.f;
	Out.vDiffuse *= vector(0.8f, 1.f, 0.8f, 1.f);
	if (0 == Out.vDiffuse.a)
		discard;
	return Out;
}

PS_OUT PS_MAIN_ADD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);


	Out.vDiffuse = vMtrlDiffuse;
	//Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
	//	(g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular + 0.8f;
	/*Out.vColor.g = Out.vColor.b;
	Out.vColor.r = Out.vColor.b;*/

	Out.vDiffuse.r = 1.f;
	Out.vDiffuse *= vector(1.f, 0.8f, 0.8f, 1.f);

	if (0 == Out.vDiffuse.a)
		discard;
	return Out;
}

PS_OUT PS_MAIN4(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = (vector)1.f;

	Out.vDiffuse.r = 1.f;
	Out.vDiffuse.g = 1.f;
	Out.vDiffuse.b = 0.f;

	
	if (0 == Out.vDiffuse.a)
		discard;
	return Out;
}

technique11 DefaultTechnique
{

	/*pass DefaultPass
	{
	VertexShader = compile vs_5_0 VS_MAIN();
	GeometryShader = NULL;
	PixelShader = compile ps_5_0 PS_MAIN();
	}*/
	pass DefaultPass2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_2();
	}
	pass SelectPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SEl();
	}

	pass AddPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ADD();
	}

	pass MagicianWeapon
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_3();
	}

	pass MagicianCard
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN4();
	}

	pass Stage
	{
		SetRasterizerState(RS_CullNone);
		SetDepthStencilState(DSS_Default, 0);
		//BS_AlphaBlend
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Stage();
	}

	pass SkyBox
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Skybox, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		/*	SetBlendState(DefaultBlendState);
		SetRasterizerState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Sky();
	}
}