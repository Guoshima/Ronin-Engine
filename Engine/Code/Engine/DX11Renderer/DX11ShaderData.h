#pragma once

#include <ShaderData.h>

#include "DX11ConstantBufferData.h"

class CDX11ConstantBufferDesc;

class CDX11ShaderData : public CShaderData
{
public:
	
	CDX11ShaderData(const stl_string& sName, 
		EShaderDataType eType, unsigned int uiCount);
	virtual ~CDX11ShaderData() {}

	CDX11ConstantBufferData* GetConstantBufferData(
		const CDX11ConstantBufferDesc* pkDesc);
	bool AddConstantBufferData(CDX11ConstantBufferData* pkData);
	CDX11ConstantBufferData* RemoveConstantBufferData(
		const CDX11ConstantBufferDesc* pkDesc);
	void UpdateConstantBufferData();

protected:

	// overload the update data to also update the constant buffers imm.
	virtual bool UpdateData(char* pkNewData, 
		unsigned int uiSize, unsigned int uiOffset);

	//////////////////////////////////////////////////////////////////////////

	// all constant buffers which should be updated if this value is changed
	stl_vector<CDX11ConstantBufferDataPtr> m_vConstantBufferDataList;
};