#pragma once

#include "ShaderDataFactory.h"

class CDX11ShaderDataFactory : public CShaderDataFactory
{
public:

	static void CreateInstance();
	static void DestroyInstance();

protected:

	virtual CShaderData* Do_CreateShaderData(const stl_string& sName, 
		CShaderData::EShaderDataType eType, unsigned int uiCount);
};