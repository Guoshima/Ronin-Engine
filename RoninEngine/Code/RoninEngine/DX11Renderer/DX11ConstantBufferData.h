#pragma once

#include <CoreMemObject.h>

class CDX11ConstantBufferDesc;

class CDX11ConstantBufferData : public CCoreRefMTObject
{
public:

	CDX11ConstantBufferData(
		const CDX11ConstantBufferDesc* pkConstantBufferDesc);
	virtual ~CDX11ConstantBufferData();

	const CDX11ConstantBufferDesc* GetDesc() const {
		return m_pkDesc;
	}
	bool IsDirty() const { return m_bDirty; }

	char* GetData() { return m_acData; }
	void UpdateData(const stl_string& sName, void* acData);

protected:

	//////////////////////////////////////////////////////////////////////////

	// do we want to update the constant buffer data next time it's used
	bool m_bDirty;

	// void* used for fast copy to the data itself
	char* m_acData;

	// desc used to get info on the variables
	const CDX11ConstantBufferDesc* m_pkDesc;
};
CoreSmartPointer(CDX11ConstantBufferData);
