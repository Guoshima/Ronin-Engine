#include "CorePCH.h"

#include "DefaultAllocator.h"
#include "SmallObjectAllocator.h"

//////////////////////////////////////////////////////////////////////////

CDefaultAllocator::CDefaultAllocator() : m_HeapInfo(0), 
	m_MemoryType(MEMORY_TYPE_INVALID), m_pSOA(0), m_stAllocatedMemory(0),
	m_stNumOfOpenAllocations(0)
{
}

//////////////////////////////////////////////////////////////////////////

bool CDefaultAllocator::Initialize(
	const TMemoryType& memType, const THeapInfo& heapInfo)
{
	m_MemoryType = memType;
	m_HeapInfo = heapInfo;

	return true;
}

void CDefaultAllocator::Shutdown()
{
#ifdef USE_MEMORY_TRACKING
	char acInfo[128];
	sprintf_s(acInfo, 128, "Leaked Allocations: %d \n", m_mAllocInfo.size());
	OutputDebugString(acInfo);	
	for (TAllocInfoMap::const_iterator it = m_mAllocInfo.begin(); it != m_mAllocInfo.end(); ++it)
	{
		const SAllocInfo& info = it->second;

		char acTmp[512];
		sprintf_s(acTmp, 512, "Leak: %s %s %d \n", info.m_sFile.c_str(), info.m_sFunction.c_str(), info.m_iLine);
		OutputDebugString(acTmp);
	}
	m_mAllocInfo.clear();
#endif
}

//////////////////////////////////////////////////////////////////////////

inline void CDefaultAllocator::SetSizeOnMemory(void* pMemory, size_t stSize)
{
    char* pcMemory = (char*) pMemory;
    char* pcSize = (char*)&stSize;
    
    pcMemory[0] = pcSize[0];
    pcMemory[1] = pcSize[1];
    pcMemory[2] = pcSize[2];
    pcMemory[3] = pcSize[3];
}

inline size_t CDefaultAllocator::GetSizeFromMemory(void* pMemory)
{
    size_t stSize;

    char* pcMemory = (char*) pMemory;
    char* pcSize = (char*)&stSize;
    pcSize[0] = pcMemory[0];
    pcSize[1] = pcMemory[1];
    pcSize[2] = pcMemory[2];
    pcSize[3] = pcMemory[3];
    
    return stSize;
}

//////////////////////////////////////////////////////////////////////////

void* CDefaultAllocator::Allocate(size_t& stSize, const char* pFile, 
	int iLine, const char* pFunction)
{
    void* pMemory = NULL;

	stSize += sizeof(size_t);

	if (m_pSOA && stSize < SOA_MAX_OBJECT_SIZE)
	{
		
	} else {
		pMemory = ExternalMalloc(stSize);
	}
    
#ifdef USE_MEMORY_TRACKING
	SAllocInfo info;
	info.m_iLine = iLine;
	info.m_sFile = pFile;
	info.m_sFunction = pFunction;
	m_mAllocInfo[pMemory] = info;
#endif

	//*((size_t*) pMemory) = stSize;
    SetSizeOnMemory(pMemory, stSize);
    pMemory = ((char*)pMemory) + sizeof(size_t);
    
	m_stAllocatedMemory += stSize;
	m_stNumOfOpenAllocations++;

	return pMemory;
}

void CDefaultAllocator::Deallocate(void* pMemory)
{
	// retrieve size from object
	pMemory = ((char*)pMemory) - sizeof(size_t);
	//size_t stSize = *((size_t*) pMemory); 
	size_t stSize = GetSizeFromMemory(pMemory);

#ifdef USE_MEMORY_TRACKING
	TAllocInfoMap::iterator it = m_mAllocInfo.find(pMemory);
	if (it != m_mAllocInfo.end())
	{
		m_mAllocInfo.erase(it);
	} else {
		__debugbreak();
	}
#endif

	m_stAllocatedMemory -= stSize;
	m_stNumOfOpenAllocations--;

	if (m_pSOA && stSize < SOA_MAX_OBJECT_SIZE)
	{
		return;
	} 

	ExternalFree(pMemory);
}

void* CDefaultAllocator::Reallocate(void* pMemory, size_t& stSize, 
	const char* pFile, int iLine, const char* pFunction)
{
	__debugbreak();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////