#include "DX11RendererPCH.h"

#include "DX11ShaderData.h"

#include "DX11ConstantBufferData.h"

//////////////////////////////////////////////////////////////////////////

CDX11ShaderData::CDX11ShaderData(
		const stl_string& sName, EShaderDataType eType, unsigned int uiCount) : 
CShaderData(sName, eType, uiCount)
{
}

//////////////////////////////////////////////////////////////////////////

bool CDX11ShaderData::UpdateData(char* pkNewData, 
	unsigned int uiSize, unsigned int uiOffset)
{
	// update the local data first
	CShaderData::UpdateData(pkNewData, uiSize, uiOffset);

	// now copy the new data at the correct spots in the constant buffers
	UpdateConstantBufferData();

	return true;
}

//////////////////////////////////////////////////////////////////////////

CDX11ConstantBufferData* CDX11ShaderData::GetConstantBufferData(
	const CDX11ConstantBufferDesc* pkDesc)
{
	for (unsigned short us = 0; us < m_vConstantBufferDataList.size(); ++us)
	{
		CDX11ConstantBufferData* pkCBData = m_vConstantBufferDataList[us];
		if (pkCBData->GetDesc() == pkDesc)
			return pkCBData;
	}

	return 0;
}

bool CDX11ShaderData::AddConstantBufferData(
	CDX11ConstantBufferData* pkData)
{
	if (GetConstantBufferData(pkData->GetDesc()))
		return false;

	m_vConstantBufferDataList.push_back(pkData);

	// update the local data
	pkData->UpdateData(m_sName, m_acData);

	return true;
}

CDX11ConstantBufferData* CDX11ShaderData::RemoveConstantBufferData(
	const CDX11ConstantBufferDesc* pkDesc)
{
	for (unsigned short us = 0; us < m_vConstantBufferDataList.size(); ++us)
	{
		CDX11ConstantBufferData* pkCBData = m_vConstantBufferDataList[us];
		if (pkCBData->GetDesc() == pkDesc)
		{
			m_vConstantBufferDataList[us] = m_vConstantBufferDataList.back();
			m_vConstantBufferDataList.pop_back();
			return pkCBData;
		}
	}

	return 0;
}

void CDX11ShaderData::UpdateConstantBufferData()
{
	for (unsigned short us = 0; us < m_vConstantBufferDataList.size(); ++us)
	{
		CDX11ConstantBufferData* pkCBData = m_vConstantBufferDataList[us];
		pkCBData->UpdateData(m_sName, m_acData);
	}
}

//////////////////////////////////////////////////////////////////////////