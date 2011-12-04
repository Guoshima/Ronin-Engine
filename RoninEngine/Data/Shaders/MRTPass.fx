//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

cbuffer cbWorldData
{
	matrix kWorldViewProjection : WORLDVIEWPROJECTION;
	matrix kWorldView : WORLDVIEW;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;	
    float3 Norm : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float3 Norm : TEXCOORD1;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
	
	output.Pos = mul( input.Pos, kWorldViewProjection );
	output.Tex = input.Tex;
	output.Norm = mul( input.Norm, kWorldView).xyz;
    
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS( PS_INPUT input) : SV_Target
{
	float3 vNormalVS = normalize(input.Norm.xyz);
	return float4(vNormalVS.xy, 0.0f, 1.0f);
}

//--------------------------------------------------------------------------------------

technique10 Render_MRT
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}


