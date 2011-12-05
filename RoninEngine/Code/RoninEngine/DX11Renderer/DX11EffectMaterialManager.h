#pragma once

#include "CoreMemObject.h"

class CDX11EffectMaterial;

class CDX11EffectMaterialManager : public CCoreMemObject
{
	friend class CDX11EffectLoader;

public:

	static void CreateInstance();
	static void DestroyInstance();

	CDX11EffectMaterial* GetEffectMaterial(
		const std::string& sEffectName);

private

	CDX11EffectMaterialManager();
	~CDX11EffectMaterialManager();

	void InsertEffect(CDX11EffectMaterial* pkEffectMaterial);

	//////////////////////////////////////////////////////////////////////////

	std::map<std::string, CDX11EffectMaterial*> m_vEffectMaterials;

};