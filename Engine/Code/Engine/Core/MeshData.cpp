#include "CorePCH.h"

#include "MeshData.h"

#include "MeshDataEntry.h"

//////////////////////////////////////////////////////////////////////////

CMeshData::CMeshData(CMeshDataEntry* pkIndexData, 
					 CMeshDataEntry* pkVertexData) :
m_pkIndexData(pkIndexData), m_pkVertexData(pkVertexData),
m_eTopology(TOPOLOGY_UNDEFINED)
{

}

CMeshData::~CMeshData()
{
	m_pkIndexData = 0;
	m_pkVertexData = 0;
}

//////////////////////////////////////////////////////////////////////////