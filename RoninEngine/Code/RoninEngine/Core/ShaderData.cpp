#include "CorePCH.h"

#include "ShaderData.h"
#include "ShaderDataFactory.h"

//////////////////////////////////////////////////////////////////////////

CShaderData* CShaderData::CreateShaderData(
	const stl_string& sName, CShaderData::EShaderDataType eType, 
	unsigned int uiCount)
{
	return CShaderDataFactory::CreateShaderData(
		sName, eType, uiCount);
}

//////////////////////////////////////////////////////////////////////////

CShaderData::CShaderData(
	const stl_string& sName, EShaderDataType eType, unsigned int uiCount) : 
m_uiSize(0), m_sName(sName), m_eType(eType), m_uiCount(uiCount), m_acData(0)
{
	// compute new size & allocate the data
	m_uiSize += GetSizeOfType(m_eType) * m_uiCount;
	m_acData = (unsigned char*) IntMalloc(m_uiSize);
}

CShaderData::~CShaderData()
{
	IntFree(m_acData);
	m_acData = 0;
}

//////////////////////////////////////////////////////////////////////////

bool CShaderData::SetFloatValue(float fValue)
{
	float afValues[] = {fValue};
	return SetFloatValues(afValues, 1);
}

bool CShaderData::SetFloatValues(
	float* pfValues, unsigned int uiEntries, unsigned int uiOffset)
{
	if (m_eType != TYPE_FLOAT ||
		m_uiCount > uiEntries) 
		return false;

	return UpdateData((char*)pfValues, 
		uiEntries, uiOffset);
}

float CShaderData::GetFloatValue() const
{
	return GetFloatValueAt(0);
}

float CShaderData::GetFloatValueAt(unsigned int uiIndex) const
{
	return ((float*) m_acData)[uiIndex];
}

const float* CShaderData::GetFloatValues() const
{
	return (float*) m_acData;
}

//////////////////////////////////////////////////////////////////////////

bool CShaderData::SetIntValue(int iValue)
{
	int aiValues[] = {iValue};
	return SetIntValues(aiValues, 1);
}

bool CShaderData::SetIntValues(
	int* piValues, unsigned int uiEntries, unsigned int uiOffset)
{
	if (m_eType != TYPE_INT ||
		m_uiCount > uiEntries) 
		return false;

	return UpdateData((char*)piValues, 
		uiEntries, uiOffset);
}

int CShaderData::GetIntValue() const
{
	return (int) &m_acData;
}

int CShaderData::GetIntValueAt(unsigned int uiIndex) const
{
	return ((int*) m_acData)[uiIndex];
}

const int* CShaderData::GetIntValues() const
{
	return (int*) m_acData;
}

//////////////////////////////////////////////////////////////////////////

bool CShaderData::UpdateData(
	char* pkNewData, unsigned int uiSize, 
	unsigned int uiOffset)
{
	unsigned int uiSizeOfType = GetSizeOfType(m_eType);
	if ((uiOffset + uiSize) * uiSizeOfType > m_uiSize)
		return false;

	memcpy(m_acData + uiOffset * uiSizeOfType, 
		pkNewData, uiSize * uiSizeOfType);

	return true;
}

//////////////////////////////////////////////////////////////////////////

unsigned int CShaderData::GetSizeOfType(EShaderDataType eType)
{
	switch (eType)
	{
	case TYPE_FLOAT: return sizeof(float);
	case TYPE_INT: return sizeof(int);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////