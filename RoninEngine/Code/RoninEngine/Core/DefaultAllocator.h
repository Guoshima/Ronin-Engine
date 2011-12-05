#pragma once

#include "IAllocator.h"

class CSmallObjectAllocator;

class CDefaultAllocator : public IAllocator
{
public:

	CDefaultAllocator();

	//////////////////////////////////////////////////////////////////////////
	// Implementation of IAllocator
	//////////////////////////////////////////////////////////////////////////

	virtual bool Initialize(const TMemoryType& memType, const THeapInfo& heapInfo);
	virtual void Shutdown();

	virtual void* Allocate(size_t& stSize, const char* pFile, 
		int iLine, const char* pFunction);
	virtual void Deallocate(void* pMemory);
	virtual void* Reallocate(void* pMemory,	size_t& stSize, const char* pFile, 
		int iLine, const char* pFunction);

	//////////////////////////////////////////////////////////////////////////	

	const CSmallObjectAllocator* GetSmallAllocator() const { return m_pSOA; }

protected:

	inline void SetSizeOnMemory(void* pMemory, size_t stSize);
	inline size_t GetSizeFromMemory(void* pMemory);

	//////////////////////////////////////////////////////////////////////////

	THeapInfo m_HeapInfo;	
	TMemoryType m_MemoryType;
	CSmallObjectAllocator* m_pSOA; 
			
	size_t m_stAllocatedMemory;
	size_t m_stNumOfOpenAllocations;

#ifdef USE_MEMORY_TRACKING
	struct SAllocInfo
	{
		stl_string m_sFile;
		int m_iLine;
		stl_string m_sFunction;
	};
	typedef stl_map<void*, SAllocInfo> TAllocInfoMap;
	TAllocInfoMap m_mAllocInfo;
#endif

};