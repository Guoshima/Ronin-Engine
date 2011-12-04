#pragma once

#include "CoreMemObject.h"

class CMaterial : public CCoreRefMTObject
{
public:
	
	static CMaterial* CreateMaterial(const stl_string& sMaterialName);

	virtual ~CMaterial();
	const stl_string& GetName() const { return m_sMaterialName; }

protected:

	CMaterial(const stl_string& sMaterialName);

	//////////////////////////////////////////////////////////////////////////

	stl_string m_sMaterialName;
};

CoreSmartPointer(CMaterial);