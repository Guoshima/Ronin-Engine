#include "DX11RendererPCH.h"

#include "DX11ConstantBufferDesc.h"

//////////////////////////////////////////////////////////////////////////

CDX11ConstantBufferDesc::CDX11ConstantBufferDesc() :
m_sName(""), m_bPredefined(false), m_uiTotalSize(0)
{

}

CDX11ConstantBufferDesc::~CDX11ConstantBufferDesc()
{
	for (unsigned short us = 0; us < m_vVariables.size(); ++us)
		IntDelete m_vVariables[us];
	m_vVariables.clear();
	m_mVariables.clear();
}

//////////////////////////////////////////////////////////////////////////

bool CDX11ConstantBufferDesc::AddVariable(
	SVariableDesc* pkVariableDesc)
{
	if (pkVariableDesc == 0) return false;
	if (m_bPredefined && !pkVariableDesc->m_bPredefined) return false;

	m_bPredefined = pkVariableDesc->m_bPredefined;
	m_uiTotalSize += pkVariableDesc->m_uiSize;

	m_vVariables.push_back(pkVariableDesc);
	m_mVariables[pkVariableDesc->m_sName] = pkVariableDesc;

	return true;
}

const CDX11ConstantBufferDesc::SVariableDesc* 
	CDX11ConstantBufferDesc::GetVariableDesc(const stl_string& sName) const
{
	TVariableDescMap::const_iterator kIter = m_mVariables.find(sName);
	if (kIter != m_mVariables.end())
		return kIter->second;

	return 0;
}

//////////////////////////////////////////////////////////////////////////