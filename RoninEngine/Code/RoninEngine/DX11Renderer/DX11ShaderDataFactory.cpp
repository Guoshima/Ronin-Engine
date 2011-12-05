#include "DX11RendererPCH.h"

#include "DX11ShaderDataFactory.h"

#include "DX11ShaderData.h"

//////////////////////////////////////////////////////////////////////////

void CDX11ShaderDataFactory::CreateInstance()
{
	if (ms_pkInstance == 0)
		ms_pkInstance = IntNew CDX11ShaderDataFactory();
}

void CDX11ShaderDataFactory::DestroyInstance()
{
	IntDelete ms_pkInstance;
	ms_pkInstance = 0;
}

//////////////////////////////////////////////////////////////////////////

CShaderData* CDX11ShaderDataFactory::Do_CreateShaderData(
	const stl_string& sName, CShaderData::EShaderDataType eType, 
	unsigned int uiCount)
{	
	CDX11ShaderData* pkShaderData = IntNew CDX11ShaderData(
		sName, eType, uiCount);

	return pkShaderData;
}

//////////////////////////////////////////////////////////////////////////
