#pragma once

#include "CoreMemObject.h"

class CTextureData;
CoreSmartPointer(CTextureData);

class CTexture : public CCoreRefMTObject
{
public:

	enum EFormat {
		NO_FORMAT,
		FORMAT_R8G8B8A8_UNORM,
		FORMAT_R32_FLOAT,
		FORMAT_R16G16_FLOAT,
		FORMAT_R16G16_SNORM,
		FORMAT_R24_UNORM_S8_UINT
	};

	CTexture(CTextureData* pkTextureData, bool bRenderTarget = false);
	
	static CTexture* CreateTextureFromFile(
		const stl_string& sFileName);
	static CTexture* CreateRenderTargetTexture(unsigned int uiWidth, unsigned uiHeight,
		EFormat eFormat, bool bRenderTargetResolvable, bool bDepthResolvable);

	virtual ~CTexture();	

	bool IsRenderTarget() const { return m_bIsRenderTarget; }
	CTextureData* GetTextureData() { return m_pkTextureData; }

protected:

	//////////////////////////////////////////////////////////////////////////

	CTextureDataPtr m_pkTextureData;
	bool m_bIsRenderTarget;

};

CoreSmartPointer(CTexture);