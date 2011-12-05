#pragma once

#include <RenderTarget.h>
#include <Transform.h>

class CDX11Renderer;

class CMesh;
class CCamera;

class CDX11HelperFunctions
{
public:

	struct SActiveData
	{
		CRenderTarget* ms_pkActiveRenderTarget;
		CMesh* ms_pkCurrentMesh;

		CTransform m_kCurrentViewMatrix;
		CTransform m_kCurrentProjectionMatrix;
	};

	static bool Init(CDX11Renderer* pkRenderer);

	static SActiveData* GetActiveData() { return &ms_kActiveData; }

	static bool SetCameraData(const CCamera* pkCamera);
	static bool BindRenderTarget(CRenderTarget* pkRenderTarget,
		CRenderTarget::EClearFlags eClearFlags);
	static bool UnbindRenderTarget();
	static bool ClearRenderTarget(CRenderTarget* pkRenderTarget,
		CRenderTarget::EClearFlags eClearFlags);
	static bool RenderMesh(CMesh* pkMesh);

protected:

	static bool ValidateRenderTarget(CRenderTarget* pkRenderTarget);
	static bool ValidateMeshRenderData(CMesh* pkMesh);
	static bool ValidateMeshMaterialData(CMesh* pkMesh);

private:

	static CDX11Renderer* ms_pkRenderer;
	static ID3D11RenderTargetView* ms_ppkActiveRenderTargetViews[
		D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	static SActiveData ms_kActiveData;

};