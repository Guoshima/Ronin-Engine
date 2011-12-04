//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : SLOT_0;
SamplerState samDiffuseLinear : SLOT_0
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D txShadow : SLOT_1;
SamplerState samShadowPoint : SLOT_1
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

cbuffer cbWorldData
{
    matrix kWorld : WORLD;
	matrix kViewProjection : VIEWPROJECTION;
	matrix kWorldViewProjection : WORLDVIEWPROJECTION;
};

cbuffer cbMaterialData
{
	float4 vMeshColor[3];
}

cbuffer cbShadowData
{
	matrix kLightViewProjection;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
};

struct VS_INPUT_TEXTURED
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
	float4 kLightViewPos : TEXCOORD2;
};

struct PS_INPUT_SHADOW
{
    float4 Pos : SV_POSITION;
	float2 vPos : TEXCOORD0;
};

struct PS_INPUT_TEXTURED
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float2 vDepth : TEXCOORD1;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
	
	float4 kWorldPos = mul( input.Pos, kWorld );
	output.Pos = mul( kWorldPos, kViewProjection );
	
	output.kLightViewPos = mul( kWorldPos, kLightViewProjection );
	//output.kLightViewPos /= output.kLightViewPos.w;
	
    //output.Pos = mul( input.Pos, kWorldViewProjection );
    //output.Pos = mul( output.Pos, kView );
    //output.Pos = mul( output.Pos, kProjection );
    
    return output;
}

PS_INPUT_SHADOW VS_Shadow( VS_INPUT input )
{
    PS_INPUT_SHADOW output = (PS_INPUT_SHADOW)0;
    output.Pos = mul( input.Pos, kWorldViewProjection );
	
	//float4 kWorldPos = mul( input.Pos, kWorld );
	//output.Pos = mul( kWorldPos, kLightViewProjection );	
    //output.Pos = mul( output.Pos, kView );
    //output.Pos = mul( output.Pos, kProjection );
	
    output.vPos = output.Pos.zw;
	
    return output;
}

PS_INPUT_TEXTURED VS_Textured( VS_INPUT_TEXTURED input )
{
    PS_INPUT_TEXTURED output = (PS_INPUT_TEXTURED)0;
    output.Pos = mul( input.Pos, kWorldViewProjection );
    //output.Pos = mul( output.Pos, kView );
    //output.Pos = mul( output.Pos, kProjection );
    output.Tex = input.Tex;
	output.vDepth = output.Pos.zw;
    
    return output;
}

#define SHADOW_EPSILON 0.00005f
#define SMAP_SIZE 256

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
	//return input.kLightViewPos;
	float3 vLightPos = input.kLightViewPos.xyz / input.kLightViewPos.w;
	
	//return float4(vLightPos.zzz,1.0f);
	
	//transform from RT space to texture space.
    float2 ShadowTexC = 0.5 * vLightPos.xy + float2( 0.5, 0.5 );
    ShadowTexC.y = 1.0f - ShadowTexC.y;
		
	//return float4(input.kLightViewPos.zzz / 10.0f, 1.0f);
	float fShadowSample = txShadow.Sample( samShadowPoint, ShadowTexC ).x;
	//return float4(fShadowSample.xxx,1.0f);
		
	//float fShadow = (fShadowSample + SHADOW_EPSILON) < saturate(vLightPos.z) ? 0.0f : 1.0f;
	//return float4(fShadow.xxx, 1.0f);
	
	// transform to texel space
    float2 texelpos = SMAP_SIZE * ShadowTexC;
        
    // Determine the lerp amounts           
    float2 lerps = frac( texelpos );

    //read in bilerp stamp, doing the shadow checks
    float sourcevals[4];
    sourcevals[0] = (txShadow.Sample( samShadowPoint, ShadowTexC ).x + SHADOW_EPSILON < saturate(vLightPos.z))? 0.0f: 1.0f;  
    sourcevals[1] = (txShadow.Sample( samShadowPoint, ShadowTexC + float2(1.0/SMAP_SIZE, 0) ).x + SHADOW_EPSILON < saturate(vLightPos.z))? 0.0f: 1.0f;  
    sourcevals[2] = (txShadow.Sample( samShadowPoint, ShadowTexC + float2(0, 1.0/SMAP_SIZE) ).x + SHADOW_EPSILON < saturate(vLightPos.z))? 0.0f: 1.0f;  
    sourcevals[3] = (txShadow.Sample( samShadowPoint, ShadowTexC + float2(1.0/SMAP_SIZE, 1.0/SMAP_SIZE) ).x + SHADOW_EPSILON < saturate(vLightPos.z))? 0.0f: 1.0f;  
        
    // lerp between the shadow values to calculate our light amount
    float fShadow = lerp( lerp( sourcevals[0], sourcevals[1], lerps.x ),
        lerp( sourcevals[2], sourcevals[3], lerps.x ), lerps.y );	
	
	//return float4(fShadow.xxx, 1.0f);
	return float4(vMeshColor[2].xyz * fShadow, vMeshColor[2].a);
}

float4 PS_Textured( PS_INPUT_TEXTURED input) : SV_Target
{
	//float fDepth = input.vDepth.x / input.vDepth.y;
	//return float4(fDepth.xxx, 1.0f);
	//float fShadow = txShadow.Sample( samShadowPoint, input.Tex ).x;
	//return float4(fShadow.xxx, 1.0f);
    return txDiffuse.Sample( samDiffuseLinear, input.Tex ) * vMeshColor[0];
}

float4 PS_Shadow( PS_INPUT_SHADOW input) : SV_Target
{
	float fDepth = input.vPos.x / input.vPos.y;
	return float4(fDepth.xxxx);
}


//--------------------------------------------------------------------------------------
technique10 Render
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

technique10 Render_Textured
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Textured() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Textured() ) );
    }
}

technique10 Render_Shadow
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Shadow() ) );
        SetGeometryShader( NULL );
        SetPixelShader(  NULL );
    }
}

