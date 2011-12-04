#pragma once

#include "DX11TextureData2D.h"

class CDX11RenderTargetTextureData2D : public CDX11TextureData2D
{
public:

	CDX11RenderTargetTextureData2D(
		const SRenderTargetTextureCreateParams& kParams,
		ID3D11Texture2D* pkTexture = 0);
	virtual ~CDX11RenderTargetTextureData2D();

	virtual bool CreateD3DData(ID3D11Device* pkDevice);

	ID3D11DepthStencilView* GetDepthStencilView()
	{
		return m_d3dDepthStencilView;
	}

	ID3D11RenderTargetView* GetRenderTargetView()
	{
		return m_d3dRenderTargetView;
	}

protected:

	//////////////////////////////////////////////////////////////////////////

	SRenderTargetTextureCreateParams m_kCreateParams;

	// the actual D3D data views
	ID3D11RenderTargetView* m_d3dRenderTargetView;
	ID3D11DepthStencilView* m_d3dDepthStencilView;
};