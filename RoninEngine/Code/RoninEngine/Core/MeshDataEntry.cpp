#include "CorePCH.h"

#include "MeshDataEntry.h"

#include "MeshDataEntryDescriptor.h"
#include "MeshDataEntryCreator.h"

//////////////////////////////////////////////////////////////////////////

CMeshDataEntry* CMeshDataEntry::Create(
	const stl_vector<CMeshDataEntryDescriptorPtr>& kDescriptors,
	unsigned int uiCount, EMeshDataInputType eInputType)
{
	// use factory for this because this is platform dependend!
	return CMeshDataEntryCreator::CreateDataEntry(
		kDescriptors, uiCount, eInputType);
}

//////////////////////////////////////////////////////////////////////////

CMeshDataEntry::CMeshDataEntry(
	const stl_vector<CMeshDataEntryDescriptorPtr>& vDescriptors, 
	unsigned int uiCount, EMeshDataInputType eInputType) : 
m_uiCount(uiCount), m_uiStride(0), m_vDescriptors(vDescriptors),
m_uiSize(0), m_eInputType(eInputType)
{	
	// compute stride from descriptors
	for (unsigned int ui = 0; ui < GetDescriptorCount(); ++ui)
	{
		CMeshDataEntryDescriptor* pkDesc = m_vDescriptors[ui];
		m_uiStride += CMeshDataEntryDescriptor::GetSizeOfFormat(
			pkDesc->GetFormat());
	}

	m_uiSize = m_uiStride * m_uiCount;
}

CMeshDataEntry::~CMeshDataEntry() 
{
	// smart pointers
	m_vDescriptors.clear();
}

//////////////////////////////////////////////////////////////////////////

unsigned int CMeshDataEntry::GetDescriptorCount() const
{
	return (unsigned int) m_vDescriptors.size();
}

const CMeshDataEntryDescriptor* CMeshDataEntry::GetDescriptorAt(
	unsigned int uiIndex) const
{
	if (uiIndex < GetDescriptorCount())
		return m_vDescriptors[uiIndex];

	return 0;
}

//////////////////////////////////////////////////////////////////////////
