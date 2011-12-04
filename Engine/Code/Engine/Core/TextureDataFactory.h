#pragma once

#include "CoreMemObject.h"

#include "TextureData.h"

class CTextureDataFactory : public CCoreMemObject
{
public:

	static CTextureData* CreateRenderTargetTextureData(
		const CTextureData::SRenderTargetTextureCreateParams& kParams);
	static CTextureData* CreateSourceTextureData(
		const CTextureData::SSourceTextureCreateParams& kParams);

protected:

	virtual CTextureData* Do_CreateRenderTargetTextureData(
		const CTextureData::SRenderTargetTextureCreateParams& kParams) = 0;
	virtual CTextureData* Do_CreateSourceTextureData(
		const CTextureData::SSourceTextureCreateParams& kParams) = 0;

	//////////////////////////////////////////////////////////////////////////

	static CTextureDataFactory* ms_pkInstance;
};