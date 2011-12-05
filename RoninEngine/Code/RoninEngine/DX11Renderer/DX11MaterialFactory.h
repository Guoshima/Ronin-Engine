#pragma once

#include "MaterialFactory.h"

class CDX11MaterialFactory : public CMaterialFactory
{
public:

	static void CreateInstance();
	static void DestroyInstance();

protected:

	virtual CMaterial* Do_CreateMaterial(
		const stl_string& sMaterialName);
};