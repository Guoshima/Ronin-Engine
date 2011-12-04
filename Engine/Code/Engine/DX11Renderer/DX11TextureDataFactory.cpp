#include "DX11RendererPCH.h"

#include "DX11TextureDataFactory.h"

#include "DX11SourceTextureData2D.h"
#include "DX11RenderTargetTextureData2D.h"

#include "DX11Renderer.h"

//////////////////////////////////////////////////////////////////////////

void CDX11TextureDataFactory::CreateInstance()
{
	if (ms_pkInstance == 0)
		ms_pkInstance = IntNew CDX11TextureDataFactory();
}

void CDX11TextureDataFactory::DestroyInstance()
{
	IntDelete ms_pkInstance;
	ms_pkInstance = 0;
}

//////////////////////////////////////////////////////////////////////////

CTextureData* CDX11TextureDataFactory::Do_CreateRenderTargetTextureData(
	const CTextureData::SRenderTargetTextureCreateParams& kParams)
{
	ID3D11Device* pkDevice = CDX11Renderer::GetDX11Renderer()->GetDevice();

	CDX11TextureData2D* pkTextureData = IntNew CDX11RenderTargetTextureData2D(kParams);
	pkTextureData->CreateD3DData(pkDevice);

	return pkTextureData;
}

CTextureData* CDX11TextureDataFactory::Do_CreateSourceTextureData(
	const CTextureData::SSourceTextureCreateParams& kParams)
{
	ID3D11Device* pkDevice = CDX11Renderer::GetDX11Renderer()->GetDevice();

	CDX11TextureData2D* pkTextureData = IntNew CDX11SourceTextureData2D(kParams);
	pkTextureData->CreateD3DData(pkDevice);

	return pkTextureData;
}

//////////////////////////////////////////////////////////////////////////
