#pragma once

#include <CoreMemObject.h>

class CSamplerDesc;

class CDX11StateFactory : public CCoreMemObject
{
public:

	static void SetD3DDevice(ID3D11Device* pkDevice);
	
	static ID3D11SamplerState* CreateSamplerState(
		const CSamplerDesc* pkSamplerDesc);

protected:

	static void FillSamplerDesc(const CSamplerDesc* pkDesc, 
		D3D11_SAMPLER_DESC& kD3DDesc);

	static ID3D11Device* ms_d3dDevice;

};