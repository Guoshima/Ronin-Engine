#pragma once

#include "CoreMemObject.h"

#include "ShaderData.h"

class CShaderDataFactory : public CCoreMemObject
{
public:

	static CShaderData* CreateShaderData(const stl_string& sName, 
		CShaderData::EShaderDataType eType, unsigned int uiCount);

protected:

	virtual CShaderData* Do_CreateShaderData(const stl_string& sName, 
		CShaderData::EShaderDataType eType, unsigned int uiCount) = 0;

	//////////////////////////////////////////////////////////////////////////

	static CShaderDataFactory* ms_pkInstance;
};