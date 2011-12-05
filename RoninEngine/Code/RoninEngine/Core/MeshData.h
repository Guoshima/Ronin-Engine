#pragma once

#include "CoreMemObject.h"

class CMeshDataEntry;
CoreSmartPointer(CMeshDataEntry);

class CMeshData : public CCoreRefMTObject
{
public:

	enum ETopology 
	{
		TOPOLOGY_UNDEFINED,
		TOPOLOGY_TRIANGLE_LIST,
		TOPOLOGY_TRIANGLE_STRIP,
	};
	
	CMeshData(CMeshDataEntry* pkIndexData, CMeshDataEntry* pkVertexData);
	virtual ~CMeshData();

	CMeshDataEntry* GetIndexBuffer() { return m_pkIndexData; }
	CMeshDataEntry* GetVertexBuffer() { return m_pkVertexData; }

	void SetTopology(ETopology eTopology) { m_eTopology = eTopology; }
	ETopology GetTopology() const { return m_eTopology; }

protected:

	//////////////////////////////////////////////////////////////////////////

	ETopology m_eTopology;

	CMeshDataEntryPtr m_pkIndexData;
	CMeshDataEntryPtr m_pkVertexData;
};

CoreSmartPointer(CMeshData);