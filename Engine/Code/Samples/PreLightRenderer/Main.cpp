#include "PreLightRendererPCH.h"

#include "Window.h"
#include "Shlwapi.h"

#include <direct.h>

#include <CoreEngine.h>
#include <FileSystem.h>

#include <Camera.h>

#include <DX11Includes.h>
#include <DX11Renderer.h>

#include <Mesh.h>
#include <MeshData.h>
#include <MeshRenderData.h>
#include <MeshDataHelper.h>
#include <MeshDataEntry.h>
#include <MeshDataEntryDescriptor.h>

#include <ShaderData.h>

#include <Texture.h>

#include <Material.h>

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

struct Vertex_PosN
{
	Vertex_PosN(D3DXVECTOR3 _Pos, D3DXVECTOR3 _Norm) : Pos(_Pos), Norm(_Norm) {}
    D3DXVECTOR3 Pos;
	D3DXVECTOR3 Norm;
};

/*
struct Vertex_PosUV
{
	Vertex_PosUV(D3DXVECTOR3 _Pos, D3DXVECTOR2 _Tex) : Pos(_Pos), Tex(_Tex) {}
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};
*/

struct Vertex_PosUVN
{
	Vertex_PosUVN(D3DXVECTOR3 _Pos, D3DXVECTOR2 _Tex, D3DXVECTOR3 _Norm) : 
		Pos(_Pos), Tex(_Tex), Norm(_Norm) {}
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
	D3DXVECTOR3 Norm;
};

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

/*
ID3D10ShaderResourceView*           g_pTextureRV = NULL;
ID3D10EffectShaderResourceVariable* g_pDiffuseVariable = NULL;
*/

float m_fYaw, m_fPitch, m_fRoll;
CCamera m_kCamera;

CMeshPtr m_pkCube = NULL;
CMeshPtr m_pkPlane = NULL;
CMeshPtr m_pkScreenPlane = NULL;

CShaderDataPtr m_pkColorShaderData = NULL;

CCameraPtr m_pkShadowCamera = NULL;
CRenderTargetPtr m_pkShadowRenderTarget = NULL;
CTexture* m_pkShadowMap = NULL;

CShaderDataPtr m_pkShadowViewProjection = NULL;

CMaterialPtr m_pkMRTMaterial;
CMaterialPtr m_pkShadowMaterial;;

CTexturePtr m_pkNormalBuffer;
CTexturePtr m_pkDepthBuffer;
CRenderTargetPtr m_pkMRTRenderTarget;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------

HRESULT SetupScene();

LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();
void Update();

HRESULT DoRegisterClass(HINSTANCE hInstance)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL; // LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	wcex.hCursor = NULL; //LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = CWindow::GetWindowClassName();
	wcex.hIconSm = NULL; //LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	if( !RegisterClassEx( &wcex ) )
		return E_FAIL;

	return S_OK;
};

void Cleanup()
{
	m_pkCube = 0;
	m_pkPlane = 0;
	m_pkScreenPlane = 0;

	m_pkColorShaderData = 0;

	m_pkShadowCamera = 0;
	m_pkShadowRenderTarget = 0;
	m_pkShadowMap = 0;

	m_pkShadowViewProjection = 0;

	m_pkMRTMaterial = 0;
	m_pkShadowMaterial = 0;

	m_pkNormalBuffer = 0;
	m_pkDepthBuffer = 0;
	m_pkMRTRenderTarget = 0;

	// release all the loaded and cached data, before starting the shutdown process
	CCoreEngine::Get()->UnloadAllData();

	CDX11Renderer::DestroyRenderer();
	CCoreEngine::Shutdown();
}

#include "dinput.h"

// globals
LPDIRECTINPUT8         g_lpDirectInput8;
LPDIRECTINPUTDEVICE8   g_lpKeyboard;
LPDIRECTINPUTDEVICE8   g_lpMouse;

