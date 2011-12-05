#pragma once

class CDX11RenderState;

class CDX11GlobalRenderState
{
public:

	CDX11GlobalRenderState(ID3D11Device* pkDevice);
	~CDX11GlobalRenderState();

	void ApplyRenderState(CDX11RenderState* pkRenderState);

private:

	//////////////////////////////////////////////////////////////////////////

	ID3D11Device* m_pkDevice;

};