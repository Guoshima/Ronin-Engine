#pragma once

#include "MeshDataEntryCreator.h"

class CDX11MeshDataEntryCreator : public CMeshDataEntryCreator
{
public:

	static void CreateInstance();
	static void DestroyInstance();

protected:

	virtual CMeshDataEntry* Do_CreateDataEntry(
		const stl_vector<CMeshDataEntryDescriptorPtr>& kDescriptors,
		unsigned short uiCount, CMeshDataEntry::EMeshDataInputType eInputType);
};