bool SetupInput(HWND hWindowHandle)
{
	if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&g_lpDirectInput8, NULL)))
		return false;

	// initialize the keyboard
	if (FAILED(g_lpDirectInput8->CreateDevice(GUID_SysKeyboard, &g_lpKeyboard, NULL)))
		return false;
	if (FAILED(g_lpKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;
	if (FAILED(g_lpKeyboard->SetCooperativeLevel(hWindowHandle, DISCL_BACKGROUND |
		DISCL_NONEXCLUSIVE)))
		return false;
	if (FAILED(g_lpKeyboard->Acquire()))
		return false;

	// initialize the mouse
	if (FAILED(g_lpDirectInput8->CreateDevice(GUID_SysMouse, &g_lpMouse, NULL)))
		return false;
	if (FAILED(g_lpMouse->SetCooperativeLevel(hWindowHandle, DISCL_BACKGROUND |
		DISCL_NONEXCLUSIVE)))
		return false;
	if (FAILED(g_lpMouse->SetDataFormat(&c_dfDIMouse)))
		return false;
	if (FAILED(g_lpMouse->Acquire()))
		return false;

	return true;
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	// register the class
	if (FAILED(DoRegisterClass(hInstance)))
		return 0;

	// change working directory to bin folder
	char acExe[_MAX_PATH];  
	GetModuleFileName( GetModuleHandle(NULL), acExe, sizeof(acExe));
	PathRemoveFileSpec(acExe);
	char* acResult = strstr(acExe,"\\bin");
	if (acResult)
		*acResult = 0;
	SetCurrentDirectory( acExe );
	//_chdir("..\\..\\..\\bin");
	
	// initialize the engine
	CCoreEngine::Init();

	// set the default data path
	CCoreEngine::Get()->GetFileSystem()->SetDataPath("Data\\");
	
	// create the window
	CWindow::SWindowDesc kWindowDesc;
	kWindowDesc.m_hInstance = hInstance;
	kWindowDesc.m_acWindowCaption = "DX11 Test";
	kWindowDesc.m_uiWidth = 1024;
	kWindowDesc.m_uiHeight = 768;
	kWindowDesc.m_nCmdShow = nCmdShow;

	CWindow* pkWindow = CWindow::Create(kWindowDesc);
	if (pkWindow == 0)
		return 0;

	// create the renderer (it's a singleton)
	CDX11Renderer::CreateRenderer(pkWindow->GetWindowHandle());

	// Create the input system
	SetupInput(pkWindow->GetWindowHandle());

    if( FAILED( SetupScene() ) )
    {
        Cleanup();
		delete pkWindow;
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			Update();
            Render();
        }
    }

    Cleanup();
	delete pkWindow;

    return ( int )msg.wParam;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------

void CreatePlane()
{
	// create the mesh object itself
	m_pkPlane = IntNew CMesh; 

	// create the vertex buffer data
	stl_vector<CMeshDataEntryDescriptorPtr> vVertexDataDescriptors;
	vVertexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_POSITION, 0,
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XYZ));
	vVertexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_TEXCOORD, 0,
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XY));
	vVertexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_NORMAL, 0,
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XYZ));

	CMeshDataEntry* pkVertexBufferEntry = CMeshDataEntry::Create(
		vVertexDataDescriptors, 4, CMeshDataEntry::TYPE_VERTEX_DATA);

	Vertex_PosUVN* pkVertexData = (Vertex_PosUVN*) pkVertexBufferEntry->LockData();

	pkVertexData[0] = Vertex_PosUVN(D3DXVECTOR3( -10.0f, -1.5f, -10.0f ),
		D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3( 0.0f , 1.0f, 0.0f));
	pkVertexData[1] = Vertex_PosUVN(D3DXVECTOR3( -10.0f, -1.5f, 10.0f ),
		D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3( 0.0f , 1.0f, 0.0f));
	pkVertexData[2] = Vertex_PosUVN(D3DXVECTOR3( 10.0f, -1.5f, -10.0f ),
		D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3( 0.0f , 1.0f, 0.0f));
	pkVertexData[3] = Vertex_PosUVN(D3DXVECTOR3( 10.0f, -1.5f, 10.0f ),
		D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3( 0.0f , 1.0f, 0.0f));

	pkVertexBufferEntry->UnLockData();

	// create the index buffer data
	stl_vector<CMeshDataEntryDescriptorPtr> vIndexDataDescriptors;
	vIndexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_INDEX, 0,
		CMeshDataEntryDescriptor::FORMAT_UINT_32_X));

	CMeshDataEntry* pkIndexBufferEntry = CMeshDataEntry::Create(
		vIndexDataDescriptors, 6, CMeshDataEntry::TYPE_INDEX_DATA);

	unsigned int* pkIndexData = (unsigned int*) pkIndexBufferEntry->LockData();

	pkIndexData[0] = 0; pkIndexData[1] = 1; pkIndexData[2] = 3;
	pkIndexData[3] = 0; pkIndexData[4] = 3; pkIndexData[5] = 2;

	pkIndexBufferEntry->UnLockData();

	// finally create the meshdata itself
	CMeshData* pkMeshData = IntNew CMeshData(
		pkIndexBufferEntry, pkVertexBufferEntry);
	pkMeshData->SetTopology(CMeshData::TOPOLOGY_TRIANGLE_LIST);
	m_pkPlane->SetMeshData(pkMeshData);

	// find the material to attach
	m_pkPlane->AddMaterial(m_pkMRTMaterial);
	m_pkPlane->SetActiveMaterial(m_pkMRTMaterial);

	// create the shader data to modify the color
	CShaderData* pkColorShaderData = CShaderData::CreateShaderData(
		"vMeshColor", CShaderData::TYPE_FLOAT, 12);
	float afColor[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
	pkColorShaderData->SetFloatValues(afColor, 12);

	m_pkPlane->AddShaderData(pkColorShaderData);

	// create the shader data to modify the color
	m_pkShadowViewProjection = CShaderData::CreateShaderData(
		"kLightViewProjection", CShaderData::TYPE_FLOAT, 16);
	m_pkPlane->AddShaderData(m_pkShadowViewProjection);

	// attach shadow map to object
	if (m_pkShadowMap)
		m_pkPlane->SetTextureSlot(1, m_pkShadowMap, 0);
}

