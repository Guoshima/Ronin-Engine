#pragma once

//////////////////////////////////////////////////////////////////////////

#define USE_MEMORY_TRACKING

typedef size_t THeapInfo;
typedef size_t TMemoryType;

enum EMemoryType
{
	MEMORY_TYPE_INVALID = 0,
	MEMORY_TYPE_DEFAULT,
	MEMORY_TYPE_MAX,
};

#ifdef USE_MEMORY_TRACKING

void* __cdecl _InternalMalloc(size_t stSize,
	const char* pFile, int iLine, const char* pFunction);
void __cdecl _InternalFree(void* pMemory);
void* __cdecl _InternalRealloc(void* pMemory, size_t stSize,
	const char* pFile, int iLine, const char* pFunction); 

#define IntNew new(__FILE__, __LINE__, __FUNCTION__)
#define IntDelete delete
#define IntAlloc(T, count) ((T*)_InternalMalloc(sizeof(T)*(count), \
	__FILE__, __LINE__, __FUNCTION__))
#define IntMalloc(size) (_InternalMalloc(size, \
	__FILE__, __LINE__, __FUNCTION__)) 
#define IntRealloc(memory, size) (_InternalRealloc(memory, size, \
	__FILE__, __LINE__, __FUNCTION__))
#define IntFree(memory) _InternalFree(memory)

#else

void* __cdecl _InternalMalloc(size_t stSize);
void __cdecl _InternalFree(void* pMemory);
void* __cdecl _InternalRealloc(void* pMemory, size_t stSize); 

#define IntNew new
#define IntDelete delete
#define IntAlloc(T, count) ((T*)_InternalMalloc(sizeof(T)*(count)))
#define IntMalloc(size) (_InternalMalloc(size)) 
#define IntRealloc(memory, size) (_InternalRealloc(memory, size))
#define IntFree(memory) _InternalFree(memory)

#endif

#define IntSafeDelete(p) { if (p) { IntDelete (p);  (p) = NULL; } }

void* __cdecl _ExternalMalloc(size_t stSize);
void* __cdecl _ExternalRealloc(size_t stSize, void* pMemory);
void __cdecl _ExternalFree(void* pMemory); 

#define ExternalNew new
#define ExternalDelete delete
#define ExternalAlloc(T, count) ((T*)_ExternalMalloc(sizeof(T)*(count)))
#define ExternalMalloc _ExternalMalloc
#define ExternalRealloc _ExternalRealloc
#define ExternalFree _ExternalFree

//////////////////////////////////////////////////////////////////////////
