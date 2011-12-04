#pragma once

#include <Renderer.h>
#include <Transform.h>
#include <RenderTarget.h>

class CCamera;

class CDX11EffectManager;

class CDX11Renderer : public CRenderer
{
	
	friend class CDX11HelperFunctions;

public:

	static CDX11Renderer* CreateRenderer(HWND hWindowHandle);
	static void DestroyRenderer();

	static CDX11Renderer* GetDX11Renderer();
	static const char* GetLastErrorMessage() { return ms_acLastErrorMessage; }

	//////////////////////////////////////////////////////////////////////////

	~CDX11Renderer();

	ID3D11Device* GetDevice() { return m_pd3dDevice; }
	ID3D11DeviceContext* GetImmediateDeviceContext() { return m_pd3dImmediateDeviceContext; }

	CDX11EffectManager* GetEffectManager() { return m_pkEffectManager; }
	CRenderTarget* GetDefaultRenderTarget() { return m_spDefaultRenderTarget; }

	//////////////////////////////////////////////////////////////////////////
	// Implementation of the default renderer
	//////////////////////////////////////////////////////////////////////////

	void ReloadShaders();
	void RenderMesh(CMesh* pkMesh);

	void BindRenderTarget(CRenderTarget::EClearFlags eClearFlags, 
		CRenderTarget* pkRenderTarget = 0);
	void UnbindRenderTarget();
	CRenderTarget* GetActiveRenderTarget();

	void SetCameraData(const CCamera* pkCamera);

	void PresentFrame();

	//////////////////////////////////////////////////////////////////////////
	//	Local Helper function to get current state
	//////////////////////////////////////////////////////////////////////////

	const D3DXMATRIXA16& GetCurrentViewMatrix() const;
	const D3DXMATRIXA16& GetCurrentProjectionMatrix() const;

private:

	CDX11Renderer();

	void Cleanup();

	bool CreateDeviceAndSwapChain(HWND hWindowHandle, 
		unsigned int uiWidth, unsigned int uiHeight);
	bool CreateDefaultData(unsigned int uiWidth, unsigned int uiHeight);
	bool LoadEffectFiles();

	//////////////////////////////////////////////////////////////////////////

	static char ms_acLastErrorMessage[];

	D3D_DRIVER_TYPE m_eD3DDriverType;
	D3D_FEATURE_LEVEL m_eD3DFeatureLevel;

	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pd3dImmediateDeviceContext;
	IDXGISwapChain* m_pSwapChain;

	CDX11EffectManager* m_pkEffectManager;

	CRenderTargetPtr m_spDefaultRenderTarget;
};