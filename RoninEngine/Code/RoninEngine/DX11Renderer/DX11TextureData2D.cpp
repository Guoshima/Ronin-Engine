#include "DX11RendererPCH.h"

#include "DX11TextureData2D.h"

//////////////////////////////////////////////////////////////////////////

CDX11TextureData2D::CDX11TextureData2D(ID3D11Texture2D* pkTexture) : 
	m_d3dShaderResourceView(0), m_d3dTexture2D(pkTexture)
{

}

CDX11TextureData2D::~CDX11TextureData2D()
{
	if (m_d3dTexture2D)
		m_d3dTexture2D->Release();
	if (m_d3dShaderResourceView)
		m_d3dShaderResourceView->Release();
}

//////////////////////////////////////////////////////////////////////////