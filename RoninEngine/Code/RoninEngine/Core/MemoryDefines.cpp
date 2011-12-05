#include "CorePCH.h"

#include "MemoryManager.h"

//////////////////////////////////////////////////////////////////////////

#ifdef USE_EASTL

#pragma warning(push)
#pragma warning(disable:4996) 
// EASTL also wants us to define this (see string.h line 197)
int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
#ifdef _MSC_VER
	return _vsnprintf(pDestination, n, pFormat, arguments);
#else
	return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}
#pragma warning(pop)

#endif

//////////////////////////////////////////////////////////////////////////

#pragma warning (push)
#pragma warning (disable:4290)
void* __cdecl operator new(std::size_t stSize) throw (std::bad_alloc)			
{ 
	return _ExternalMalloc(stSize); 
}
void* __cdecl operator new[](std::size_t stSize) throw (std::bad_alloc)			
{ 
	return _ExternalMalloc(stSize);  
}
#pragma warning (pop)
void* __cdecl operator new(std::size_t stSize, const std::nothrow_t&)			
{ 
	return _ExternalMalloc(stSize); 
}
void* __cdecl operator new[](std::size_t stSize, const std::nothrow_t&)			
{ 
	return _ExternalMalloc(stSize);  
}
void __cdecl operator delete(void* pMemory)									
{ 
	_ExternalFree(pMemory); 
}
void __cdecl operator delete[](void* pMemory)	
{
	_ExternalFree(pMemory); 
}

//////////////////////////////////////////////////////////////////////////

#ifdef USE_EASTL

// EASTL expects us to define these, see allocator.h line 194
void* operator new[](size_t size, const char* pName, int flags,
					 unsigned debugFlags, const char* file, int line)
{
	return _ExternalMalloc(size);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset,
					 const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	// this allocator doesn't support alignment
	EASTL_ASSERT(alignment <= 8);
	return _ExternalMalloc(size);
}

#endif

//////////////////////////////////////////////////////////////////////////

void* _InternalMalloc(size_t stSize
#ifdef USE_MEMORY_TRACKING
	  , const char* pFile, int iLine, const char* pFunction
#endif
					  )
{
	if (stSize == 0)
		stSize = 1;

	return CMemoryManager::GetInstance()->Allocate(stSize, 
		MEMORY_TYPE_DEFAULT
#ifdef USE_MEMORY_TRACKING
		, pFile, iLine, pFunction
#endif
		);
}

void* _InternalRealloc(void* pMemory, size_t stSize
#ifdef USE_MEMORY_TRACKING
	, const char* pFile, int iLine, const char* pFunction
#endif
					   )
{
	// If the intention is to use realloc like free, just use free
	if (stSize == 0 && pMemory != 0)
	{
		IntFree(pMemory);
		return NULL;
	}
	else if (pMemory == 0)
	{
		return _InternalMalloc(stSize
#ifdef USE_MEMORY_TRACKING
			, pFile, iLine, pFunction
#endif
			);
	}

	return CMemoryManager::GetInstance()->Reallocate(pMemory, stSize,
		MEMORY_TYPE_DEFAULT
#ifdef USE_MEMORY_TRACKING
		, pFile, iLine, pFunction
#endif
		);
}

void _InternalFree(void* pMemory)
{
	if (pMemory == NULL)
		return;

	CMemoryManager::GetInstance()->Deallocate(pMemory);
}

//////////////////////////////////////////////////////////////////////////

void* _ExternalMalloc(size_t stSize)
{
	if (stSize == 0)
		stSize = 1;

	// for now default malloc, but should also be tracked
	// and pushed over SOA
	return malloc(stSize);
}

void* _ExternalRealloc(size_t stSize, void* pMemory)
{
	// need to implement
	__debugbreak();
	return malloc(stSize);
}

void _ExternalFree(void* pMemory)
{
	if (pMemory == NULL)
		return;

	// same as ExternalMalloc
	free(pMemory);
}

//////////////////////////////////////////////////////////////////////////