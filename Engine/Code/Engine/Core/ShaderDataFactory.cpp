#include "CorePCH.h"

#include "ShaderDataFactory.h"

//////////////////////////////////////////////////////////////////////////

CShaderDataFactory* CShaderDataFactory::ms_pkInstance = 0;

//////////////////////////////////////////////////////////////////////////

CShaderData* CShaderDataFactory::CreateShaderData(
	const stl_string& sName, CShaderData::EShaderDataType eType, 
	unsigned int uiCount)

{
	if (ms_pkInstance == 0)
		return 0;

	return ms_pkInstance->Do_CreateShaderData(sName, eType, uiCount);
}

//////////////////////////////////////////////////////////////////////////
