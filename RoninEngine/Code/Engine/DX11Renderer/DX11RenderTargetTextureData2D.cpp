#include "DX11RendererPCH.h"

#include "DX11RenderTargetTextureData2D.h"

#include "DX11EnumConverters.h"

//////////////////////////////////////////////////////////////////////////

CDX11RenderTargetTextureData2D::CDX11RenderTargetTextureData2D(
	const SRenderTargetTextureCreateParams& kParams,
	ID3D11Texture2D* pkTexture) :
CDX11TextureData2D(pkTexture),
m_d3dRenderTargetView(0), m_kCreateParams(kParams), 
m_d3dDepthStencilView(0)
{
	m_uiWidth = m_kCreateParams.m_uiWidth;
	m_uiHeight = m_kCreateParams.m_uiHeight;
}

CDX11RenderTargetTextureData2D::~CDX11RenderTargetTextureData2D()
{
	if (m_d3dRenderTargetView)
		m_d3dRenderTargetView->Release();

	if (m_d3dDepthStencilView)
		m_d3dDepthStencilView->Release();
}

//////////////////////////////////////////////////////////////////////////

bool CDX11RenderTargetTextureData2D::CreateD3DData(
	ID3D11Device* pkDevice)
{
	// do we need to create the texture itself
	if (m_d3dTexture2D == 0)
	{
		DXGI_FORMAT eD3DFormat = CDX11EnumConverters::GetTextureCreateFormat(
			m_kCreateParams.m_eFormat);

		if (eD3DFormat == DXGI_FORMAT_UNKNOWN)
			return false;

		UINT uiBindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_kCreateParams.m_bDepthResolvable)
			uiBindFlags |= D3D11_BIND_DEPTH_STENCIL;
		if (m_kCreateParams.m_bRenderTargetResolvable)
			uiBindFlags |= D3D11_BIND_RENDER_TARGET;

		D3D11_TEXTURE2D_DESC kDesc = 
		{
			m_kCreateParams.m_uiWidth,//UINT Width;
			m_kCreateParams.m_uiHeight,//UINT Height;
			1,//UINT MipLevels;
			1,//UINT ArraySize;
			eD3DFormat,//DXGI_FORMAT Format;
			1,//DXGI_SAMPLE_DESC SampleDesc;
			0,
			D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
			uiBindFlags,//UINT BindFlags;
			0,//UINT CPUAccessFlags;
			0//UINT MiscFlags;    
		};

		pkDevice->CreateTexture2D( &kDesc, NULL, &m_d3dTexture2D );
	}

	if (m_d3dTexture2D == 0)
		return false;

	// texture will always be attached to some some shader, else there
	// is no need to create it anyway (except default back buffer texture)
	if (m_kCreateParams.m_bResourceView)
	{
		DXGI_FORMAT eD3DFormat = CDX11EnumConverters::GetShaderResourceViewFormat(
			m_kCreateParams.m_eFormat);

		if (eD3DFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC kDesc = 
			{
				eD3DFormat,
				D3D11_SRV_DIMENSION_TEXTURE2D,
				0,
				0
			};
			kDesc.Texture2D.MipLevels = 1;

			pkDevice->CreateShaderResourceView(m_d3dTexture2D, 
				&kDesc, &m_d3dShaderResourceView);
		} else {
			pkDevice->CreateShaderResourceView(m_d3dTexture2D, 
				NULL, &m_d3dShaderResourceView);
		}
	}

	// bind as depth stencil target ?
	if (m_kCreateParams.m_bDepthResolvable)
	{
		DXGI_FORMAT eD3DFormat = CDX11EnumConverters::GetDepthStencilViewFormat(
			m_kCreateParams.m_eFormat);

		if (eD3DFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC kDesc = 
			{
				eD3DFormat,
				D3D11_DSV_DIMENSION_TEXTURE2D,
				0
			};
			pkDevice->CreateDepthStencilView(m_d3dTexture2D, 
				&kDesc, &m_d3dDepthStencilView); 
		} else {
			pkDevice->CreateDepthStencilView(m_d3dTexture2D, 
				NULL, &m_d3dDepthStencilView); 
		}
	}

	// bind as normal render target ?
	if (m_kCreateParams.m_bRenderTargetResolvable)
	{
		DXGI_FORMAT eD3DFormat = CDX11EnumConverters::GetRenderTargetViewFormat(
			m_kCreateParams.m_eFormat);

		if (eD3DFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_RENDER_TARGET_VIEW_DESC kDesc = 
			{
				eD3DFormat,
				D3D11_RTV_DIMENSION_TEXTURE2D,
				0
			};

			pkDevice->CreateRenderTargetView(m_d3dTexture2D, 
				&kDesc, &m_d3dRenderTargetView);
		} else {
			pkDevice->CreateRenderTargetView(m_d3dTexture2D, 
				NULL, &m_d3dRenderTargetView);
		}
	}	

	return true;
}

//////////////////////////////////////////////////////////////////////////