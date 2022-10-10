
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;

vector		g_vLightDir;

vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
float		g_vSpecularPower = 15.f;

vector		g_vCamPosition;

sampler DefaultSampler = sampler_state {

	filter = min_mag_mip_linear;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
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
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	float4		vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);

	Out.fShade = max(dot(normalize(g_vLightDir.xyz) * -1.f,
		normalize(vNormal.xyz)), 0.f);

	float4		vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	float3		vReflect = reflect(normalize(g_vLightDir.xyz), normalize(vNormal.xyz));
	float3		vLook = (vWorldPos - g_vCamPosition).xyz;

	Out.fSpecular = pow(max(dot(normalize(vLook) * -1.f,
		normalize(vReflect)), 0.f), g_vSpecularPower);

	Out.vWorldPos = vWorldPos;

	return Out;
}

VS_OUT VS_MAIN_2(VS_IN In)
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
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = (vector)1.f;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);



	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
		(g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;
	if (0 == Out.vColor.a)
		discard;
	return Out;
}

PS_OUT PS_MAIN_2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = (vector)1.f;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vColor = vMtrlDiffuse;
	if (0 == Out.vColor.a)
		discard;
	return Out;
}

PS_OUT PS_MAIN_SEl(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = (vector)1.f;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);


	Out.vColor = vMtrlDiffuse;
	//Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
	//	(g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular + 0.8f;
	Out.vColor.g = 1.f;
	Out.vColor *= vector(0.8f, 1.f, 0.8f, 1.f);
	if (0 == Out.vColor.a)
		discard;
	return Out;
}

PS_OUT PS_MAIN_ADD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = (vector)1.f;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);


	Out.vColor = vMtrlDiffuse;
	//Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
	//	(g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular + 0.8f;
	/*Out.vColor.g = Out.vColor.b;
	Out.vColor.r = Out.vColor.b;*/

	Out.vColor.r = 1.f;
	Out.vColor *= vector(1.f, 0.8f, 0.8f, 1.f);

	if (0 == Out.vColor.a)
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
		VertexShader = compile vs_5_0 VS_MAIN_2();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_2();
	}
	pass SelectPass
	{
		VertexShader = compile vs_5_0 VS_MAIN_2();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SEl();
	}

	pass AddPass
	{
		VertexShader = compile vs_5_0 VS_MAIN_2();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ADD();
	}
}