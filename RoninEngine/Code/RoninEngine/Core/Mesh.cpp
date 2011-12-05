#include "CorePCH.h"

#include "Mesh.h"

#include "MeshData.h"
#include "Material.h"
#include "MaterialData.h"
#include "RenderStateBlock.h"
#include "MeshRenderData.h"
#include "ShaderData.h"

#include "Renderer.h"

//////////////////////////////////////////////////////////////////////////

CMesh::CMesh() : m_pkMeshData(0), m_pkRenderStateBlock(0),
m_pkActiveMaterial(0), m_pkRenderData(0), m_bMaterialListDirty(false),
m_bRenderDataDirty(true)
{

}

CMesh::~CMesh() 
{
	m_pkMeshData = 0;
	m_pkRenderStateBlock = 0;
	m_pkRenderData = 0;

	m_mShaderDataMap.clear();
}

//////////////////////////////////////////////////////////////////////////

void CMesh::Render(CRenderer* pkRenderer)
{
	// make sure we have at least some mesh data
	if (m_pkMeshData == 0)
		return;

	pkRenderer->RenderMesh(this);
}

//////////////////////////////////////////////////////////////////////////

bool CMesh::SetActiveMaterial(CMaterial* pkMaterial)
{
	bool bFound = HasMaterial(pkMaterial);
	if (!bFound)
		return false;

	m_pkActiveMaterial = pkMaterial;

	return true;
}

void CMesh::AddMaterial(CMaterial* pkMaterial)
{
	// make sure it's not yet added
	if (!HasMaterial(pkMaterial))
	{
		m_vMaterials.push_back(pkMaterial);
		m_bMaterialListDirty = true;
	}
}

bool CMesh::RemoveMaterial(CMaterial* pkMaterial)
{
	for (unsigned short us = 0; us < m_vMaterials.size(); ++us)
	{
		if (m_vMaterials[us] == pkMaterial)
		{
			m_vMaterials[us] = m_vMaterials.back();
			m_vMaterials.pop_back();
			m_bMaterialListDirty = true;
			return true;
		}
	}

	return false;
}

bool CMesh::HasMaterial(CMaterial* pkMaterial) const
{
	for (unsigned short us = 0; us < m_vMaterials.size(); ++us)
	{
		if (m_vMaterials[us] == pkMaterial)
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void CMesh::AddShaderData(CShaderData* pkShaderData)
{
	CShaderData* pkOtherData = GetShaderData(pkShaderData->GetName());
	if (pkOtherData)
	{
		IntDelete pkOtherData;
	}

	m_mShaderDataMap[pkShaderData->GetName()] = pkShaderData;
	m_vShaderDataList.push_back(pkShaderData);
}

CShaderData* CMesh::GetShaderData(const stl_string& sName)
{
	TShaderDataMap::iterator kIter = m_mShaderDataMap.find(sName);
	if (kIter != m_mShaderDataMap.end())
		return kIter->second;

	return 0;
}

const CShaderData* CMesh::GetShaderData(const stl_string& sName) const
{
	TShaderDataMap::const_iterator kIter = m_mShaderDataMap.find(sName);
	if (kIter != m_mShaderDataMap.end())
		return kIter->second;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

CMesh::STextureSlot* CMesh::GetTextureSlot(
	unsigned int uiIndex)
{
	if (uiIndex >= MAX_TEXTURE_SLOTS)
		return 0;

	return &(m_akTextureSlots[uiIndex]);
}

const CMesh::STextureSlot* CMesh::GetTextureSlot(
	unsigned int uiIndex) const
{
	if (uiIndex >= MAX_TEXTURE_SLOTS)
		return 0;

	return &(m_akTextureSlots[uiIndex]);
}

void CMesh::SetTextureSlot(
	unsigned int uiIndex, CTexture* pkTexture, 
	CSamplerDesc* pkSamplerDesc)
{
	STextureSlot* pkTextureSlot = GetTextureSlot(uiIndex);
	if (pkTextureSlot == 0) return;

	pkTextureSlot->m_pkTexture = pkTexture;
	pkTextureSlot->m_pkSamplerDesc = pkSamplerDesc;
}

//////////////////////////////////////////////////////////////////////////