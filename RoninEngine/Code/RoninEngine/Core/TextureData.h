#pragma once

#include "CoreMemObject.h"

#include "Texture.h"

class CTextureData : public CCoreRefMTObject
{
public:
	
	struct SRenderTargetTextureCreateParams
	{
		SRenderTargetTextureCreateParams() :
			m_uiWidth(0), m_uiHeight(0), m_eFormat(CTexture::NO_FORMAT),
			m_bDepthResolvable(false), m_bRenderTargetResolvable(true),
			m_bResourceView(true)
		{}

		unsigned int m_uiWidth;
		unsigned int m_uiHeight;
		CTexture::EFormat m_eFormat;
		bool m_bResourceView;
		bool m_bDepthResolvable;
		bool m_bRenderTargetResolvable;
	};

	struct SSourceTextureCreateParams
	{
		stl_string m_sFileName;
	};

	virtual ~CTextureData() {}

	unsigned int GetWidth() const { return m_uiWidth; }
	unsigned int GetHeight() const { return m_uiHeight; }
	unsigned int GetDepth() const { return m_uiDepth; }
	
protected:

	CTextureData();

	//////////////////////////////////////////////////////////////////////////

	unsigned int m_uiWidth;
	unsigned int m_uiHeight;
	unsigned int m_uiDepth;
};

CoreSmartPointer(CTextureData);