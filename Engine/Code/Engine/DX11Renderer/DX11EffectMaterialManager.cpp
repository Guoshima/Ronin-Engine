#include "DX11RendererPCH.h"

#include "DX11EffectMaterial.h"

#include "DX11Effect.h"
#include "DX11Technique.h"

#include <d3dx11effect.h>

//////////////////////////////////////////////////////////////////////////

CDX11EffectMaterial::CDX11EffectMaterial() : 
m_pkEffect(0), m_pkTechnique(0)
{

}

CDX11EffectMaterial::~CDX11EffectMaterial()
{
	// nothing special
}

//////////////////////////////////////////////////////////////////////////

unsigned int CDX11EffectMaterial::GetPassCount()
{
	if (m_pkTechnique)
		return m_pkTechnique->GetPassCount();

	return 0;
}

void CDX11EffectMaterial::ApplyPass(unsigned int uiPassID, 
	ID3D11DeviceContext* pkDeviceContext)
{
	if (m_pkTechnique)
		m_pkTechnique->GetPass(uiPassID)->Apply(0, pkDeviceContext);
}

//////////////////////////////////////////////////////////////////////////