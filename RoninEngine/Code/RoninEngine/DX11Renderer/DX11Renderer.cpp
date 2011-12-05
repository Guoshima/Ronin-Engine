#include "DX11RendererPCH.h"

#include "DX11Renderer.h"

#include "DX11EffectLoader.h"
#include "DX11RenderTarget.h"

#include "DX11MeshDataEntryCreator.h"
#include "DX11MaterialFactory.h"
#include "DX11ShaderDataFactory.h"
#include "DX11TextureDataFactory.h"

#include "DX11StateFactory.h"

#include "DX11EnumConverters.h"

#include "DX11HelperFunctions.h"

#include "DX11RenderTargetTextureData2D.h"

#include <Camera.h>

//////////////////////////////////////////////////////////////////////////

char CDX11Renderer::ms_acLastErrorMessage[] = ""; 

//////////////////////////////////////////////////////////////////////////

CDX11Renderer* CDX11Renderer::CreateRenderer(
	HWND hWindowHandle)
{
	RECT rc;
	GetClientRect( hWindowHandle, &rc );
	unsigned int uiWidth = rc.right - rc.left;
	unsigned int uiHeight = rc.bottom - rc.top;

	CDX11Renderer* pkDX11Renderer = IntNew CDX11Renderer;
	ms_pkRenderer = pkDX11Renderer;

	if (!pkDX11Renderer->CreateDeviceAndSwapChain(hWindowHandle, uiWidth, uiHeight))
	{
		DestroyRenderer();
		return 0;
	}

	if (!pkDX11Renderer->CreateDefaultData(uiWidth, uiHeight))
	{
		DestroyRenderer();
		return 0;
	}

	if (!pkDX11Renderer->LoadEffectFiles())
	{
		DestroyRenderer();
		return 0;
	}

	CDX11HelperFunctions::Init(pkDX11Renderer);

	CDX11MeshDataEntryCreator::CreateInstance();
	CDX11MaterialFactory::CreateInstance();
	CDX11ShaderDataFactory::CreateInstance();
	CDX11TextureDataFactory::CreateInstance();

	CDX11StateFactory::SetD3DDevice(pkDX11Renderer->m_pd3dDevice);

	CDX11EnumConverters::InitData();

	return pkDX11Renderer;
}

void CDX11Renderer::DestroyRenderer()
{
	CDX11StateFactory::SetD3DDevice(0);
	CDX11EnumConverters::ReleaseData();

	CDX11MeshDataEntryCreator::DestroyInstance();
	CDX11MaterialFactory::DestroyInstance();
	CDX11ShaderDataFactory::DestroyInstance();
	CDX11TextureDataFactory::DestroyInstance();

	IntDelete ms_pkRenderer;
	ms_pkRenderer = 0;
}

CDX11Renderer* CDX11Renderer::GetDX11Renderer()
{
	return (CDX11Renderer*) ms_pkRenderer;
}

//////////////////////////////////////////////////////////////////////////

CDX11Renderer::CDX11Renderer() : m_pd3dDevice(NULL), 
m_pd3dImmediateDeviceContext(0), m_eD3DDriverType(D3D_DRIVER_TYPE_UNKNOWN),
m_pSwapChain(NULL), m_spDefaultRenderTarget(NULL),
m_pkEffectManager(0)
{

}

CDX11Renderer::~CDX11Renderer()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////////

void CDX11Renderer::Cleanup()
{
	IntDelete m_pkEffectManager;
	m_pkEffectManager = 0;

	m_spDefaultRenderTarget = 0;

	if( m_pd3dImmediateDeviceContext ) m_pd3dImmediateDeviceContext->ClearState();
	if( m_pd3dImmediateDeviceContext ) m_pd3dImmediateDeviceContext->Flush();
	if( m_pd3dImmediateDeviceContext) m_pd3dImmediateDeviceContext->Release();

	if( m_pSwapChain ) m_pSwapChain->Release();
	if( m_pd3dDevice ) m_pd3dDevice->Release();
}

//////////////////////////////////////////////////////////////////////////

