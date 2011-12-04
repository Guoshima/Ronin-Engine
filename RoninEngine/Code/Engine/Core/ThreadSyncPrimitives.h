#pragma once

#include "CoreMemObject.h"

// Wrapper class for a Critical Section (contains platform specific code)
class CCriticalSection : CCoreMemObject
{
public:

	CCriticalSection(unsigned int uiSpinCount = 0) : m_uiSpinCount(0)
#ifdef _DEBUG
		, m_ulThreadOwner(0), m_uiLockCount(0)
#endif
	{
		InitializeCriticalSectionEx(&m_kCriticalSection, m_uiSpinCount, 0);
	}

	~CCriticalSection()
	{
		DeleteCriticalSection(&m_kCriticalSection);
	}

	void Lock()
	{
		EnterCriticalSection(&m_kCriticalSection);
#ifdef _DEBUG
		m_ulThreadOwner = GetCurrentThreadId();
		m_uiLockCount++;
#endif
	}
	void Unlock()
	{
#ifdef _DEBUG
		m_uiLockCount--;
		if (m_uiLockCount == 0)
			m_ulThreadOwner = 0;
#endif
		LeaveCriticalSection(&m_kCriticalSection);
	}

	unsigned int GetSpinCount() const  { return m_uiSpinCount; }

#ifdef _DEBUG
	unsigned int GetOwningThreadID() const { return m_ulThreadOwner; }
	unsigned int GetLockCount() const { return m_uiLockCount; }
#endif

protected:

	CRITICAL_SECTION m_kCriticalSection;
	unsigned int m_uiSpinCount; 

#ifdef _DEBUG
	// The ID of the last thread to lock the critical section.
	unsigned long m_ulThreadOwner;
	// The count of how many times the critical section has been locked by the same thread.
	unsigned int m_uiLockCount;
#endif 
};


//////////////////////////////////////////////////////////////////////////

// Helper class used to auto lock a synchronization primitive
template<class SyncPrimitive> class CSyncPrimitiveAutoLock : CCoreMemObject
{
public:
	CSyncPrimitiveAutoLock(SyncPrimitive& syncPrimitive) : 
	  m_SyncPrimitive(syncPrimitive) 
	{ 
		m_SyncPrimitive.Lock(); 
	}
	~CSyncPrimitiveAutoLock() 
	{ 
		m_SyncPrimitive.Unlock(); 
	}

private:
	SyncPrimitive& m_SyncPrimitive;

	// don't allow default constructor or copy constructor to be used
	CSyncPrimitiveAutoLock();
	CSyncPrimitiveAutoLock(const CSyncPrimitiveAutoLock<SyncPrimitive>&);
	CSyncPrimitiveAutoLock<SyncPrimitive>& operator = (
		const CSyncPrimitiveAutoLock<SyncPrimitive>&);
};

// typedef most commonly used auto lock classes
typedef CSyncPrimitiveAutoLock<CCriticalSection> CAutoCriticalSection;

//////////////////////////////////////////////////////////////////////////


