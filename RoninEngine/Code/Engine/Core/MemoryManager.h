#pragma once

#include "MemoryDefines.h"

class IAllocator;

class CMemoryManager
{
public:

	//////////////////////////////////////////////////////////////////////////

	static CMemoryManager* GetInstance();
	static void CreateInstance();
	static void DestroyInstance();

	//////////////////////////////////////////////////////////////////////////

	bool Initialize();
	void Shutdown();

	IAllocator* GetAllocator(TMemoryType memType);
	const IAllocator* GetAllocator(TMemoryType memType) const;

	void* Allocate(size_t& stSize, TMemoryType memType = MEMORY_TYPE_DEFAULT,
		const char* pFile = 0, int iLine = -1, const char* pFunction = 0);
	void Deallocate(void* pMemory, TMemoryType memType = MEMORY_TYPE_DEFAULT);
	void* Reallocate(void* pMemory,	size_t& stSize, TMemoryType memType = MEMORY_TYPE_DEFAULT,
		const char* pFile = 0, int iLine = -1, const char* pFunction = 0);

	//////////////////////////////////////////////////////////////////////////

protected:

	static CMemoryManager* ms_pInstance;

	typedef stl_map<TMemoryType, IAllocator*> TAllocatorMap;
	TAllocatorMap m_Allocators;
			
};