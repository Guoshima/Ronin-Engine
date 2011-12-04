#pragma once

#include "CoreMemObject.h"

class CFileSystem;
class CMaterialManager;

class CCoreEngine : public CCoreMemObject
{
public:
	
	// global engine init and shutdown functions
	static bool Init();
	static void Shutdown();
	static CCoreEngine* Get() { return ms_pEngine; }

	// TODO: need to find better name or place for this function
	void UnloadAllData();

	CFileSystem* GetFileSystem() { return m_pFileSystem; }
	CMaterialManager* GetMaterialManager() { return m_pMaterialManager; }
	
private:

	// make constructor/destructor private
	CCoreEngine();
	~CCoreEngine();

	bool Initialize();
	void Cleanup();
	
	//////////////////////////////////////////////////////////////////////////

	// the main engine singleton
	static CCoreEngine* ms_pEngine;

	// core member engine member which can be retrieved through the engine singleton
	CFileSystem* m_pFileSystem;
	CMaterialManager* m_pMaterialManager;
};
