#include "CorePCH.h"

#include "Material.h"

#include "MaterialManager.h"
#include "MaterialFactory.h"

//////////////////////////////////////////////////////////////////////////

CMaterial* CMaterial::CreateMaterial(
	const stl_string& sMaterialName)
{
	CMaterial* pkMaterial = 0;

	// first of all check the manager to see if we already have an instance
	CMaterialManager* pkMaterialManager = CCoreEngine::Get()->GetMaterialManager();
	if (pkMaterialManager)
	{
		pkMaterial = pkMaterialManager->GetMaterial(
			sMaterialName);
	}

	if (pkMaterial)
		return pkMaterial;

	// need to create a new material
	pkMaterial = CMaterialFactory::CreateMaterial(sMaterialName);

	// if the material is valid and we have a manager, insert it so we 
	// don't have to recreate it later again
	if (pkMaterial && pkMaterialManager)
	{
		pkMaterialManager->InsertMaterial(pkMaterial);
	}

	return pkMaterial;
}

//////////////////////////////////////////////////////////////////////////

CMaterial::CMaterial(const stl_string& sMaterialName) : 
	m_sMaterialName(sMaterialName)
{
}

CMaterial::~CMaterial() {

}

//////////////////////////////////////////////////////////////////////////