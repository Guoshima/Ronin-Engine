#include "CorePCH.h"

#include "MaterialManager.h"

#include "Material.h"

//////////////////////////////////////////////////////////////////////////

CMaterialManager::CMaterialManager()
{

}

CMaterialManager::~CMaterialManager()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////

void CMaterialManager::Clear()
{
	m_mMaterials.clear();
}

//////////////////////////////////////////////////////////////////////////

CMaterial* CMaterialManager::GetMaterial(
	const stl_string& sMaterialName)
{
	TMaterialMap::iterator kResult = m_mMaterials.find(sMaterialName);
	if (kResult != m_mMaterials.end())
		return kResult->second;

	return 0;
}

void CMaterialManager::InsertMaterial(CMaterial* pkMaterial)
{
	m_mMaterials[pkMaterial->GetName()] = pkMaterial;
}

//////////////////////////////////////////////////////////////////////////
