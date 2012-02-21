#pragma once

#include "CoreMemObject.h"

class CFileSystem;
class CMaterialManager;
class CStreamEngine;

class ISystemEventListener
{
public:

	ISystemEventListener();
	virtual ~ISystemEventListener();

	enum ESystemEvent 
	{
		ESystemEvent_FocusChange = 0,
	};

	virtual void OnSystemEvent(ESystemEvent eEvent) = 0;
};

class CCoreEngine : public CCoreMemObject
{
public:
	
	// global engine init and shutdown functions
	static bool Init();
	static void Shutdown();
	static CCoreEngine* Get() { return ms_pEngine; }

	// TODO: need to find better name or place for this function
	void UnloadAllData();

	//////////////////////////////////////////////////////////////////////////

	CFileSystem* GetFileSystem() { return m_pFileSystem; }
	CMaterialManager* GetMaterialManager() { return m_pMaterialManager; }
	CStreamEngine* GetStreamEngine() { return m_pStreamEngine; }

	//////////////////////////////////////////////////////////////////////////

	void SetFocus(bool bHasFocus);
	bool HasFocus() const { return m_bHasFocus; }
	
	void RegisterSystemEventListener(ISystemEventListener* pListener);
	bool UnregisterSystemEventListener(ISystemEventListener* pListener);

private:

	// make constructor/destructor private
	CCoreEngine();
	~CCoreEngine();

	bool Initialize();
	void Cleanup();

	void SendSystemEvent(ISystemEventListener::ESystemEvent eEvent);
	
	//////////////////////////////////////////////////////////////////////////

	// the main engine singleton
	static CCoreEngine* ms_pEngine;

	// does the application currently have focus
	bool m_bHasFocus;

	// core member engine member which can be retrieved through the engine singleton
	CFileSystem* m_pFileSystem;
	CMaterialManager* m_pMaterialManager;
	CStreamEngine* m_pStreamEngine;

	// listerens of general system events
	typedef stl_vector<ISystemEventListener*> TSystemEventListenerList;
	TSystemEventListenerList m_SystemEventListeners;
};

// easy macro's to retrieve the core member
#define FILE_SYSTEM CCoreEngine::Get()->GetFileSystem()
#define STREAM_ENGINE CCoreEngine::Get()->GetStreamEngine()
#define MATERIAL_MANAGER CCoreEngine::Get()->GetMaterialManager()
