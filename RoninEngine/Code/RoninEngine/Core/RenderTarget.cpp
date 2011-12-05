#include "CorePCH.h"

#include "RenderTarget.h"

#include "TextureData.h"

//////////////////////////////////////////////////////////////////////////

CRenderTarget::CRenderTarget() : m_uiNumOfTargets(0),
	m_spDepthStencil(0), m_uiHeight(0), m_uiWidth(0), m_fDepthClear(0.0f),
	m_akClearColors(0), m_iStencilClear(0)
{

}

CRenderTarget::~CRenderTarget()
{
	IntFree(m_akClearColors);
}

bool CRenderTarget::Init(unsigned int uiNumOfTargets, 
	const stl_vector<CTexturePtr>& vRenderTargets, CTexture* pkDepthStencilView)
{
	m_uiNumOfTargets = uiNumOfTargets;
	m_vRenderTargets = vRenderTargets;
	m_spDepthStencil = pkDepthStencilView;

	// retrieve the width and height info
	CTextureData* pkTextureData = 0;
	if (uiNumOfTargets > 0)
	{
		pkTextureData = m_vRenderTargets[0]->GetTextureData();
	} else if (m_spDepthStencil) {
		pkTextureData = m_spDepthStencil->GetTextureData();
	}

	if (pkTextureData == 0)
		return false;

	m_uiWidth = pkTextureData->GetWidth();
	m_uiHeight = pkTextureData->GetHeight();

	m_akClearColors = (D3DXVECTOR4*) IntAlloc(D3DXVECTOR4, m_uiNumOfTargets);
	memset(m_akClearColors, 0, sizeof(D3DXCOLOR) * m_uiNumOfTargets);

	return true;
}

bool CRenderTarget::Init(CTexture* pkDepthStencilView)
{
	m_uiNumOfTargets = 0;
	m_spDepthStencil = pkDepthStencilView;

	// retrieve the width and height info
	CTextureData* pkTextureData = m_spDepthStencil->GetTextureData();
	if (pkTextureData == 0)
		return false;

	m_uiWidth = pkTextureData->GetWidth();
	m_uiHeight = pkTextureData->GetHeight();

	return true;
}

//////////////////////////////////////////////////////////////////////////

void CRenderTarget::SetClearColor(D3DXVECTOR4 kColor)
{
	for (unsigned int ui = 0; ui < m_uiNumOfTargets; ++ui)
	{
		m_akClearColors[ui] = kColor;
	}
}

void CRenderTarget::SetClearColor(D3DXVECTOR4 kColor, unsigned int uiTarget)
{
	if (uiTarget > m_uiNumOfTargets || m_akClearColors == 0)
		return;

	m_akClearColors[uiTarget] = kColor;
}


//////////////////////////////////////////////////////////////////////////