#pragma once

class CDX11RenderTarget
{
public:

	CDX11RenderTarget();
	~CDX11RenderTarget();

	bool Init(unsigned int uiNumOfTargets, 
		ID3D11RenderTargetView** ppkRenderTargetViews,
		ID3D11DepthStencilView* pkDepthStencilView, 
		unsigned int uiWidth, unsigned int uiHeight);

	void Clear(ID3D11DeviceContext* pkContext, 
		unsigned int uiClearMode);
	bool BindRenderTarget(ID3D11DeviceContext* pkContext, 
		unsigned int uiClearMode);

	unsigned int GetWidth() const { return m_uiWidth; }
	unsigned int GetHeight() const { return m_uiHeight; }

private:

	//////////////////////////////////////////////////////////////////////////

	unsigned int m_uiWidth;
	unsigned int m_uiHeight;

	unsigned int m_uiNumOfTargets;
	ID3D11RenderTargetView** m_ppkRenderTargetViews;
	ID3D11DepthStencilView* m_pkDepthStencilView;

};