bool CDX11Renderer::CreateDeviceAndSwapChain(
	HWND hWindowHandle, unsigned int uiWidth, unsigned int uiHeight)
{
	HRESULT hr = S_OK;	

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// only hardware driver type for now
	m_eD3DDriverType = D3D_DRIVER_TYPE_HARDWARE;

	// try highest feature level
	m_eD3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	// use the default adapater for now ..

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = uiWidth;
	sd.BufferDesc.Height = uiHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWindowHandle;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain( NULL, m_eD3DDriverType, NULL, createDeviceFlags,
		NULL, 0, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, 
		&m_eD3DFeatureLevel, &m_pd3dImmediateDeviceContext );
	
	if( FAILED( hr ) )
		return false;

	//////////////////////////////////////////////////////////////////////////
/*
	// Look for 'NVIDIA PerfHUD' adapter 
	// If it is present, override default settings 
	IDXGIFactory *pDXGIFactory; 
	ID3D10Device *pDevice; 
	HRESULT hRes; 

	hRes = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pDXGIFactory); 

	// Search for a PerfHUD adapter.   
	UINT nAdapter = 0; 
	IDXGIAdapter* adapter = NULL; 
	IDXGIAdapter* selectedAdapter = NULL; 
	D3D10_DRIVER_TYPE driverType = D3D10_DRIVER_TYPE_HARDWARE; 

	while (pDXGIFactory->EnumAdapters(nAdapter, &adapter) != 
		DXGI_ERROR_NOT_FOUND) 
	{ 
		if (adapter) 
		{ 
			DXGI_ADAPTER_DESC adaptDesc; 
			if (SUCCEEDED(adapter->GetDesc(&adaptDesc))) 
			{ 
				const bool isPerfHUD = wcscmp(adaptDesc.Description, L"NVIDIA PerfHUD") == 0; 

				// Select the first adapter in normal circumstances or the PerfHUD one if it exists. 
				if(nAdapter == 0 || isPerfHUD) 
					selectedAdapter = adapter; 

				if(isPerfHUD) 
					driverType = D3D11_DRIVER_TYPE_REFERENCE; 

			} 
		} 
		++nAdapter; 
	} 

	if(FAILED(D3D11CreateDevice( selectedAdapter, driverType, NULL, 
		0, D3D10_SDK_VERSION, &pDevice))) 
		return E_FAIL; 
*/
	//////////////////////////////////////////////////////////////////////////

	m_uiWidth = uiWidth;
	m_uiHeight = uiHeight;

	return true;
}

bool CDX11Renderer::CreateDefaultData(
	unsigned int uiWidth, unsigned int uiHeight)
{
	HRESULT hr = S_OK;

	// Create a render target view
	ID3D11Texture2D* pBuffer;
	hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), 
		( LPVOID* )&pBuffer );
	if( FAILED( hr ) )
		return false;

	D3D11_TEXTURE2D_DESC kDesc;
	pBuffer->GetDesc(&kDesc);

	CTextureData::SRenderTargetTextureCreateParams kParams;
	kParams.m_bDepthResolvable = false;
	kParams.m_bRenderTargetResolvable = true;
	kParams.m_bResourceView = false;
	kParams.m_uiHeight = kDesc.Height;
	kParams.m_uiWidth = kDesc.Width;

	CDX11RenderTargetTextureData2D* pkRenderData = 
		IntNew CDX11RenderTargetTextureData2D(kParams, pBuffer);
	pkRenderData->CreateD3DData(m_pd3dDevice);

	stl_vector<CTexturePtr> vRenderTargets;
	vRenderTargets.push_back(IntNew CTexture(pkRenderData, true));

	m_spDefaultRenderTarget = IntNew CRenderTarget();
	m_spDefaultRenderTarget->Init(1, vRenderTargets, 0);

	D3DXVECTOR4 kClearColor(0.0f, 0.125f, 0.3f, 1.0f);
	m_spDefaultRenderTarget->SetClearColor(kClearColor);

	return true;
}

bool CDX11Renderer::LoadEffectFiles()
{
	m_pkEffectManager = IntNew CDX11EffectManager;
	m_pkEffectManager->LoadEffectsFromFolder(m_pd3dDevice,
		"Shaders\\");
	if (m_pkEffectManager->GetNumOfEffects() > 0)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////

void CDX11Renderer::ReloadShaders()
{
	if (m_pkEffectManager)
		m_pkEffectManager->ReloadEffects(m_pd3dDevice);
}

void CDX11Renderer::RenderMesh(CMesh* pkMesh)
{
	CDX11HelperFunctions::RenderMesh(pkMesh);
}

//////////////////////////////////////////////////////////////////////////

void CDX11Renderer::BindRenderTarget(
	CRenderTarget::EClearFlags eClearFlags, 
	CRenderTarget* pkRenderTarget)
{
	if (pkRenderTarget == 0)
		pkRenderTarget = m_spDefaultRenderTarget;

	CDX11HelperFunctions::BindRenderTarget(pkRenderTarget, eClearFlags);

}

void CDX11Renderer::UnbindRenderTarget()
{
	CDX11HelperFunctions::UnbindRenderTarget();
}

//////////////////////////////////////////////////////////////////////////

void CDX11Renderer::SetCameraData(const CCamera* pkCamera)
{
	CDX11HelperFunctions::SetCameraData(pkCamera);
}

//////////////////////////////////////////////////////////////////////////

const D3DXMATRIXA16& CDX11Renderer::GetCurrentViewMatrix() const
{
	return CDX11HelperFunctions::GetActiveData()->
		m_kCurrentViewMatrix.GetData();
}

const D3DXMATRIXA16& CDX11Renderer::GetCurrentProjectionMatrix() const
{
	return CDX11HelperFunctions::GetActiveData()->
		m_kCurrentProjectionMatrix.GetData();
}

//////////////////////////////////////////////////////////////////////////

void CDX11Renderer::PresentFrame()
{
	 m_pSwapChain->Present( 0, 0 );
}

//////////////////////////////////////////////////////////////////////////