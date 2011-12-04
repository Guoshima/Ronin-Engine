#include "DX11RendererPCH.h"

#include "DX11Material.h"

#include "DX11ConstantBuffer.h"

//////////////////////////////////////////////////////////////////////////

CDX11Material::CDX11Material(const stl_string& sMaterialName) : 
CMaterial(sMaterialName), m_pkInputLayout(0)
{

}

CDX11Material::~CDX11Material() {
}

//////////////////////////////////////////////////////////////////////////

void CDX11Material::UpdateConstantBufferData(
	CDX11Renderer* pkRenderer, CMesh* pkMesh)
{

	for (unsigned short us = 0; us < m_vConstantBuffers.size(); ++us)
	{
		CDX11ConstantBuffer* pkCB = m_vConstantBuffers[us];
		pkCB->UpdateD3DData(pkRenderer, pkMesh);		
	}
}

//////////////////////////////////////////////////////////////////////////
