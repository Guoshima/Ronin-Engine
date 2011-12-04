#include "DX11RendererPCH.h"

#include "DX11MeshDataEntryCreator.h"

#include "DX11MeshDataEntry.h"

//////////////////////////////////////////////////////////////////////////

void CDX11MeshDataEntryCreator::CreateInstance()
{
	if (ms_pkInstance == 0)
		ms_pkInstance = IntNew CDX11MeshDataEntryCreator();
}

void CDX11MeshDataEntryCreator::DestroyInstance()
{
	IntDelete ms_pkInstance;
	ms_pkInstance = 0;
}

//////////////////////////////////////////////////////////////////////////

CMeshDataEntry* CDX11MeshDataEntryCreator::Do_CreateDataEntry(
	const stl_vector<CMeshDataEntryDescriptorPtr>& vDescriptors,
	unsigned short uiCount, CMeshDataEntry::EMeshDataInputType eInputType)
{
	CDX11MeshDataEntry* pkDataEntry = IntNew CDX11MeshDataEntry(
		vDescriptors, uiCount, eInputType);

	return pkDataEntry;
}

//////////////////////////////////////////////////////////////////////////
