#include "DX11RendererPCH.h"

#include "DX11HelperFunctions.h"

#include "DX11Renderer.h"
#include "DX11Material.h"
#include "DX11MeshDataEntry.h"
#include "DX11MeshRenderData.h"
#include "DX11EnumConverters.h"
#include "DX11ConstantBuffer.h"
#include "DX11ConstantBufferData.h"
#include "DX11ConstantBufferDesc.h"
#include "DX11RenderTargetTextureData2D.h"

#include <Mesh.h>
#include <MeshData.h>
#include <MeshDataEntryDescriptor.h>

#include <Camera.h>

#include <Texture.h>

#include <RenderTarget.h>

//////////////////////////////////////////////////////////////////////////

CDX11Renderer* CDX11HelperFunctions::ms_pkRenderer;
ID3D11RenderTargetView* CDX11HelperFunctions::ms_ppkActiveRenderTargetViews[
	D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
CDX11HelperFunctions::SActiveData CDX11HelperFunctions::ms_kActiveData;

//////////////////////////////////////////////////////////////////////////

bool CDX11HelperFunctions::Init(CDX11Renderer* pkRenderer)
{
	ms_pkRenderer = pkRenderer;

	ms_kActiveData.m_kCurrentProjectionMatrix.SetUnitTransform();
	ms_kActiveData.m_kCurrentViewMatrix.SetUnitTransform();
	ms_kActiveData.ms_pkCurrentMesh = 0;
	ms_kActiveData.ms_pkActiveRenderTarget = 0;

	memset(ms_ppkActiveRenderTargetViews, 0, 
		D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT * 
		sizeof(ID3D11RenderTargetView*));

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CDX11HelperFunctions::SetCameraData(const CCamera* pkCamera)
{
	if (pkCamera == 0)
		return false;

	CRenderTarget* pkRenderTarget = ms_kActiveData.ms_pkActiveRenderTarget;
	if (pkRenderTarget == 0)
		pkRenderTarget = ms_pkRenderer->GetDefaultRenderTarget();

	float fWidth = (float) pkRenderTarget->GetWidth();
	float fHeight = (float) pkRenderTarget->GetHeight();

	const CCamera::SViewPort& kViewPort = pkCamera->GetViewPort();

	ID3D11DeviceContext* pkDeviceContext = 
		ms_pkRenderer->GetImmediateDeviceContext();	

	// Setup the viewport from our own viewport
	D3D11_VIEWPORT kD3D11ViewPort;
	kD3D11ViewPort.TopLeftX = (FLOAT)(kViewPort.m_fLeft * fWidth);
	kD3D11ViewPort.TopLeftY = (FLOAT)((1.0f - kViewPort.m_fTop) * fHeight);
	kD3D11ViewPort.Width = 
		(FLOAT)((kViewPort.m_fRight - kViewPort.m_fLeft) * fWidth);
	kD3D11ViewPort.Height = 
		(FLOAT)((kViewPort.m_fTop - kViewPort.m_fBottom) * fHeight);
	kD3D11ViewPort.MinDepth = 0.0f;     
	kD3D11ViewPort.MaxDepth = 1.0f;
	pkDeviceContext->RSSetViewports(1, &kD3D11ViewPort);

	// store the view and projection matrix to set the predefined values later
	D3DXMATRIXA16 kView;
	D3DXVECTOR3 Eye( 0.0f, 3.0f, -6.0f );
	D3DXVECTOR3 At( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 Up( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &kView, &Eye, &At, &Up );

	D3DXMATRIXA16 kViewInv;
	D3DXMatrixInverse( &kViewInv, 0, &kView);

	pkCamera->GetViewMatrix(ms_kActiveData.m_kCurrentViewMatrix);

	//D3DXMatrixTranspose(&m_kCurrentViewMatrix, &m_kCurrentViewMatrix);

	D3DXMatrixPerspectiveFovLH( &ms_kActiveData.m_kCurrentProjectionMatrix.GetData(), 
		pkCamera->GetFov(), fWidth / fHeight, pkCamera->GetNearDistance(), 
		pkCamera->GetFarDistance() );

	//D3DXMatrixTranspose(&m_kCurrentProjectionMatrix, &m_kCurrentProjectionMatrix);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CDX11HelperFunctions::BindRenderTarget(
	CRenderTarget* pkRenderTarget, CRenderTarget::EClearFlags eClearFlags)
{
	if (pkRenderTarget == 0)
		return false;

	if (!ValidateRenderTarget(pkRenderTarget))
		return false;

	ID3D11DeviceContext* pkDeviceContext = 
		ms_pkRenderer->GetImmediateDeviceContext();	

	unsigned int uiNumOfTargets = pkRenderTarget->GetNumOfTargets();
	ID3D11DepthStencilView* pkDepthStencilView = 0;

	if (uiNumOfTargets > 0)
	{
		// set the render target views
		for (unsigned short us = 0; us < 
			pkRenderTarget->GetNumOfTargets(); ++us)
		{
			CTexture* pkTexture = pkRenderTarget->GetRenderTarget(us);
			if (!pkTexture->IsRenderTarget())
				continue;

			CDX11RenderTargetTextureData2D* pkTextureData =
				(CDX11RenderTargetTextureData2D*) pkTexture->GetTextureData();
			if (!pkTextureData)
				continue;

			ms_ppkActiveRenderTargetViews[us] = 
				pkTextureData->GetRenderTargetView();
		}
	}

	if (pkRenderTarget->GetDepthStencil())
	{
		CTexture* pkTexture = pkRenderTarget->GetDepthStencil();
		if (pkTexture->IsRenderTarget())
		{
			CDX11RenderTargetTextureData2D* pkTextureData =
				(CDX11RenderTargetTextureData2D*) pkTexture->GetTextureData();
			if (pkTextureData)
			{
				pkDepthStencilView = 
					pkTextureData->GetDepthStencilView();
			}
		}
	}

	pkDeviceContext->OMSetRenderTargets(uiNumOfTargets, 
		ms_ppkActiveRenderTargetViews, pkDepthStencilView);

	ms_kActiveData.ms_pkActiveRenderTarget = pkRenderTarget;

	if (eClearFlags != CRenderTarget::CLEAR_NONE)
	{
		return ClearRenderTarget(pkRenderTarget, eClearFlags);
	}

	return true;
}

bool CDX11HelperFunctions::UnbindRenderTarget()
{
	// end using all the shader resources
	ID3D11DeviceContext* pkDeviceContext = 
		ms_pkRenderer->GetImmediateDeviceContext();	

	ID3D10ShaderResourceView* apkShaderResourceViews
		[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
	memset(apkShaderResourceViews, 0 , sizeof(ID3D10ShaderResourceView*) 
		* D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
 
	pkDeviceContext->PSSetShaderResources(0, 
		D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, 
		(ID3D11ShaderResourceView *const *)apkShaderResourceViews);

	// resolve data


	ms_kActiveData.ms_pkActiveRenderTarget = 0;

	return true;
}


bool CDX11HelperFunctions::ClearRenderTarget(
	CRenderTarget* pkRenderTarget, CRenderTarget::EClearFlags eClearFlags)
{
	if (pkRenderTarget == 0 || eClearFlags == CRenderTarget::CLEAR_NONE)
		return false;

	if (!ValidateRenderTarget(pkRenderTarget))
		return false;

	ID3D11DeviceContext* pkDeviceContext = 
		ms_pkRenderer->GetImmediateDeviceContext();	

	unsigned int uiNumOfTargets = pkRenderTarget->GetNumOfTargets();

	if (uiNumOfTargets > 0 && (eClearFlags & CRenderTarget::CLEAR_BACKBUFFER) )
	{
		FLOAT afClearColor[4];

		// set the render target views
		for (unsigned short us = 0; us < 
			pkRenderTarget->GetNumOfTargets(); ++us)
		{
			CTexture* pkTexture = pkRenderTarget->GetRenderTarget(us);
			if (!pkTexture->IsRenderTarget())
				continue;

			CDX11RenderTargetTextureData2D* pkTextureData =
				(CDX11RenderTargetTextureData2D*) pkTexture->GetTextureData();
			if (!pkTextureData)
				continue;

			ID3D11RenderTargetView* d3dRenderTargetView = 
				pkTextureData->GetRenderTargetView();
			if (d3dRenderTargetView)
			{
				D3DXVECTOR4 kClearColor = pkRenderTarget->GetClearColor(us);
				afClearColor[0] = kClearColor.x;
				afClearColor[1] = kClearColor.y;
				afClearColor[2] = kClearColor.z;
				afClearColor[3] = kClearColor.w;
				pkDeviceContext->ClearRenderTargetView(d3dRenderTargetView, afClearColor);
			}
		}
	}

	if (pkRenderTarget->GetDepthStencil() && 
		(eClearFlags & CRenderTarget::CLEAR_DEPTHBUFFER ||
		 eClearFlags & CRenderTarget::CLEAR_STENCILBUFFER))
	{
		CTexture* pkTexture = pkRenderTarget->GetDepthStencil();
		if (pkTexture->IsRenderTarget())
		{
			CDX11RenderTargetTextureData2D* pkTextureData =
				(CDX11RenderTargetTextureData2D*) pkTexture->GetTextureData();
			if (pkTextureData)
			{
				ID3D11DepthStencilView* pkDepthStencilView = 
					pkTextureData->GetDepthStencilView();
				if (pkDepthStencilView)
				{
					float fDepthClear = pkRenderTarget->GetDepthClear();
					int iStencilClear = pkRenderTarget->GetStencilClear();
					UINT uiClearFlags = 0;
					if (eClearFlags & CRenderTarget::CLEAR_DEPTHBUFFER)
						uiClearFlags |= D3D11_CLEAR_DEPTH;
					if (eClearFlags & CRenderTarget::CLEAR_STENCILBUFFER)
						uiClearFlags |= D3D11_CLEAR_STENCIL;
					pkDeviceContext->ClearDepthStencilView(pkDepthStencilView, 
						uiClearFlags, fDepthClear, iStencilClear);
				}
			}
		}		
	}

	return true;
}

bool CDX11HelperFunctions::ValidateRenderTarget(
	CRenderTarget* pkRenderTarget)
{
	// nothing special for now
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CDX11HelperFunctions::RenderMesh(CMesh* pkMesh)
{
	if (pkMesh == 0)
		return false;

	ms_kActiveData.ms_pkCurrentMesh = pkMesh;

	if (!ValidateMeshRenderData(pkMesh))
	{
		ms_kActiveData.ms_pkCurrentMesh = 0;
		return false;
	}

	if (!ValidateMeshMaterialData(pkMesh))
	{
		ms_kActiveData.ms_pkCurrentMesh = 0;
		return false;
	}

	CDX11MeshRenderData* pkRenderData = (CDX11MeshRenderData*) 
		pkMesh->GetRenderData();
	CDX11Material* pkActiveMaterial = (CDX11Material*)pkMesh->GetActiveMaterial();

	if (pkRenderData == 0 || pkActiveMaterial == 0)
	{
		ms_kActiveData.ms_pkCurrentMesh = 0;
		return false;
	}

	ID3D11DeviceContext* pkDeviceContext = ms_pkRenderer->GetImmediateDeviceContext();	
	if (pkDeviceContext == 0)
	{
		ms_kActiveData.ms_pkCurrentMesh = 0;
		return false;
	}

	// make sure we have a valid topology!
	if (pkRenderData->GetTopology() == D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
		return false;

	// set the default data
	pkDeviceContext->IASetPrimitiveTopology(pkRenderData->GetTopology());
	pkDeviceContext->IASetInputLayout(pkActiveMaterial->GetInputLayout());

	pkRenderData->SetVertexBuffer(pkDeviceContext);
	pkRenderData->SetIndexBuffer(pkDeviceContext);

	// make sure all constant buffers have been setup correctly for this mesh
	pkActiveMaterial->UpdateConstantBufferData(ms_pkRenderer, pkMesh);

	// set the correct textures
	pkActiveMaterial->ApplyTextures(ms_pkRenderer, pkMesh);

	for( unsigned short us = 0; us < pkActiveMaterial->GetPassCount(); ++us )
	{
		pkActiveMaterial->ApplyPass(us, pkDeviceContext);
		//m_pkActiveTechnique->GetPass(us)->Apply(0, pkDeviceContext);

		if (pkRenderData->GetIndexCount() > 0)
			pkDeviceContext->DrawIndexed( pkRenderData->GetIndexCount(), 0, 0 );
		else
			pkDeviceContext->Draw( pkRenderData->GetVertexCount(), 0 );
	}

	ms_kActiveData.ms_pkCurrentMesh = 0;

	return true;
}	

//////////////////////////////////////////////////////////////////////////

bool CDX11HelperFunctions::ValidateMeshRenderData(CMesh* pkMesh)
{
	CDX11MeshRenderData* pkRenderData = (CDX11MeshRenderData*) 
		pkMesh->GetRenderData();

	// data still valid ?
	if (pkRenderData && !pkMesh->IsRenderDataDirty())
		return true;

	if (pkRenderData == 0) {
		pkRenderData = IntNew CDX11MeshRenderData;
		pkMesh->SetRenderData(pkRenderData);
	}

	ID3D11Device* pkDevice = ms_pkRenderer->GetDevice();

	CMeshData* pkMeshData = pkMesh->GetMeshData();
	if (pkMeshData == 0)
		return false;

	// set the vertex buffer data
	CDX11MeshDataEntry* pkVertexBuffer = (CDX11MeshDataEntry*) 
		pkMeshData->GetVertexBuffer();
	if (pkVertexBuffer == 0)
		return false;

	// make sure we have valid D3D data
	pkVertexBuffer->CreateD3D11Buffer(pkDevice);

	ID3D11Buffer** ppkVertexBuffers = (ID3D11Buffer**) IntAlloc(ID3D11Buffer*,1);
	ppkVertexBuffers[0] = pkVertexBuffer->GetD3D11Buffer();
	unsigned int* puiStrides = (unsigned int*) IntAlloc(unsigned int, 1);
	puiStrides[0] = pkVertexBuffer->GetStride();
	unsigned int* puiOffsets = (unsigned int*) IntAlloc(unsigned int, 1);
	puiOffsets[0] = 0;
	pkRenderData->SetVertexBufferData(1, ppkVertexBuffers,
		puiStrides, puiOffsets);

	// set the index buffer data
	CDX11MeshDataEntry* pkIndexBuffer = (CDX11MeshDataEntry*) 
		pkMeshData->GetIndexBuffer();
	if (pkIndexBuffer)
	{
		// make sure we have valid D3D data
		pkIndexBuffer->CreateD3D11Buffer(pkDevice);

		DXGI_FORMAT kFormat = CDX11EnumConverters::GetFormat(
			pkIndexBuffer->GetDescriptorAt(0)->GetFormat());
		pkRenderData->SetIndexBufferData(pkIndexBuffer->GetD3D11Buffer(),
			kFormat, 0);
		pkRenderData->SetIndexCount(pkIndexBuffer->GetCount());
	} else 
	{
		pkRenderData->SetVertexCount(pkVertexBuffer->GetCount());
	}

	D3D11_PRIMITIVE_TOPOLOGY eTopology = CDX11EnumConverters::GetPrimitiveToplogy(
		pkMeshData->GetTopology());
	pkRenderData->SetTopology(eTopology);

	pkMesh->ResetRenderDataDirty();

	return true;
}

bool CDX11HelperFunctions::ValidateMeshMaterialData(CMesh* pkMesh)
{
	if (!pkMesh->IsMaterialListDirty())
		return true;

	CMeshData* pkMeshData = pkMesh->GetMeshData();
	if (pkMeshData == 0)
		return false;

	CDX11MeshRenderData* pkRenderData = (CDX11MeshRenderData*) 
		pkMesh->GetRenderData();

	CDX11MeshDataEntry* pkVertexBuffer = (CDX11MeshDataEntry*) 
		pkMeshData->GetVertexBuffer();
	if (pkVertexBuffer == 0)
		return false;

	ID3D11Device* pkDevice = ms_pkRenderer->GetDevice();

	unsigned int uiElementCount;
	D3D11_INPUT_ELEMENT_DESC* kElementDesc = 
		pkVertexBuffer->GetElementDesc(uiElementCount);

	stl_vector<CMaterialPtr>& vMaterials = pkMesh->GetMaterialList();
	for (unsigned short us = 0; us < vMaterials.size(); ++us)
	{
		CDX11Material* pkMaterial = SmartPointerCast(CDX11Material, vMaterials[us]);

		if (pkMaterial == 0) continue;

		// make sure the material has an input layout
		if (pkMaterial->GetInputLayout() == 0)
		{
			// set the input layout			
			unsigned int uiInputSignatureDataSize = 0;
			const void* pkInputSignatureData = 
				pkMaterial->GetInputSignatureData(uiInputSignatureDataSize);
			if (pkInputSignatureData == 0)
				return false;

			ID3D11InputLayout* pkInputLayout;
			pkDevice->CreateInputLayout(kElementDesc, uiElementCount,
				pkInputSignatureData, uiInputSignatureDataSize, &pkInputLayout);

			pkMaterial->SetInputLayout(pkInputLayout);
		}

		// do we already have all the constant buffer data mapped
		TConstantBufferVector& vConstantBuffers = 
			pkMaterial->GetContantBufferList();
		for (unsigned short uk = 0; uk < vConstantBuffers.size(); ++uk)
		{
			CDX11ConstantBuffer* pkConstantBuffer = vConstantBuffers[uk];
			const CDX11ConstantBufferDesc* pkCBDesc = pkConstantBuffer->GetDesc();

			CDX11ConstantBufferData* pkCBData = 
				pkRenderData->GetConstantBufferData(pkCBDesc);
			if (pkCBData == 0)
			{
				pkCBData = IntNew CDX11ConstantBufferData(pkCBDesc);
				pkRenderData->AddConstantBufferData(pkCBData, pkMesh);
			}
		}
	}

	pkMesh->ResetMaterialListDirty();

	return true;
}

//////////////////////////////////////////////////////////////////////////