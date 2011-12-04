//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

Texture2D txNormal : SLOT_0;
SamplerState samNormalPoint : SLOT_0
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D txDepth : SLOT_1;
SamplerState samDepthPoint : SLOT_1
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
	
	// already in clip space!
    output.Pos = input.Pos;
    output.Tex = input.Tex;
    
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS( PS_INPUT input) : SV_Target
{
    float2 vNormalData = txNormal.Sample( samNormalPoint, input.Tex ).xy;
	float fDepth = txDepth.Sample( samDepthPoint, input.Tex ).x;
	//return float4(fDepth.xxx, 1.0f);
	return float4(vNormalData.xy, 0.0f, 1.0f);
}

//--------------------------------------------------------------------------------------

technique10 PreLightPass
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}


