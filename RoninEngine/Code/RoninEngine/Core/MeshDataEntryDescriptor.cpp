#include "CorePCH.h"

#include "MeshDataEntryDescriptor.h"

//////////////////////////////////////////////////////////////////////////

unsigned int CMeshDataEntryDescriptor::GetSizeOfFormat(
	EMeshDataEntryFormat eFormat)
{
	switch (eFormat) {
		case FORMAT_UINT_16_X: return (1 * 2);
		case FORMAT_UINT_32_X: return (1 * 4);
		case FORMAT_FLOAT_16_XYZ: return (3 * 2);
		case FORMAT_FLOAT_32_XYZ: return (3 * 4);
		case FORMAT_FLOAT_16_XY: return (2 * 4);
		case FORMAT_FLOAT_32_XY: return (2 * 4);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////