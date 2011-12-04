#include "CorePCH.h"

#include "CoreEngine.h"

#include "MemoryManager.h"

#include "MaterialManager.h"
#include "FileSystem.h"
#include "StreamingEngine.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ISystemEventListener::ISystemEventListener()
{
	CCoreEngine::Get()->RegisterSystemEventListener(this);
}

ISystemEventListener::~ISystemEventListener()
{
	CCoreEngine::Get()->UnregisterSystemEventListener(this);
}

//////////////////////////////////////////////////////////////////////////
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

CCoreEngine::CCoreEngine() : m_pFileSystem(0), m_pMaterialManager(0),
	m_bHasFocus(false)
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
	
	m_pStreamingEngine = IntNew CStreamingEngine;
	if (!m_pStreamingEngine->Initialize())
		return false;

	m_pMaterialManager = IntNew CMaterialManager;

	return true;
}

void CCoreEngine::Cleanup()
{
	IntSafeDelete(m_pMaterialManager);
	IntSafeDelete(m_pStreamingEngine);
	IntSafeDelete(m_pFileSystem);

	// all system event listeners should be unregistered by now
	assert(m_SystemEventListeners.empty());
}

//////////////////////////////////////////////////////////////////////////

void CCoreEngine::UnloadAllData()
{
	m_pMaterialManager->Clear();
}

//////////////////////////////////////////////////////////////////////////

void CCoreEngine::RegisterSystemEventListener(
	ISystemEventListener* pListener)
{
	// make sure listener is only registered once
	TSystemEventListenerList::iterator itFindRes = stl_find(
		m_SystemEventListeners.begin(), m_SystemEventListeners.end(), pListener);
	if (itFindRes != m_SystemEventListeners.end())
		return;

	m_SystemEventListeners.push_back(pListener);
}

bool CCoreEngine::UnregisterSystemEventListener(
	ISystemEventListener* pListener)
{
	TSystemEventListenerList::iterator itFindRes = stl_find(
		m_SystemEventListeners.begin(), m_SystemEventListeners.end(), pListener);
	if (itFindRes != m_SystemEventListeners.end())
	{
		m_SystemEventListeners.erase(itFindRes);
		return true;
	}

	return false;
}

void CCoreEngine::SendSystemEvent(
	ISystemEventListener::ESystemEvent eEvent)
{
	for (TSystemEventListenerList::iterator it = m_SystemEventListeners.begin();
		it != m_SystemEventListeners.end(); ++it)
	{
		(*it)->OnSystemEvent(eEvent);
	}
}

//////////////////////////////////////////////////////////////////////////

void CCoreEngine::SetFocus(bool bHasFocus)
{
	m_bHasFocus = bHasFocus;
	SendSystemEvent(ISystemEventListener::ESystemEvent_FocusChange);
}

//////////////////////////////////////////////////////////////////////////