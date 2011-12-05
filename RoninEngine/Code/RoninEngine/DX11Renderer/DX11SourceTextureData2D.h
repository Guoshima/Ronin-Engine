#pragma once

#include "DX11TextureData2D.h"

class CDX11SourceTextureData2D : public CDX11TextureData2D
{
public:

	CDX11SourceTextureData2D(
		const SSourceTextureCreateParams& kParams);
	virtual ~CDX11SourceTextureData2D();

	virtual bool CreateD3DData(ID3D11Device* pkDevice);

protected:

	//////////////////////////////////////////////////////////////////////////

	SSourceTextureCreateParams m_kCreateParams;
};