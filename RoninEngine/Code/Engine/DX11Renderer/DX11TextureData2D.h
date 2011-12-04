#pragma once

#include <TextureData2D.h>

class CDX11TextureData2D : public CTextureData2D
{
public:

	virtual ~CDX11TextureData2D();

	virtual bool CreateD3DData(ID3D11Device* pkDevice) = 0;

	ID3D11ShaderResourceView* GetShaderResourceView() { 
		return m_d3dShaderResourceView; 
	}

protected:

	CDX11TextureData2D(ID3D11Texture2D* pkTexture = 0);

	//////////////////////////////////////////////////////////////////////////

	// the actual D3D data
	ID3D11Texture2D* m_d3dTexture2D;
	ID3D11ShaderResourceView* m_d3dShaderResourceView;


	
};