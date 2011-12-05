#pragma once

#include "CoreMemObject.h"

class CMeshDataEntryDescriptor : public CCoreRefMTObject
{
public:

	enum EMeshDataSemantic
	{
		SEMANTIC_INDEX,
		SEMANTIC_POSITION,
		SEMANTIC_NORMAL,
		SEMANTIC_BINORMAL,
		SEMANTIC_TANGENT,
		SEMANTIC_TEXCOORD,
		SEMANTIC_COLOR,
		SEMANTIC_COUNT
	};

	enum EMeshDataEntryFormat
	{
		FORMAT_UINT_16_X,
		FORMAT_UINT_32_X,
		FORMAT_FLOAT_16_XYZ,
		FORMAT_FLOAT_32_XYZ,
		FORMAT_FLOAT_16_XY,
		FORMAT_FLOAT_32_XY,
		FORMAT_COUNT,
	};

	static unsigned int GetSizeOfFormat(EMeshDataEntryFormat eFormat);

	CMeshDataEntryDescriptor() {}
	CMeshDataEntryDescriptor(EMeshDataSemantic eSemantic, unsigned int uiSemanticIndex,
		EMeshDataEntryFormat eFormat) :
	m_eSemantic(eSemantic), m_eFormat(eFormat), m_uiSemanticIndex(uiSemanticIndex)
	{}

	~CMeshDataEntryDescriptor() {}
	
	EMeshDataSemantic GetSemantic() const { return m_eSemantic; }
	unsigned int GetSemanticIndex() const { return m_uiSemanticIndex; }
	EMeshDataEntryFormat GetFormat() const { return m_eFormat; }

protected:

	//////////////////////////////////////////////////////////////////////////

	EMeshDataSemantic m_eSemantic;
	unsigned int m_uiSemanticIndex;
	EMeshDataEntryFormat m_eFormat;

};

CoreSmartPointer(CMeshDataEntryDescriptor);