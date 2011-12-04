#include "DX11RendererPCH.h"

#include "DX11EffectMaterial.h"

#include "DX11EffectData.h"
#include "DX11ConstantBuffer.h"

#include <Mesh.h>
#include <MeshData.h>
#include <MeshRenderData.h>
#include <Texture.h>

#include "DX11TextureData2D.h"

#include <D3DX10math.h>

//////////////////////////////////////////////////////////////////////////

CDX11EffectMaterial::CDX11EffectMaterial(const stl_string& sMaterialName) : 
	CDX11Material(sMaterialName)
{
}

CDX11EffectMaterial::~CDX11EffectMaterial()
{
	// nothing special
}

//////////////////////////////////////////////////////////////////////////

void CDX11EffectMaterial::SetEffectData(CDX11EffectData* pkEffectData)
{
	m_spEffectData = pkEffectData;

	// retrieve constant buffers from the effect data
	const stl_vector<CDX11ConstantBufferPtr> vConstantBuffers =
		pkEffectData->GetConstanBufferList();
	for (unsigned short us = 0; us < vConstantBuffers.size(); ++us)
	{
		m_vConstantBuffers.push_back(vConstantBuffers[us]);
	}
}

void CDX11EffectMaterial::ReleaseEffectData()
{
	m_vConstantBuffers.clear();
	m_spEffectData = 0;
}

//////////////////////////////////////////////////////////////////////////

unsigned int CDX11EffectMaterial::GetPassCount()
{
	if (m_spEffectData)
		return m_spEffectData->GetPassCount();

	return 0;
}

void CDX11EffectMaterial::ApplyPass(unsigned int uiPassID, 
	ID3D11DeviceContext* pkDeviceContext)
{
	if (m_spEffectData)
		m_spEffectData->GetPass(uiPassID)->Apply(0, pkDeviceContext);
}

//////////////////////////////////////////////////////////////////////////

const void* CDX11EffectMaterial::GetInputSignatureData(
	unsigned int& uiSize)
{
	if (m_spEffectData == 0)
		return 0;

	return m_spEffectData->GetInputSignatureData(uiSize);
}

//////////////////////////////////////////////////////////////////////////

void CDX11EffectMaterial::ApplyTextures(
	CDX11Renderer* pkRenderer, CMesh* pkMesh)
{
	for (unsigned short us = 0; us < MAX_TEXTURE_SLOTS; ++us)
	{
		ID3DX11EffectShaderResourceVariable* pkShaderResource = 
			m_spEffectData->GetShaderResourceAt(us);
		if (pkShaderResource)
		{
			const CMesh::STextureSlot* pkTextureSlot = 
				pkMesh->GetTextureSlot(us);
			if (pkTextureSlot && pkTextureSlot->m_pkTexture)
			{
				CDX11TextureData2D* pkTextureData = (CDX11TextureData2D*)
					pkTextureSlot->m_pkTexture->GetTextureData();

				if (pkTextureData)
				{
					pkShaderResource->SetResource(
						pkTextureData->GetShaderResourceView());
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////