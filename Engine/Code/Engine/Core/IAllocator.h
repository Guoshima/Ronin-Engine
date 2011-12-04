#pragma once

#include "MemoryDefines.h"

class IAllocator
{
public:

	inline virtual ~IAllocator(){};

	// global initialize functions
	virtual bool Initialize(const TMemoryType& memType, const THeapInfo& heapInfo) = 0;
	virtual void Shutdown() = 0;

	// Encapsulate all memory management through a simple set of calls
	virtual void* Allocate(size_t& stSizeInBytes, const char* pFile, 
		int iLine, const char* pFunction) = 0;
	virtual void Deallocate(void* pvMemory) = 0;
	virtual void* Reallocate(void* pvMemory, size_t& stSizeInBytes, 
		const char* pFile, int iLine, const char* pFunction) = 0;
};