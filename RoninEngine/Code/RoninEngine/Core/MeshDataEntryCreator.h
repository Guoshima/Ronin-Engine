#pragma once

#include "CoreMemObject.h"

#include "MeshDataEntry.h"

class CMeshDataEntryCreator : public CCoreMemObject
{
public:

	static CMeshDataEntry* CreateDataEntry(
		const stl_vector<CMeshDataEntryDescriptorPtr>& kDescriptors,
		unsigned short uiCount, CMeshDataEntry::EMeshDataInputType eInputType);

protected:

	virtual CMeshDataEntry* Do_CreateDataEntry(
		const stl_vector<CMeshDataEntryDescriptorPtr>& kDescriptors,
		unsigned short uiCount, CMeshDataEntry::EMeshDataInputType eInputType) = 0;

	//////////////////////////////////////////////////////////////////////////

	static CMeshDataEntryCreator* ms_pkInstance;
};