#pragma once

#include "SceneGraphObject.h"
#include "Texture.h"
#include "SamplerDesc.h"

class CRenderer;

class CMeshData;
class CRenderStateBlock;
class CMaterial;
class CMeshRenderData;
class CShaderData;

CoreSmartPointer(CMeshData);
CoreSmartPointer(CRenderStateBlock);
CoreSmartPointer(CMaterial);
CoreSmartPointer(CMeshRenderData);
CoreSmartPointer(CShaderData);

#define MAX_TEXTURE_SLOTS 16

class CMesh : public CSceneGraphObject
{
public:
	
	CMesh();
	virtual ~CMesh();

	void Render(CRenderer* pkRenderer);

	CMeshData* GetMeshData() { return m_pkMeshData; }
	void SetMeshData(CMeshData* pkMeshData) { m_pkMeshData = pkMeshData; }

	CMaterial* GetActiveMaterial() { return m_pkActiveMaterial; }
	bool SetActiveMaterial(CMaterial* pkMaterial);
	void AddMaterial(CMaterial* pkMaterial);
	bool RemoveMaterial(CMaterial* pkMaterial);
	bool HasMaterial(CMaterial* pkMaterial) const;

	bool IsMaterialListDirty() const { return m_bMaterialListDirty; }
	void ResetMaterialListDirty() { m_bMaterialListDirty = false; }
	stl_vector<CMaterialPtr> GetMaterialList() { return m_vMaterials; }

	CMeshRenderData* GetRenderData() { return m_pkRenderData; }
	void SetRenderData(CMeshRenderData* pkRenderData) { m_pkRenderData = pkRenderData; }

	bool IsRenderDataDirty() const { return m_bRenderDataDirty; }
	void ForceRenderDataDirty() { m_bRenderDataDirty = true; }
	void ResetRenderDataDirty() { m_bRenderDataDirty = false; }

	void AddShaderData(CShaderData* pkShaderData);

	unsigned int GetShaderDataCount() const;
	stl_vector<CShaderDataPtr>& GetShaderDataList() { return m_vShaderDataList; }
	CShaderData* GetShaderData(const stl_string& sName);
	const CShaderData* GetShaderData(const stl_string& sName) const;

	struct STextureSlot : public CCoreMemObject
	{
		STextureSlot() : m_pkTexture(0), m_pkSamplerDesc(0)
		{
		}
		CTexturePtr m_pkTexture;
		CSamplerDescPtr m_pkSamplerDesc;
	};
	STextureSlot* GetTextureSlot(unsigned int uiIndex);
	const STextureSlot* GetTextureSlot(unsigned int uiIndex) const;
	void SetTextureSlot(unsigned int uiIndex, CTexture* pkTexture, 
		CSamplerDesc* pkSamplerDesc);

	CRenderStateBlock* GetRenderStateBlock() { return m_pkRenderStateBlock; }

protected:

	CMeshDataPtr m_pkMeshData;
	CRenderStateBlockPtr m_pkRenderStateBlock;

	CMaterialPtr m_pkActiveMaterial;
	stl_vector<CMaterialPtr> m_vMaterials;
	bool m_bMaterialListDirty;

	typedef stl_map<stl_string, CShaderDataPtr> TShaderDataMap;
	typedef stl_vector<CShaderDataPtr> TShaderDataList;
	TShaderDataMap m_mShaderDataMap;
	TShaderDataList m_vShaderDataList;

	STextureSlot m_akTextureSlots[MAX_TEXTURE_SLOTS];

	bool m_bRenderDataDirty;
	CMeshRenderDataPtr m_pkRenderData;
};

CoreSmartPointer(CMesh);