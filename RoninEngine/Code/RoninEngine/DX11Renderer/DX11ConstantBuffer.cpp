#include "DX11RendererPCH.h"

#include "DX11ConstantBuffer.h"

#include "DX11Renderer.h"
#include "DX11MeshRenderData.h"
#include "DX11ConstantBufferData.h"
#include "DX11ConstantBufferDesc.h"

#include <Mesh.h>
#include <MeshData.h>
#include <MeshRenderData.h>
#include <Material.h>

//////////////////////////////////////////////////////////////////////////

CDX11ConstantBuffer::CDX11ConstantBuffer(ID3D11Buffer* d3dContantBuffer,
	 CDX11ConstantBufferDesc* pkDesc) :
m_d3dContantBuffer(d3dContantBuffer), m_pkDesc(pkDesc), 
m_acData(0), m_uiSize(0)
{
	if (m_pkDesc && m_pkDesc->IsPredefined())
	{
		m_acData = (char*) IntMalloc(m_pkDesc->GetTotalSize());
		m_uiSize = m_pkDesc->GetTotalSize();
	}
}

CDX11ConstantBuffer::~CDX11ConstantBuffer()
{
	IntDelete m_pkDesc;
	IntFree(m_acData);

	SAFE_RELEASE(m_d3dContantBuffer);
}

//////////////////////////////////////////////////////////////////////////

void CDX11ConstantBuffer::UpdateD3DData(
	CDX11Renderer* pkRenderer, CMesh* pkMesh)
{
	ID3D11DeviceContext* d3dDeviceContext = 
		pkRenderer->GetImmediateDeviceContext();

	// get the buffer to update the data
	char* acData = 0;
	unsigned int uiSize = 0;

	if (m_pkDesc->IsPredefined())
	{
		UpdatePredefinedData(pkRenderer, pkMesh);

		acData = m_acData;
		uiSize = m_uiSize;
	} else {

		// should find constant buffer data somewhere in render data
		CDX11MeshRenderData* pkRenderData = (CDX11MeshRenderData*)
			pkMesh->GetRenderData();
		CDX11ConstantBufferData* pkCBData = 
			pkRenderData->GetConstantBufferData(m_pkDesc);
		if (pkCBData)
		{
			acData = pkCBData->GetData();
			uiSize = m_pkDesc->GetTotalSize();
		}
	}

	if (acData && uiSize)
	{
		d3dDeviceContext->UpdateSubresource(m_d3dContantBuffer, 0, NULL, 
			acData, uiSize, uiSize);
	}
}

//////////////////////////////////////////////////////////////////////////

void CDX11ConstantBuffer::UpdatePredefinedData(
	CDX11Renderer* pkRenderer, CMesh* pkMesh)
{
	unsigned int uiCurrentOffset = 0;

	char* acLocalData = (char*) m_acData;

	const CDX11ConstantBufferDesc::TVariableDescList& vVariableList =
		m_pkDesc->GetVariableDescList();
	for (unsigned short us = 0; us < vVariableList.size(); ++us)
	{
		const CDX11ConstantBufferDesc::SVariableDesc* pkVariable = 
			vVariableList[us];

		switch (pkVariable->m_ePredefinedType)
		{
		case CDX11ConstantBufferDesc::PREDEFINED_WORLD:
			{
				const CTransform& kTransform = pkMesh->GetWorldTransform();
				D3DXMATRIXA16 kWorldTranspose;
				D3DXMatrixTranspose(&kWorldTranspose, &kTransform.GetData());
				//D3DXMatrixIdentity(&kWorldTranspose);
				memcpy(acLocalData, &kWorldTranspose, sizeof(D3DXMATRIXA16));
				break;
			}
		case CDX11ConstantBufferDesc::PREDEFINED_VIEW:
			{
				const D3DXMATRIXA16& kView = pkRenderer->GetCurrentViewMatrix();
				D3DXMATRIXA16 kViewTranspose;
				D3DXMatrixTranspose(&kViewTranspose, &kView);
				memcpy(acLocalData, &kViewTranspose, sizeof(D3DXMATRIXA16));
				break;
			}
		case CDX11ConstantBufferDesc::PREDEFINED_PROJECTION:
			{
				const D3DXMATRIXA16& kProjection = pkRenderer->GetCurrentProjectionMatrix();
				D3DXMATRIXA16 kProjectionTranspose;
				D3DXMatrixTranspose(&kProjectionTranspose, &kProjection);
				memcpy(acLocalData, &kProjectionTranspose, sizeof(D3DXMATRIXA16));
				break;
			}
		case CDX11ConstantBufferDesc::PREDEFINED_VIEWPROJECTION:
			{
				const D3DXMATRIXA16& kView = pkRenderer->GetCurrentViewMatrix();
				const D3DXMATRIXA16& kProjection = pkRenderer->GetCurrentProjectionMatrix();

				D3DXMATRIXA16 kViewProj = kView * kProjection;

				D3DXMATRIXA16 kViewProjTranspose;
				D3DXMatrixTranspose(&kViewProjTranspose, &kViewProj);

				memcpy(acLocalData, &kViewProjTranspose, sizeof(D3DXMATRIXA16));
				break;
			}
		case CDX11ConstantBufferDesc::PREDEFINED_WORLDVIEW:
			{
				const CTransform& kTransform = pkMesh->GetWorldTransform();

				const D3DXMATRIXA16& kView = pkRenderer->GetCurrentViewMatrix();

				D3DXMATRIXA16 kWorldView = kTransform.GetData() * kView;

				D3DXMATRIXA16 kWorldViewTranspose;
				D3DXMatrixTranspose(&kWorldViewTranspose, &kWorldView);

				memcpy(acLocalData, &kWorldViewTranspose, sizeof(D3DXMATRIXA16));
				break;
			}
		case CDX11ConstantBufferDesc::PREDEFINED_WORLDVIEWPROJECTION:
			{
				const CTransform& kTransform = pkMesh->GetWorldTransform();
				
				const D3DXMATRIXA16& kView = pkRenderer->GetCurrentViewMatrix();
				const D3DXMATRIXA16& kProjection = pkRenderer->GetCurrentProjectionMatrix();

				D3DXMATRIXA16 kWorldViewProj = kTransform.GetData() * kView * kProjection;
				
				D3DXMATRIXA16 kWorldViewProjTranspose;
				D3DXMatrixTranspose(&kWorldViewProjTranspose, &kWorldViewProj);

				memcpy(acLocalData, &kWorldViewProjTranspose, sizeof(D3DXMATRIXA16));
				break;
			}
		}

		acLocalData += pkVariable->m_uiSize;
	}
}

//////////////////////////////////////////////////////////////////////////