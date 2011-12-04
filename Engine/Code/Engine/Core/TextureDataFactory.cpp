#include "CorePCH.h"

#include "TextureDataFactory.h"

//////////////////////////////////////////////////////////////////////////

CTextureDataFactory* CTextureDataFactory::ms_pkInstance = 0;

//////////////////////////////////////////////////////////////////////////

CTextureData* CTextureDataFactory::CreateRenderTargetTextureData(
	const CTextureData::SRenderTargetTextureCreateParams& kParams)
{
	if (ms_pkInstance == 0)
		return 0;

	return ms_pkInstance->Do_CreateRenderTargetTextureData(kParams);
}

CTextureData* CTextureDataFactory::CreateSourceTextureData(
	const CTextureData::SSourceTextureCreateParams& kParams)
{
	if (ms_pkInstance == 0)
		return 0;

	return ms_pkInstance->Do_CreateSourceTextureData(kParams);
}

//////////////////////////////////////////////////////////////////////////
