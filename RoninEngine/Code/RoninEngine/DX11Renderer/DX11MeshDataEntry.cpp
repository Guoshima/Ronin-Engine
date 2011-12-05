#include "DX11RendererPCH.h"

#include "DX11MeshDataEntry.h"

#include "DX11EnumConverters.h"

//////////////////////////////////////////////////////////////////////////

CDX11MeshDataEntry::CDX11MeshDataEntry(
	const stl_vector<CMeshDataEntryDescriptorPtr>& vDescriptors, 
	unsigned short uiCount, CMeshDataEntry::EMeshDataInputType eInputType) :
CMeshDataEntry(vDescriptors, uiCount, eInputType), m_pkD3D11Buffer(0),
m_pkData(0), m_eD3D11Usage(D3D11_USAGE_DEFAULT), m_uiD3D11BindFlags(0),
m_pkElementDesc(0)
{
	ExtractD3DInfo();
	AllocateData();
}

CDX11MeshDataEntry::~CDX11MeshDataEntry()
{
	IntFree(m_pkElementDesc);
	IntFree(m_pkData);
	SAFE_RELEASE(m_pkD3D11Buffer);
}

//////////////////////////////////////////////////////////////////////////

void* CDX11MeshDataEntry::LockData()
{
	// for now simply return the temp data .. no dynamic update allowed
	return m_pkData;
}

void CDX11MeshDataEntry::UnLockData()
{
	// nothing special so far
}

//////////////////////////////////////////////////////////////////////////

void CDX11MeshDataEntry::AllocateData()
{
	if (m_pkData)
		return;

	m_pkData = IntMalloc(GetSize());
}

//////////////////////////////////////////////////////////////////////////

bool CDX11MeshDataEntry::CreateD3D11Buffer(ID3D11Device* pkDevice)
{
	if (m_pkD3D11Buffer)
		return true;

	// make sure we have valid data!
	if (m_pkData == 0)
		return false;

	D3D11_BUFFER_DESC kBufferDesc;
	kBufferDesc.Usage = m_eD3D11Usage;
	kBufferDesc.ByteWidth = GetSize();
	kBufferDesc.BindFlags = m_uiD3D11BindFlags;
	kBufferDesc.CPUAccessFlags = 0;
	kBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA kInitData;
	kInitData.pSysMem = m_pkData;

	HRESULT hr = pkDevice->CreateBuffer( &kBufferDesc, 
		&kInitData, &m_pkD3D11Buffer );
	if( FAILED( hr ) )
		return false;

	IntFree(m_pkData);
	m_pkData = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////////

D3D11_INPUT_ELEMENT_DESC* CDX11MeshDataEntry::GetElementDesc(
	unsigned int& uiElementCount)
{
	uiElementCount = GetDescriptorCount();
	if (uiElementCount == 0)
		return 0;

	if (m_pkElementDesc)
	{
		return m_pkElementDesc;
	}

	unsigned int uiCurrentOffset = 0;

	m_pkElementDesc = (D3D11_INPUT_ELEMENT_DESC*) IntAlloc(D3D11_INPUT_ELEMENT_DESC, uiElementCount);
	for (unsigned int ui = 0; ui < uiElementCount; ++ui)
	{
		const CMeshDataEntryDescriptor* pkDescriptor = GetDescriptorAt(ui);

		D3D11_INPUT_ELEMENT_DESC& kDesc = m_pkElementDesc[ui];
		kDesc.SemanticName = CDX11EnumConverters::GetSemanticName(
			pkDescriptor->GetSemantic());
		kDesc.SemanticIndex = pkDescriptor->GetSemanticIndex();
		kDesc.Format = CDX11EnumConverters::GetFormat(
			pkDescriptor->GetFormat());
		kDesc.InputSlot = 0;
		kDesc.AlignedByteOffset = uiCurrentOffset;
		kDesc.InputSlotClass = CDX11EnumConverters::GetInputClassification(
			GetInputType());
		kDesc.InstanceDataStepRate = 0;

		uiCurrentOffset += CMeshDataEntryDescriptor::GetSizeOfFormat(
			pkDescriptor->GetFormat());
	}

	return m_pkElementDesc;
}

//////////////////////////////////////////////////////////////////////////

void CDX11MeshDataEntry::ExtractD3DInfo()
{
	switch (GetInputType())
	{
	case TYPE_VERTEX_DATA:
		{
			m_uiD3D11BindFlags = D3D11_BIND_VERTEX_BUFFER;
			break;
		}
	case TYPE_INDEX_DATA:
		{
			m_uiD3D11BindFlags = D3D11_BIND_INDEX_BUFFER;
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////