void CreateCube()
{
	// create the mesh object itself
	m_pkCube = IntNew CMesh;

	// create the vertex buffer data
	stl_vector<CMeshDataEntryDescriptorPtr> vVertexDataDescriptors;
	vVertexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_POSITION, 0,
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XYZ));
	vVertexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_TEXCOORD, 0,
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XY));
	vVertexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_NORMAL, 0,
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XYZ));

	CMeshDataEntry* pkVertexBufferEntry = CMeshDataEntry::Create(
		vVertexDataDescriptors, 24, CMeshDataEntry::TYPE_VERTEX_DATA);

	Vertex_PosUVN* pkVertexData = (Vertex_PosUVN*) pkVertexBufferEntry->LockData();

	pkVertexData[0] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXVECTOR2( 0.0f, 0.0f ),
		D3DXVECTOR3( 0.0f, 1.0f, 0.0f ));
	pkVertexData[1] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXVECTOR2( 1.0f, 0.0f ),
		D3DXVECTOR3( 0.0f, 1.0f, 0.0f ));
	pkVertexData[2] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXVECTOR2( 1.0f, 1.0f ),
		D3DXVECTOR3( 0.0f, 1.0f, 0.0f ));
	pkVertexData[3] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 1.0f ),
		D3DXVECTOR3( 0.0f, 1.0f, 0.0f ));

	pkVertexData[4] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXVECTOR2( 0.0f, 0.0f ),
		D3DXVECTOR3( 0.0f, -1.0f, 0.0f ));
	pkVertexData[5] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXVECTOR2( 1.0f, 0.0f ),
		D3DXVECTOR3( 0.0f, -1.0f, 0.0f ));
	pkVertexData[6] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXVECTOR2( 1.0f, 1.0f ),
		D3DXVECTOR3( 0.0f, -1.0f, 0.0f ));
	pkVertexData[7] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 1.0f ),
		D3DXVECTOR3( 0.0f, -1.0f, 0.0f ));

	pkVertexData[8] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 0.0f ),
		D3DXVECTOR3( -1.0f, 0.0f, 0.0f ));
	pkVertexData[9] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXVECTOR2( 1.0f, 0.0f ),
		D3DXVECTOR3( -1.0f, 0.0f, 0.0f ));
	pkVertexData[10] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXVECTOR2( 1.0f, 1.0f ),
		D3DXVECTOR3( -1.0f, 0.0f, 0.0f ));
	pkVertexData[11] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 1.0f ),
		D3DXVECTOR3( -1.0f, 0.0f, 0.0f ));

	pkVertexData[12] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 0.0f ),
		D3DXVECTOR3( 1.0f, 0.0f, 0.0f ));
	pkVertexData[13] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXVECTOR2( 1.0f, 0.0f ),
		D3DXVECTOR3( 1.0f, 0.0f, 0.0f ));
	pkVertexData[14] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXVECTOR2( 1.0f, 1.0f ),
		D3DXVECTOR3( 1.0f, 0.0f, 0.0f ));
	pkVertexData[15] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 1.0f ),
		D3DXVECTOR3( 1.0f, 0.0f, 0.0f ));

	pkVertexData[16] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXVECTOR2( 0.0f, 0.0f ),
		D3DXVECTOR3( 0.0f, 0.0f, -1.0f ));
	pkVertexData[17] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXVECTOR2( 1.0f, 0.0f ),
		D3DXVECTOR3( 0.0f, 0.0f, -1.0f ));
	pkVertexData[18] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXVECTOR2( 1.0f, 1.0f ),
		D3DXVECTOR3( 0.0f, 0.0f, -1.0f ));
	pkVertexData[19] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXVECTOR2( 0.0f, 1.0f ),
		D3DXVECTOR3( 0.0f, 0.0f, -1.0f ));

	pkVertexData[20] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 0.0f ),
		D3DXVECTOR3( 0.0f, 0.0f, 1.0f ));
	pkVertexData[21] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXVECTOR2( 1.0f, 0.0f ),
		D3DXVECTOR3( 0.0f, 0.0f, 1.0f ));
	pkVertexData[22] = Vertex_PosUVN(D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXVECTOR2( 1.0f, 1.0f ),
		D3DXVECTOR3( 0.0f, 0.0f, 1.0f ));
	pkVertexData[23] = Vertex_PosUVN(D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 1.0f ),
		D3DXVECTOR3( 0.0f, 0.0f, 1.0f ));

	pkVertexBufferEntry->UnLockData();

	// create the index buffer data
	stl_vector<CMeshDataEntryDescriptorPtr> vIndexDataDescriptors;
	vIndexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_INDEX, 0,
		CMeshDataEntryDescriptor::FORMAT_UINT_32_X));

	CMeshDataEntry* pkIndexBufferEntry = CMeshDataEntry::Create(
		vIndexDataDescriptors, 36, CMeshDataEntry::TYPE_INDEX_DATA);

	unsigned int* pkIndexData = (unsigned int*) pkIndexBufferEntry->LockData();

	pkIndexData[0] = 3; pkIndexData[1] = 1; pkIndexData[2] = 0;
	pkIndexData[3] = 2; pkIndexData[4] = 1; pkIndexData[5] = 3;

	pkIndexData[6] = 6; pkIndexData[7] = 4; pkIndexData[8] = 5;
	pkIndexData[9] = 7; pkIndexData[10] = 4; pkIndexData[11] = 6;

	pkIndexData[12] = 11; pkIndexData[13] = 9; pkIndexData[14] = 8;
	pkIndexData[15] = 10; pkIndexData[16] = 9; pkIndexData[17] = 11;

	pkIndexData[18] = 14; pkIndexData[19] = 12; pkIndexData[20] = 13;
	pkIndexData[21] = 15; pkIndexData[22] = 12; pkIndexData[23] = 14;

	pkIndexData[24] = 19; pkIndexData[25] = 17; pkIndexData[26] = 16;
	pkIndexData[27] = 18; pkIndexData[28] = 17; pkIndexData[29] = 19;

	pkIndexData[30] = 22; pkIndexData[31] = 20; pkIndexData[32] = 21;
	pkIndexData[33] = 23; pkIndexData[34] = 20; pkIndexData[35] = 22;
	
	pkIndexBufferEntry->UnLockData();

	// finally create the meshdata itself
	CMeshData* pkMeshData = IntNew CMeshData(
		pkIndexBufferEntry, pkVertexBufferEntry);
	pkMeshData->SetTopology(CMeshData::TOPOLOGY_TRIANGLE_LIST);
	m_pkCube->SetMeshData(pkMeshData);

	// find the material to attach
	m_pkCube->AddMaterial(m_pkMRTMaterial);
	m_pkCube->SetActiveMaterial(m_pkMRTMaterial);

	// create the shader data to modify the color
	//m_pkColorShaderData = CShaderData::CreateShaderData(
	//	"vMeshColor", CShaderData::TYPE_FLOAT, 4);
	//float afColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
	//m_pkColorShaderData->SetFloatValues(afColor, 4);

	m_pkColorShaderData = CShaderData::CreateShaderData(
		"vMeshColor", CShaderData::TYPE_FLOAT, 12);
	float afColor[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
	m_pkColorShaderData->SetFloatValues(afColor, 12);

	m_pkCube->AddShaderData(m_pkColorShaderData);

	m_pkCube->AddShaderData(m_pkShadowViewProjection);

	// attach a texture
	CTexture* pkSourceTexture = CTexture::CreateTextureFromFile(
		"Textures/seafloor.dds");
	if (pkSourceTexture)
	{
		m_pkCube->SetTextureSlot(0, pkSourceTexture, 0);
	}

	// attach shadow map to object
	if (m_pkShadowMap)
		m_pkCube->SetTextureSlot(1, m_pkShadowMap, 0);
}

