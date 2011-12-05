#pragma once

#include <Material.h>

class CDX11ConstantBuffer;
class CDX11ConstantBufferData;
class CDX11MeshRenderData;
class CDX11Renderer;

class CMesh;

typedef stl_vector<CDX11ConstantBuffer*> TConstantBufferVector;

class CDX11Material : public CMaterial
{
public:
	
	CDX11Material(const stl_string& sMaterialName);
	virtual ~CDX11Material();

	virtual unsigned int GetPassCount() = 0;
	virtual void ApplyPass(unsigned int uiPassID, 
		ID3D11DeviceContext* pkDeviceContext) = 0;

	ID3D11InputLayout* GetInputLayout() { return m_pkInputLayout; }
	void SetInputLayout(ID3D11InputLayout* pkLayout) { m_pkInputLayout = pkLayout; }

	TConstantBufferVector& GetContantBufferList() { return m_vConstantBuffers; }
	void UpdateConstantBufferData(CDX11Renderer* pkRenderer, 
		CMesh* pkMesh);

	virtual void ApplyTextures(CDX11Renderer* pkRenderer, 
		CMesh* pkMesh) = 0;

	virtual const void* GetInputSignatureData(unsigned int& uiSize) = 0;

protected:

	//////////////////////////////////////////////////////////////////////////

	// vertex layout used for this material
	ID3D11InputLayout* m_pkInputLayout;

	// all the constant buffers which are used for this material	
	TConstantBufferVector m_vConstantBuffers;
};

CoreSmartPointer(CDX11Material);