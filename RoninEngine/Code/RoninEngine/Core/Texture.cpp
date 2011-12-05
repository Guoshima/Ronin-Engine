#include "CorePCH.h"

#include "Texture.h"

#include "TextureData.h"
#include "TextureDataFactory.h"

//////////////////////////////////////////////////////////////////////////

CTexture* CTexture::CreateTextureFromFile(const stl_string& sFileName)
{
	CTextureData::SSourceTextureCreateParams kParams;
	kParams.m_sFileName = sFileName;

	CTextureData* pkTextureData = 
		CTextureDataFactory::CreateSourceTextureData(kParams);
	if (pkTextureData == 0)
		return 0;

	CTexture* pkTexture = IntNew CTexture(pkTextureData);
	pkTexture->m_bIsRenderTarget = false;
	return pkTexture;
}

CTexture* CTexture::CreateRenderTargetTexture(
	unsigned int uiWidth, unsigned uiHeight, EFormat eFormat,
	bool bRenderTargetResolvable, bool bDepthResolvable)
{
	CTextureData::SRenderTargetTextureCreateParams kParams;
	kParams.m_uiHeight = uiHeight;
	kParams.m_uiWidth = uiWidth;
	kParams.m_eFormat = eFormat;
	kParams.m_bDepthResolvable = bDepthResolvable;
	kParams.m_bRenderTargetResolvable = bRenderTargetResolvable;

	CTextureData* pkTextureData = 
		CTextureDataFactory::CreateRenderTargetTextureData(kParams);
	if (pkTextureData == 0)
		return 0;

	CTexture* pkTexture = IntNew CTexture(pkTextureData);
	pkTexture->m_bIsRenderTarget = true;
	return pkTexture;
}

//////////////////////////////////////////////////////////////////////////

CTexture::CTexture(CTextureData* pkTextureData, 
				   bool bRenderTarget /* = false */) : 
m_pkTextureData(pkTextureData), m_bIsRenderTarget(bRenderTarget)
{

}

CTexture::~CTexture()
{
	// smart pointer
	m_pkTextureData = 0;
}


//////////////////////////////////////////////////////////////////////////