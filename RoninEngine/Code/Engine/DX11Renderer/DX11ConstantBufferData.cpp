#include "DX11RendererPCH.h"

#include "DX11ConstantBufferData.h"
#include "DX11ConstantBufferDesc.h"

//////////////////////////////////////////////////////////////////////////

CDX11ConstantBufferData::CDX11ConstantBufferData(
	const CDX11ConstantBufferDesc* pkConstantBufferDesc) : 
m_bDirty(true), m_pkDesc(pkConstantBufferDesc), m_acData(0)
{
	m_acData = (char*) IntMalloc(m_pkDesc->GetTotalSize());
}

CDX11ConstantBufferData::~CDX11ConstantBufferData() 
{
	IntFree(m_acData);
	m_acData = 0;
}

//////////////////////////////////////////////////////////////////////////

void CDX11ConstantBufferData::UpdateData(
	const stl_string& sName, void* acData)
{
	const CDX11ConstantBufferDesc::SVariableDesc* pkVariableDesc =
		m_pkDesc->GetVariableDesc(sName);
	if (pkVariableDesc)
	{
		memcpy(m_acData + pkVariableDesc->m_uiOffset, acData, 
			pkVariableDesc->m_uiSize);
	}
}

//////////////////////////////////////////////////////////////////////////