void UpdateCamera()
{
	CTransform kYaw;
	kYaw.SetXRotation(m_fYaw);

	CTransform kPitch;
	kPitch.SetYRotation(m_fPitch);
/*
	CTransform kRoll;
	kRoll.SetZRotation(m_fRoll);
*/
	CTransform kYawPitchRoll = kYaw * kPitch;// * kPitch;// * kRoll;
	kYawPitchRoll.SetTranslate(m_kCamera.GetWorldLocation());

	m_kCamera.SetWorldTransform(kYawPitchRoll);
}

void SetupShadowData()
{
	// setup the shadow camera
	CTransform kYaw;
	kYaw.SetXRotation((float)D3DX_PI / 4.0f);
	CTransform kPitch;
	kPitch.SetYRotation((float)D3DX_PI / 2.0f);

	CTransform kShadowCameraTransform = kYaw * kPitch;
	kShadowCameraTransform.SetTranslate(D3DXVECTOR3(-3.0f, 3.0f, 0.0f));

	m_pkShadowCamera = IntNew CCamera;
	m_pkShadowCamera->SetWorldTransform(kShadowCameraTransform);
	m_pkShadowCamera->SetNearFarDistance(1.0f, 5.0f);
	m_pkShadowCamera->SetFOV((float) D3DX_PI / 3.0f);

	// create the shadow map used to render the shadow to
	m_pkShadowMap = CTexture::CreateRenderTargetTexture(
		256, 256, CTexture::FORMAT_R32_FLOAT, false, true);
	if (m_pkShadowMap == 0)
		return;

	// create the render target which has texture as depthstencil view only
	m_pkShadowRenderTarget = IntNew CRenderTarget;
	m_pkShadowRenderTarget->Init(m_pkShadowMap);
	//m_pkShadowRenderTarget->Init(1, pkRenderTargets, 0);
	m_pkShadowRenderTarget->SetDepthClear(1.0f);
	m_pkShadowRenderTarget->SetClearColor(D3DXVECTOR4(FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX));
}

