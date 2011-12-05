#include "DX11RendererPCH.h"

#include "DX11MeshRenderData.h"

#include "DX11ConstantBufferData.h"
#include "DX11ConstantBufferDesc.h"
#include "DX11ShaderData.h"

#include <Mesh.h>
#include <MeshData.h>
#include <MeshRenderData.h>
#include <Material.h>

//////////////////////////////////////////////////////////////////////////

CDX11MeshRenderData::CDX11MeshRenderData() : 
m_ppkVertexBuffers(0), m_ePrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED),
m_uiVertexBufferCount(0), m_puiVertexStrides(0), m_puiVertexOffsets(0),
m_pkIndexBuffer(0), m_eIBFormat(DXGI_FORMAT_UNKNOWN), 
m_uiIndexBufferOffset(0), m_uiIndexCount(0), m_uiVertexCount(0)
{
}

CDX11MeshRenderData::~CDX11MeshRenderData()
{
	ReleaseIndexBufferData();
	ReleaseVertexBufferData();
}

//////////////////////////////////////////////////////////////////////////

void CDX11MeshRenderData::SetIndexBufferData(
	ID3D11Buffer* pkIndexBuffer, DXGI_FORMAT eFormat,
	unsigned int uiIndexBufferOffset)
{
	ReleaseIndexBufferData();

	m_pkIndexBuffer = pkIndexBuffer;
	m_eIBFormat = eFormat;
	m_uiIndexBufferOffset = uiIndexBufferOffset;
}

void CDX11MeshRenderData::SetIndexBuffer(ID3D11DeviceContext* pkContext)
{
	pkContext->IASetIndexBuffer(m_pkIndexBuffer, 
		m_eIBFormat, m_uiIndexBufferOffset);
}

void CDX11MeshRenderData::ReleaseIndexBufferData()
{
	SAFE_RELEASE(m_pkIndexBuffer);
}

//////////////////////////////////////////////////////////////////////////

void CDX11MeshRenderData::SetVertexBufferData(unsigned int uiVertexBuffercount, 
	ID3D11Buffer** ppkVertexBuffers, unsigned int* puiVertexStrides,
	unsigned int* puiVertexOffsets)
{
	ReleaseVertexBufferData();

	m_uiVertexBufferCount = uiVertexBuffercount;
	m_ppkVertexBuffers = ppkVertexBuffers;
	m_puiVertexStrides = puiVertexStrides;
	m_puiVertexOffsets= puiVertexOffsets;
}

void CDX11MeshRenderData::SetVertexBuffer(ID3D11DeviceContext* pkContext)
{
	pkContext->IASetVertexBuffers(0, m_uiVertexBufferCount,
		m_ppkVertexBuffers, m_puiVertexStrides, m_puiVertexOffsets);
}

void CDX11MeshRenderData::ReleaseVertexBufferData()
{
	for (unsigned int ui = 0; ui < m_uiVertexBufferCount; ++ui)
	{
		SAFE_RELEASE(m_ppkVertexBuffers[ui]);
	}

	if (m_ppkVertexBuffers)
		IntFree(m_ppkVertexBuffers);
	m_ppkVertexBuffers = 0;

	if (m_puiVertexStrides)
		IntFree(m_puiVertexStrides);
	m_puiVertexStrides = 0;

	if (m_puiVertexOffsets)
		IntFree(m_puiVertexOffsets);
	m_puiVertexOffsets = 0;
}

//////////////////////////////////////////////////////////////////////////

stl_vector<CDX11ConstantBufferDataPtr>& 
	CDX11MeshRenderData::GetConstantBufferDataList()
{
	return m_vContantBufferData;
}

CDX11ConstantBufferData* CDX11MeshRenderData::GetConstantBufferData(
	const CDX11ConstantBufferDesc* pkCBDesc)
{
	for (unsigned short us = 0; us < m_vContantBufferData.size(); ++us)
	{
		CDX11ConstantBufferData* pkCBData = m_vContantBufferData[us];
		if (pkCBData->GetDesc() == pkCBDesc)
			return pkCBData;
	}

	return 0;
}

bool CDX11MeshRenderData::AddConstantBufferData(
	CDX11ConstantBufferData* pkData, CMesh* pkMesh)
{
	const CDX11ConstantBufferDesc* pkCBDesc = pkData->GetDesc();

	// make sure it's unique for this descriptor
	if (GetConstantBufferData(pkCBDesc))
		return false;

	m_vContantBufferData.push_back(pkData);

	// if constant buffer is predefined, then we are done, else setup the mapping
	if (pkCBDesc->IsPredefined())
		return true;

	// add the constant buffer data to the correct shader data
	stl_vector<CShaderDataPtr>& vShaderDataList = pkMesh->GetShaderDataList();
	for (unsigned short us = 0; us < vShaderDataList.size(); ++us)
	{
		CDX11ShaderData* pkShaderData = SmartPointerCast(CDX11ShaderData,vShaderDataList[us]);
		const CDX11ConstantBufferDesc::SVariableDesc* pkVariableDesc = 
			pkCBDesc->GetVariableDesc(pkShaderData->GetName());
		if (pkVariableDesc && pkVariableDesc->m_uiSize == pkShaderData->GetSize())
		{
			pkShaderData->AddConstantBufferData(pkData);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////