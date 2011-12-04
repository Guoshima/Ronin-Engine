#pragma once

#include <TextureDataFactory.h>

class CDX11TextureDataFactory : public CTextureDataFactory
{
public:

	static void CreateInstance();
	static void DestroyInstance();

protected:

	virtual CTextureData* Do_CreateRenderTargetTextureData(
		const CTextureData::SRenderTargetTextureCreateParams& kParams);
	virtual CTextureData* Do_CreateSourceTextureData(
		const CTextureData::SSourceTextureCreateParams& kParams);
};