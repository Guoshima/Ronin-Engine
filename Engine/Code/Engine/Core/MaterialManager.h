#pragma once

class CMaterial;
CoreSmartPointer(CMaterial);

class CMaterialManager : public CCoreMemObject
{
	// only core engine can create or destroy a materialmanager
	friend class CCoreEngine;

public:

	typedef stl_map<stl_string, CMaterialPtr> TMaterialMap;

	CMaterial* GetMaterial(const stl_string& sMaterialName);
	void InsertMaterial(CMaterial* pkMaterial);

	TMaterialMap& GetMaterials() { return m_mMaterials; }
	void Clear();
 
protected:

	CMaterialManager();
	~CMaterialManager();

	//////////////////////////////////////////////////////////////////////////

	TMaterialMap m_mMaterials;
};