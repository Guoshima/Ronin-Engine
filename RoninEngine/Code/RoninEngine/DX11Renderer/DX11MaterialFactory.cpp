#include "DX11RendererPCH.h"

#include "DX11MaterialFactory.h"

#include "DX11Renderer.h"
#include "DX11EffectLoader.h"
#include "DX11EffectMaterial.h"

//////////////////////////////////////////////////////////////////////////

void CDX11MaterialFactory::CreateInstance()
{
	if (ms_pkInstance == 0)
		ms_pkInstance = IntNew CDX11MaterialFactory();
}

void CDX11MaterialFactory::DestroyInstance()
{
	IntDelete ms_pkInstance;
	ms_pkInstance = 0;
}

//////////////////////////////////////////////////////////////////////////

CMaterial* CDX11MaterialFactory::Do_CreateMaterial(
	const stl_string& sMaterialName)
{
	CDX11Renderer* pkDX11Renderer = CDX11Renderer::GetDX11Renderer();
	if (pkDX11Renderer == 0) return 0;

	CDX11EffectManager* pkDX11EffectManager = 
		pkDX11Renderer->GetEffectManager();
	if (pkDX11EffectManager == 0) return 0;

	CDX11EffectMaterial* pkEffectMaterial = 
		pkDX11EffectManager->CreateEffectMaterial(sMaterialName);

	return pkEffectMaterial;
}

//////////////////////////////////////////////////////////////////////////
