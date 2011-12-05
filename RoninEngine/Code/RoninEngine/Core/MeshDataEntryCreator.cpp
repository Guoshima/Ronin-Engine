#include "CorePCH.h"

#include "MeshDataEntryCreator.h"

//////////////////////////////////////////////////////////////////////////

CMeshDataEntryCreator* CMeshDataEntryCreator::ms_pkInstance = 0;

//////////////////////////////////////////////////////////////////////////

CMeshDataEntry* CMeshDataEntryCreator::CreateDataEntry(
	const stl_vector<CMeshDataEntryDescriptorPtr>& kDescriptors,
	unsigned short uiCount, CMeshDataEntry::EMeshDataInputType eInputType)
	
{
	if (ms_pkInstance == 0)
		return 0;

	return ms_pkInstance->Do_CreateDataEntry(
		kDescriptors, uiCount, eInputType);
}

//////////////////////////////////////////////////////////////////////////
