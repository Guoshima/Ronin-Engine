#include "CorePCH.h"

#include "MemoryManager.h"

#include "DefaultAllocator.h"

//////////////////////////////////////////////////////////////////////////

CMemoryManager* CMemoryManager::ms_pInstance = 0;

//////////////////////////////////////////////////////////////////////////

CMemoryManager* CMemoryManager::GetInstance()
{
	return ms_pInstance;
}

void CMemoryManager::CreateInstance()
{
	if (ms_pInstance == 0)
		ms_pInstance = ExternalNew CMemoryManager();
}

void CMemoryManager::DestroyInstance()
{
	ExternalDelete ms_pInstance;
	ms_pInstance = 0;
}

//////////////////////////////////////////////////////////////////////////

bool CMemoryManager::Initialize()
{
	IAllocator* pDefaultAllocator = ExternalNew CDefaultAllocator();
	if (!pDefaultAllocator->Initialize(MEMORY_TYPE_DEFAULT, 0))
	{
		ExternalDelete pDefaultAllocator;
		return false;
	}

	m_Allocators[MEMORY_TYPE_DEFAULT] = pDefaultAllocator;
	return true;
}

void CMemoryManager::Shutdown()
{
	for (TAllocatorMap::iterator it = m_Allocators.begin(); it != m_Allocators.end(); ++it)
	{
		IAllocator* pAllocator = it->second;
		pAllocator->Shutdown();
		ExternalDelete pAllocator;
	}
	m_Allocators.clear();
}

//////////////////////////////////////////////////////////////////////////

IAllocator* CMemoryManager::GetAllocator(TMemoryType memType)
{
	TAllocatorMap::iterator findRes = m_Allocators.find(memType);
	if (findRes == m_Allocators.end())
		return 0;

	return findRes->second;
}

const IAllocator* CMemoryManager::GetAllocator(TMemoryType memType) const
{
	TAllocatorMap::const_iterator findRes = m_Allocators.find(memType);
	if (findRes == m_Allocators.end())
		return 0;

	return findRes->second;
}

//////////////////////////////////////////////////////////////////////////

void* CMemoryManager::Allocate(size_t& stSize, TMemoryType memType,
	const char* pFile, int iLine, const char* pFunction)
{
	IAllocator* pAllocator = GetAllocator(memType);
	if (pAllocator)
		return pAllocator->Allocate(stSize, pFile, iLine, pFunction);

	return NULL;
}
void CMemoryManager::Deallocate(void* pMemory, TMemoryType memType)
{
	IAllocator* pAllocator = GetAllocator(memType);
	if (pAllocator)
		return pAllocator->Deallocate(pMemory);
}
void* CMemoryManager::Reallocate(void* pMemory,	size_t& stSize, TMemoryType memType,
	const char* pFile, int iLine, const char* pFunction)
{
	IAllocator* pAllocator = GetAllocator(memType);
	if (pAllocator)
		return pAllocator->Reallocate(pMemory, stSize, pFile, iLine, pFunction);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////