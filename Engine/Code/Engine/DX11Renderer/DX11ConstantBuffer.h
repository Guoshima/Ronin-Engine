#pragma once

#include <CoreMemObject.h>

class CDX11ConstantBufferDesc;
class CMesh;
class CDX11Renderer;

class CDX11ConstantBuffer : public CCoreRefSTObject
{
public:

	CDX11ConstantBuffer(ID3D11Buffer* d3dContantBuffer,
		CDX11ConstantBufferDesc* pkDesc);
	virtual ~CDX11ConstantBuffer();

	const CDX11ConstantBufferDesc* GetDesc() const { return m_pkDesc; }

	ID3D11Buffer* GetD3DData() { return m_d3dContantBuffer; }

	void UpdateD3DData(CDX11Renderer* pkRenderer, CMesh* pkMesh);

protected:

	void UpdatePredefinedData(
		CDX11Renderer* pkRenderer, CMesh* pkMesh);

	//////////////////////////////////////////////////////////////////////////

	// the actual D3D data
	ID3D11Buffer* m_d3dContantBuffer;

	// data used only when constant buffer is predefined cosntant buffer
	// because no constant buffer data coutnerpart excist then
	char* m_acData;
	unsigned int m_uiSize;

	// desc used to get info on the variables
	CDX11ConstantBufferDesc* m_pkDesc;
};
CoreSmartPointer(CDX11ConstantBuffer);