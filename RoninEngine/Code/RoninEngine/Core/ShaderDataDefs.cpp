#include "CorePCH.h"

#include "ShaderData.h"

#include "GarbageProcessor.h"

//////////////////////////////////////////////////////////////////////////

CShaderData::CShaderData(const std::string& sName, 
						 const TEntryVector& vEntries) : 
m_uiSize(0), m_bDirty(true), m_sName(sName)
{
	FillMap(vEntries);
}

CShaderData::~CShaderData()
{
	CGarbageProcessor* pkGarbageProcessor = CGarbageProcessor::GetInstance();
	for (TEntryMap::iterator kIter = m_mShaderEntries.begin(); 
		kIter != m_mShaderEntries.end(); ++kIter)
	{
		if (pkGarbageProcessor)
			pkGarbageProcessor->RequestDelete(kIter->second);
		else
			delete kIter->second;
	}
}

//////////////////////////////////////////////////////////////////////////

unsigned int CShaderData::GetEntryCount() const
{
	return m_mShaderEntries.size();
}

const CShaderData::SShaderDataEntry* CShaderData::GetEntry(
	const std::string& sName) const
{
	TEntryMap::const_iterator kFound = m_mShaderEntries.find(sName);
	if (kFound != m_mShaderEntries.end())
		return kFound->second;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CShaderData::FillMap(const TEntryVector& vEntries)
{
	unsigned int uiOffset = 0;
	for (TEntryVector::const_iterator kIter = vEntries.begin(); 
		kIter < vEntries.end(); ++kIter)
	{
		SShaderDataEntry* pkEntry = *kIter;

		pkEntry->m_uiOffset = uiOffset;

		TEntryMap::iterator kFound = 
			m_mShaderEntries.find(pkEntry->m_sName);
		if (kFound != m_mShaderEntries.end())
		{
			//assert(0);
			continue;
		}

		m_mShaderEntries[pkEntry->m_sName] = pkEntry;

		// compute new offset
		uiOffset += GetSizeOfType(pkEntry->m_eType) 
			* pkEntry->m_uiCount;
	}

	// final offset is the total size!
	m_uiSize = uiOffset;
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