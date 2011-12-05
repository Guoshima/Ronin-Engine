#include "DX11RendererPCH.h"

#include "DX11RenderTarget.h"

//////////////////////////////////////////////////////////////////////////

CDX11RenderTarget::CDX11RenderTarget() : m_uiNumOfTargets(0),
	m_ppkRenderTargetViews(0), m_pkDepthStencilView(0),
	m_uiHeight(0), m_uiWidth(0)
{

}

CDX11RenderTarget::~CDX11RenderTarget()
{
	for (unsigned int ui = 0; ui < m_uiNumOfTargets; ++ui)
		m_ppkRenderTargetViews[ui]->Release();
	delete m_ppkRenderTargetViews;

	if (m_pkDepthStencilView)
		m_pkDepthStencilView->Release();
}

bool CDX11RenderTarget::Init(unsigned int uiNumOfTargets, 
	ID3D11RenderTargetView** ppkRenderTargetViews, 
	ID3D11DepthStencilView* pkDepthStencilView, 
	unsigned int uiWidth, unsigned int uiHeight)
{
	m_uiNumOfTargets = uiNumOfTargets;
	m_ppkRenderTargetViews = ppkRenderTargetViews;
	m_pkDepthStencilView = pkDepthStencilView;

	m_uiWidth = uiWidth;
	m_uiHeight = uiHeight;

	return true;
}

bool CDX11RenderTarget::BindRenderTarget(
	ID3D11DeviceContext* pkContext, unsigned int uiClearMode)
{
	pkContext->OMSetRenderTargets(m_uiNumOfTargets,
		m_ppkRenderTargetViews, m_pkDepthStencilView);

	Clear(pkContext, uiClearMode);

	return true;
}

void CDX11RenderTarget::Clear(
	ID3D11DeviceContext* pkContext, unsigned int uiClearMode)
{
	/*
	if (uiClearMode == 0)
		return;
	*/

	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
	pkContext->ClearRenderTargetView( 
		m_ppkRenderTargetViews[0], ClearColor );
}

//////////////////////////////////////////////////////////////////////////