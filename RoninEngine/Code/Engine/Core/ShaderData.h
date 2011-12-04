#pragma once

#include "CoreMemObject.h"

class CShaderData : public CCoreRefMTObject
{
public:
	
	enum EShaderDataType
	{
		TYPE_INVALID,
		TYPE_FLOAT,
		TYPE_INT,
		MAX_TYPES,
	};

	static unsigned int GetSizeOfType(EShaderDataType eType);

	static CShaderData* CreateShaderData(const stl_string& sName, 
		CShaderData::EShaderDataType eType, unsigned int uiCount);

	//////////////////////////////////////////////////////////////////////////

	virtual ~CShaderData();

	const stl_string& GetName() const { return m_sName;}
	unsigned int GetEntryCount() const { return m_uiCount; }
	unsigned int GetSize() const { return m_uiSize; }
 
	bool SetFloatValue(float fValue);
	bool SetFloatValues(float* pfValues, unsigned int uiEntries, 
		unsigned int uiOffset = 0);
	float GetFloatValue() const;
	float GetFloatValueAt(unsigned int uiIndex) const;
	const float* GetFloatValues() const;

	bool SetIntValue(int iValue);
	bool SetIntValues(int* piValues, unsigned int uiEntries, 
		unsigned int uiOffset = 0);
	int GetIntValue() const;
	int GetIntValueAt(unsigned int uiIndex) const;
	const int* GetIntValues() const;

protected:

	virtual bool UpdateData(char* pkNewData, 
		unsigned int uiSize, unsigned int uiOffset);

	CShaderData(const stl_string& sName, 
		EShaderDataType eType, unsigned int uiCount);

	//////////////////////////////////////////////////////////////////////////

	// unique entry name in this shaderdata
	stl_string m_sName;
	// type of shader data
	EShaderDataType m_eType;
	// amount of entries of type (float4 -> 4)
	unsigned int m_uiCount;

	// total size of the entry
	unsigned int m_uiSize;

	// the actual shader data
	unsigned char* m_acData;
};

CoreSmartPointer(CShaderData);