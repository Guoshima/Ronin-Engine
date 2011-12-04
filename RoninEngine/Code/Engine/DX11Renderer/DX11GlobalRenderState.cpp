#include "DX11RendererPCH.h"

#include "DX11GlobalRenderState.h"
#include "DX11RenderState.h"

//////////////////////////////////////////////////////////////////////////

CDX11GlobalRenderState::CDX11GlobalRenderState(
	ID3D11Device* pkDevice) : m_pkDevice(pkDevice)
{

}

CDX11GlobalRenderState::~CDX11GlobalRenderState()
{

}

//////////////////////////////////////////////////////////////////////////

void CDX11GlobalRenderState::ApplyRenderState(
	CDX11RenderState* pkRenderState)
{

}

//////////////////////////////////////////////////////////////////////////