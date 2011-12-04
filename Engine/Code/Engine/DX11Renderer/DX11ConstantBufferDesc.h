#pragma once

#include <CoreMemObject.h>

class CDX11ConstantBufferDesc : public CCoreMemObject
{
public:

	enum EShaderDataPredefined
	{
		PREDEFINED_INVALID,
		PREDEFINED_PROJECTION,
		PREDEFINED_VIEW,
		PREDEFINED_WORLD,
		PREDEFINED_WORLDVIEW,
		PREDEFINED_VIEWPROJECTION,
		PREDEFINED_WORLDVIEWPROJECTION,
		MAX_PREDEFINED,
	};

	struct SVariableDesc : public CCoreMemObject
	{
		SVariableDesc() : m_sName(""), m_uiCount(0), m_uiSize(0),
			m_uiOffset(0), m_bPredefined(false), m_ePredefinedType(PREDEFINED_INVALID)
		{}

		// unique name of this variable
		stl_string m_sName;
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
	typedef stl_vector<SVariableDesc*> TVariableDescList;
	typedef stl_map<stl_string, SVariableDesc*> TVariableDescMap;

	CDX11ConstantBufferDesc();
	virtual ~CDX11ConstantBufferDesc();

	const stl_string& GetName() const { return m_sName; }
	void SetName(const stl_string& sName) { m_sName = sName; }

	bool AddVariable(SVariableDesc* pkVariableDesc);
	const SVariableDesc* GetVariableDesc(const stl_string& sName) const;

	TVariableDescList& GetVariableDescList() { return m_vVariables; }
	const TVariableDescList& GetVariableDescList() const { return m_vVariables; }

	bool IsPredefined() const { return m_bPredefined; }
	unsigned int GetTotalSize() const { return m_uiTotalSize; }

protected:

	//////////////////////////////////////////////////////////////////////////

	stl_string m_sName;

	bool m_bPredefined;
	unsigned int m_uiTotalSize;
	
	TVariableDescList m_vVariables;
	TVariableDescMap m_mVariables;
};
