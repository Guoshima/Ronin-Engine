#pragma once

#include "CoreMemObject.h"

#include "Material.h"

class CMaterialFactory : public CCoreMemObject
{
public:

	static CMaterial* CreateMaterial(const stl_string& sMaterialName);

protected:

	virtual CMaterial* Do_CreateMaterial(const stl_string& sMaterialName) = 0;

	//////////////////////////////////////////////////////////////////////////

	static CMaterialFactory* ms_pkInstance;
};