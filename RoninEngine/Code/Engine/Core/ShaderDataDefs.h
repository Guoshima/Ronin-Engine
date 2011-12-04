#pragma once

#include "CoreMemObject.h"

class CShaderData : public CCoreMemObject
{
public:
	
	enum EShaderDataType
	{
		TYPE_INVALID,
		TYPE_FLOAT,
		TYPE_INT,
		MAX_TYPES,
	};

	enum EShaderDataPredefined
	{
		PREDEFINED_INVALID,
		PREDEFINED_PROJECTION,
		PREDEFINED_VIEW,
		PREDEFINED_WORLD,
		MAX_PREDEFINED,
	};

	static unsigned int GetSizeOfType(EShaderDataType eType);

	struct SShaderDataEntry : public CCoreMemObject
	{
		SShaderDataEntry() : m_sName(""), m_eType(TYPE_INVALID),
			m_uiCount(0), m_uiOffset(0), m_uiSize(0),
			m_bPredefined(false), m_ePredefinedType(PREDEFINED_INVALID)
		{}

		// unique entry name in this shaderdata
		std::string m_sName;
		// type of shader data
		EShaderDataType m_eType;
		// amount of entries of type (float4 -> 4)
		unsigned int m_uiCount;
		// total size of the entry
		unsigned int m_uiSize;
		// offset from start of shaderdata to this entry
		unsigned int m_uiOffset;
		// is the entry pre defined
		bool m_bPredefined;
		// predefined type (if valid)
		EShaderDataPredefined m_ePredefinedType;
	};
	typedef std::vector<SShaderDataEntry*> TEntryVector;

	CShaderData(const std::string& sName,
		const TEntryVector& vEntries);
	virtual ~CShaderData();

	const std::string& GetName() const { return m_sName;}

	unsigned int GetEntryCount() const;
	const SShaderDataEntry* GetEntry(const std::string& sName) const;

	unsigned int GetTotalSize() const { return m_uiSize; }
	bool IsDirty() const { return m_bDirty; }
	void SetDirty() { m_bDirty = true; }
	void ResetDirty() { m_bDirty = false; }
 
	virtual bool SetFloatValue(const std::string& sName, float fValue) = 0;
	virtual bool SetFloatValues(const std::string& sName, float* pfValues, 
		unsigned int uiEntries) = 0;

	virtual bool SetIntValue(const std::string& sName, int iValue) = 0;
	virtual bool SetIntValues(const std::string& sName, int* piValues, 
		unsigned int uiEntries) = 0;

protected:

	void FillMap(const TEntryVector& vEntries);

	//////////////////////////////////////////////////////////////////////////

	std::string m_sName;

	typedef std::map<std::string, SShaderDataEntry*> TEntryMap;
	std::map<std::string, SShaderDataEntry*> m_mShaderEntries;

	unsigned int m_uiSize;
	bool m_bDirty;
};