void UpdateShadowViewProjectionMatrix()
{
	if (m_pkShadowCamera == 0 || m_pkShadowViewProjection == 0)
		return;

	CTransform kShadowViewTransform;
	m_pkShadowCamera->GetViewMatrix(kShadowViewTransform);
	
	D3DXMATRIXA16 kShadowProjection;
	D3DXMatrixPerspectiveFovLH( &kShadowProjection, 
		m_pkShadowCamera->GetFov(), 1.0f, m_pkShadowCamera->GetNearDistance(), 
		m_pkShadowCamera->GetFarDistance() );

	D3DXMATRIXA16 kShadowViewProj;
	D3DXMatrixIdentity( &kShadowViewProj);
	D3DXMatrixMultiply( &kShadowViewProj, &kShadowViewProj, &kShadowViewTransform.GetData() );
	D3DXMatrixMultiply( &kShadowViewProj, &kShadowViewProj, &kShadowProjection );

	//D3DXMATRIXA16 kShadowViewProj = kShadowViewTransform.GetData() * kShadowProjection;

	D3DXMATRIXA16 kShadowViewProjTranspose;
	D3DXMatrixTranspose(&kShadowViewProjTranspose, &kShadowViewProj);

	m_pkShadowViewProjection->SetFloatValues(
		(float*) kShadowViewProjTranspose, 16);
}

