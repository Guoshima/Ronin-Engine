#pragma once

#include <MeshDataEntry.h>

class CDX11MeshDataEntry : public CMeshDataEntry
{
	// only creator can create an instance of this class
	friend class CDX11MeshDataEntryCreator;

public:

	virtual ~CDX11MeshDataEntry();

	virtual void* LockData();
	virtual void UnLockData();

	ID3D11Buffer* GetD3D11Buffer() { return m_pkD3D11Buffer; }

	bool CreateD3D11Buffer(ID3D11Device* pkDevice);
	
	D3D11_INPUT_ELEMENT_DESC* GetElementDesc(unsigned int& uiElementCount);

protected:

	CDX11MeshDataEntry(const stl_vector<CMeshDataEntryDescriptorPtr>& vDescriptors,
		unsigned short uiCount, CMeshDataEntry::EMeshDataInputType eInputType);

	void AllocateData();
	void ExtractD3DInfo();
	
	//////////////////////////////////////////////////////////////////////////

	ID3D11Buffer* m_pkD3D11Buffer;
	void* m_pkData;

	D3D11_INPUT_ELEMENT_DESC* m_pkElementDesc;

	UINT m_uiD3D11BindFlags;
	D3D11_USAGE m_eD3D11Usage;
};