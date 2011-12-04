#pragma once

#include "CoreMemObject.h"

class CMeshDataEntryDescriptor;
CoreSmartPointer(CMeshDataEntryDescriptor);

class CMeshDataEntry : public CCoreRefMTObject
{
public:

	enum EMeshDataInputType
	{
		TYPE_VERTEX_DATA,
		TYPE_INDEX_DATA,
		TYPE_COUNT,
	};
	
	static CMeshDataEntry* Create(const stl_vector<CMeshDataEntryDescriptorPtr>& kDescriptors,
		unsigned int uiCount, EMeshDataInputType eInputType);
	virtual ~CMeshDataEntry();

	unsigned int GetCount() const { return m_uiCount; }
	unsigned int GetStride() const { return m_uiStride; }
	unsigned int GetSize() const { return m_uiSize; }
	EMeshDataInputType GetInputType() const { return m_eInputType; }
 
	virtual void* LockData() = 0;
	virtual void UnLockData() = 0;

	unsigned int GetDescriptorCount() const;
	const CMeshDataEntryDescriptor* GetDescriptorAt(unsigned int uiIndex) const;

protected:

	CMeshDataEntry(const stl_vector<CMeshDataEntryDescriptorPtr>& kDescriptors,
		unsigned int uiCount, EMeshDataInputType eInputType);

	//////////////////////////////////////////////////////////////////////////

	stl_vector<CMeshDataEntryDescriptorPtr> m_vDescriptors;

	unsigned int m_uiCount;
	unsigned int m_uiSize;
	unsigned int m_uiStride;

	EMeshDataInputType m_eInputType;
};

CoreSmartPointer(CMeshDataEntry);