void CreateMRTData()
{
	unsigned uiScreenWidth = CRenderer::GetRenderer()->GetWidth();
	unsigned uiScreenHeight = CRenderer::GetRenderer()->GetHeight();

	// create the depth buffer (which should be resolvable)
	m_pkDepthBuffer = CTexture::CreateRenderTargetTexture(
		uiScreenWidth, uiScreenHeight, CTexture::FORMAT_R32_FLOAT, false, true);
	if (m_pkDepthBuffer == 0)
		return;

	// create the normal buffer (which should be resolvable)
	m_pkNormalBuffer = CTexture::CreateRenderTargetTexture(
		uiScreenWidth, uiScreenHeight, CTexture::FORMAT_R16G16_SNORM, true, false);
	if (m_pkNormalBuffer == 0)
		return;

	stl_vector<CTexturePtr> vRenderTargets;
	vRenderTargets.push_back(m_pkNormalBuffer);

	// create the render target which has texture as depthstencil view only
	m_pkMRTRenderTarget = IntNew CRenderTarget;
	m_pkMRTRenderTarget->Init(1, vRenderTargets, m_pkDepthBuffer);
	m_pkMRTRenderTarget->SetDepthClear(1.0f);

	// create the prelight render target itself
	CMeshData* pkMeshData = CMeshDataHelper::CreateScreenSpaceMeshData();
	m_pkScreenPlane = IntNew CMesh;
	m_pkScreenPlane->SetMeshData(pkMeshData);

	CMaterial* pkMaterial = CMaterial::CreateMaterial("PreLightPass");
	m_pkScreenPlane->AddMaterial(pkMaterial);
	m_pkScreenPlane->SetActiveMaterial(pkMaterial);

	// attach the textures
	if (m_pkNormalBuffer)
		m_pkScreenPlane->SetTextureSlot(0, m_pkNormalBuffer, 0);
	if (m_pkDepthBuffer)
		m_pkScreenPlane->SetTextureSlot(1, m_pkDepthBuffer, 0);
}

