#include "CorePCH.h"

#include "CoreEngine.h"

#include "MemoryManager.h"
#include "MaterialManager.h"
#include "FileSystem.h"

//////////////////////////////////////////////////////////////////////////

// the global engine object
CCoreEngine* CCoreEngine::ms_pEngine = 0;

//////////////////////////////////////////////////////////////////////////

bool CCoreEngine::Init()
{
	// start by creating the memory manager!
	CMemoryManager::CreateInstance();
	CMemoryManager::GetInstance()->Initialize();

	ms_pEngine = IntNew CCoreEngine;
	return ms_pEngine->Initialize();
}

void CCoreEngine::Shutdown()
{
	IntSafeDelete(ms_pEngine);

	CMemoryManager::GetInstance()->Shutdown();
	CMemoryManager::DestroyInstance();
}

//////////////////////////////////////////////////////////////////////////

CCoreEngine::CCoreEngine() : m_pFileSystem(0), m_pMaterialManager(0)
{
}

CCoreEngine::~CCoreEngine()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////////

bool CCoreEngine::Initialize()
{
	m_pFileSystem = IntNew CFileSystem;
	m_pMaterialManager = IntNew CMaterialManager;

	return true;
}

void CCoreEngine::Cleanup()
{
	IntSafeDelete(m_pMaterialManager);
	IntSafeDelete(m_pFileSystem);
}

//////////////////////////////////////////////////////////////////////////

void CCoreEngine::UnloadAllData()
{
	m_pMaterialManager->Clear();
}

//////////////////////////////////////////////////////////////////////////