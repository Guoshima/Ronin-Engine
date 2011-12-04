#include "DX11RendererPCH.h"

#include "DX11StateFactory.h"

#include <SamplerDesc.h>

//////////////////////////////////////////////////////////////////////////

ID3D11Device* CDX11StateFactory::ms_d3dDevice = 0;

//////////////////////////////////////////////////////////////////////////

void CDX11StateFactory::SetD3DDevice(ID3D11Device* pkDevice)
{
	if (ms_d3dDevice)
		ms_d3dDevice->Release();
	ms_d3dDevice = pkDevice;
	if (ms_d3dDevice)
		ms_d3dDevice->AddRef();
}

//////////////////////////////////////////////////////////////////////////

ID3D11SamplerState* CDX11StateFactory::CreateSamplerState(
	const CSamplerDesc* pkSamplerDesc)
{
	if (pkSamplerDesc == 0 || ms_d3dDevice == 0) return 0;

	D3D11_SAMPLER_DESC kDesc;
	memset(&kDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	FillSamplerDesc(pkSamplerDesc, kDesc);

	ID3D11SamplerState* pkSamplerSate = 0;
	ms_d3dDevice->CreateSamplerState(&kDesc, &pkSamplerSate);

	return pkSamplerSate;
}

void CDX11StateFactory::FillSamplerDesc(
	const CSamplerDesc* pkDesc, D3D11_SAMPLER_DESC& kD3DDesc)
{
	// nothing special for now
}

//////////////////////////////////////////////////////////////////////////
