#pragma once

#include "MeshRenderData.h"

#include "DX11ConstantBufferData.h"

class CMesh;
class CDX11ConstantBufferDesc;

class CDX11MeshRenderData : public CMeshRenderData
{
public:

	CDX11MeshRenderData();
	~CDX11MeshRenderData();

	// index buffer related
	void SetIndexBufferData(ID3D11Buffer* pkIndexBuffer, DXGI_FORMAT eFormat,
		unsigned int uiIndexBufferOffset);
	void SetIndexBuffer(ID3D11DeviceContext* pkContext);
	void ReleaseIndexBufferData();

	// vertex buffer related
	void SetVertexBufferData(unsigned int uiVertexBuffercount, 
		ID3D11Buffer** ppkVertexBuffers, unsigned int* puiVertexStrides,
		unsigned int* uiVertexOffsets);
	void SetVertexBuffer(ID3D11DeviceContext* pkContext);
	void ReleaseVertexBufferData();

	void SetIndexCount(unsigned int uiIndexCount) { m_uiIndexCount = uiIndexCount; }
	unsigned int GetIndexCount() { return m_uiIndexCount; }

	void SetVertexCount(unsigned int uiVertexCount) { m_uiVertexCount = uiVertexCount; }
	unsigned int GetVertexCount() { return m_uiVertexCount; }

	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY eTopology) { m_ePrimitiveTopology = eTopology; }
	D3D11_PRIMITIVE_TOPOLOGY GetTopology() { return m_ePrimitiveTopology; }

	stl_vector<CDX11ConstantBufferDataPtr>& GetConstantBufferDataList();
	CDX11ConstantBufferData* GetConstantBufferData(
		const CDX11ConstantBufferDesc* pkCBDesc);
	bool AddConstantBufferData(CDX11ConstantBufferData* pkData, CMesh* pkMesh);

private:
	
	//////////////////////////////////////////////////////////////////////////

	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveTopology;

	unsigned int m_uiVertexBufferCount;
	ID3D11Buffer** m_ppkVertexBuffers;
	unsigned int* m_puiVertexStrides;
	unsigned int* m_puiVertexOffsets;

	ID3D11Buffer* m_pkIndexBuffer;
	DXGI_FORMAT m_eIBFormat;
	unsigned int m_uiIndexBufferOffset;

	stl_vector<CDX11ConstantBufferDataPtr> m_vContantBufferData;

	unsigned int m_uiIndexCount;
	unsigned int m_uiVertexCount;
};