HRESULT SetupScene()
{
    HRESULT hr = S_OK;

	m_fYaw = m_fPitch = m_fRoll = 0.0f;

	m_fYaw = (float)D3DX_PI / 12.0f;

	CTransform& kCameraTransform = m_kCamera.GetWorldTransform();
	//kCameraTransform.SetXRotation((float)D3DX_PI / 16.0f);
	kCameraTransform.SetTranslate(D3DXVECTOR3(0.0f, 3.0f, -6.0f));
	//m_kCamera.SetLookAt(D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	UpdateCamera();

	SetupShadowData();

	CreateMRTData();
	
	m_pkMRTMaterial = CMaterial::CreateMaterial("Render_MRT");
	m_pkShadowMaterial = CMaterial::CreateMaterial("Render_Shadow");

	CreatePlane();
	CreateCube();

	UpdateShadowViewProjectionMatrix();

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;
		
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

void RotateCamera(int iDeltaX, int iDeltaY)
{
	m_fPitch += (float) iDeltaX / 200.0f;
	m_fYaw += (float) iDeltaY / 200.0f;
	UpdateCamera();
}

void Update()
{
	// update our input devices
	/*
	if (FAILED(g_lpKeyboard->GetDeviceState(sizeof(UCHAR[256]), (LPVOID)keystate)))
		return false;
	*/
	DIMOUSESTATE mouse_state;
	if (!FAILED(g_lpMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state)))
	{
		if (mouse_state.rgbButtons[0] & 0x80)
		{
			RotateCamera(mouse_state.lX, mouse_state.lY);
		}
	}

	char acKeyBuffer[256];
	if(!FAILED(g_lpKeyboard->GetDeviceState(sizeof(acKeyBuffer),
		(LPVOID)&acKeyBuffer)))
	{
		if ((acKeyBuffer[DIK_UP] & 0x80) || (acKeyBuffer[DIK_W] & 0x80))
		{
			D3DXVECTOR3 kForward = m_kCamera.GetForward();
			m_kCamera.SetWorldLocation(m_kCamera.GetWorldLocation() + 
				kForward * 0.05f);
		}

		if ((acKeyBuffer[DIK_DOWN] & 0x80) || (acKeyBuffer[DIK_S] & 0x80))
		{
			D3DXVECTOR3 kForward = m_kCamera.GetForward();
			m_kCamera.SetWorldLocation(m_kCamera.GetWorldLocation() - 
				kForward * 0.05f);
		}

		if ((acKeyBuffer[DIK_LEFT] & 0x80) || (acKeyBuffer[DIK_A] & 0x80))
		{
			D3DXVECTOR3 kRight = m_kCamera.GetRight();
			m_kCamera.SetWorldLocation(m_kCamera.GetWorldLocation() - 
				kRight * 0.05f);
		}

		if ((acKeyBuffer[DIK_RIGHT] & 0x80) || (acKeyBuffer[DIK_D] & 0x80))
		{
			D3DXVECTOR3 kRight = m_kCamera.GetRight();
			m_kCamera.SetWorldLocation(m_kCamera.GetWorldLocation() + 
				kRight * 0.05f);
		}

		if ((acKeyBuffer[DIK_R] & 0x80))
		{
			CRenderer::GetRenderer()->ReloadShaders();
		}
	}


	// Update our time
	static float t = 0.0f;

	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if( dwTimeStart == 0 )
			dwTimeStart = dwTimeCur;
		t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
	}
	
	if (m_pkCube)
	{
		m_pkCube->GetLocalTransform().SetYRotation(t);
		m_pkCube->UpdateWorldData();
	}

	/*
	CTransform& kCameraTransform = m_kCamera.GetWorldTransform();
	kCameraTransform.SetXRotation(t);
	kCameraTransform.SetTranslate(D3DXVECTOR3(0.0f, 3.0f, -6.0f));
*/

	if (m_pkColorShaderData)
	{
		float afColor[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

		//float afColor[4];
		afColor[0] = ( sinf( t * 1.0f ) + 1.0f ) * 0.5f;
		afColor[1] = ( cosf( t * 3.0f ) + 1.0f ) * 0.5f;
		afColor[2] = ( sinf( t * 5.0f ) + 1.0f ) * 0.5f;
		afColor[3] = 0.0f;
		m_pkColorShaderData->SetFloatValues(afColor, 12);
	}
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------

void Render()
{
	CDX11Renderer* pkRenderer = CDX11Renderer::GetDX11Renderer();

	// first render to the mrt pass
	{	
		// bind the default render target
		pkRenderer->BindRenderTarget(CRenderTarget::CLEAR_ALL, 
			m_pkMRTRenderTarget);

		// set the default camera data
		pkRenderer->SetCameraData(&m_kCamera);

		// render the cube
		if (m_pkPlane)
		{
			m_pkPlane->Render(pkRenderer);
		}

		// render the cube
		if (m_pkCube)
		{
			m_pkCube->Render(pkRenderer);
		}

		pkRenderer->UnbindRenderTarget();
	}

	/*
	// first update the shadow map
	{
		// bind the default render target
		pkRenderer->BindRenderTarget(CRenderTarget::CLEAR_ALL, 
			m_pkShadowRenderTarget);

		// set the current camera data
		pkRenderer->SetCameraData(m_pkShadowCamera);

		// render the cube
		if (m_pkCube)
		{
			CMaterial* pkActiveMaterial = m_pkCube->GetActiveMaterial();
			m_pkCube->SetActiveMaterial(m_pkShadowMaterial);
			m_pkCube->Render(pkRenderer);
			m_pkCube->SetActiveMaterial(pkActiveMaterial);
		}


		pkRenderer->UnbindRenderTarget();
	}
	*/

	// now perform the prelight pass
	{
		// bind the default render target
		pkRenderer->BindRenderTarget(CRenderTarget::CLEAR_ALL);

		// no camera data required - screenspace

		// render the meshes
		if (m_pkScreenPlane)
			m_pkScreenPlane->Render(pkRenderer);

		pkRenderer->UnbindRenderTarget();
	}

	// now render the scene
	{
		// bind the default render target
		//pkRenderer->BindRenderTarget(CRenderTarget::CLEAR_ALL);
		
		/*
		// set the current camera data
		pkRenderer->SetCameraData(&m_kCamera);

		// render the meshes
		if (m_pkPlane)
			m_pkPlane->Render(pkRenderer);

		if (m_pkCube) 
			m_pkCube->Render(pkRenderer);
			pkRenderer->UnbindRenderTarget();
			*/
	}

	// finally present the frame
	pkRenderer->PresentFrame();
}


