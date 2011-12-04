#include "CorePCH.h"

#include "Thread.h"

//////////////////////////////////////////////////////////////////////////

CThread::CThread(const stl_string& sName) : 
	m_sName(sName), m_nStackSize(0), 
	m_nReturnValue(0), m_eStatus(EStatus_PreInit)
{
}

CThread::~CThread()
{
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI CThread::ThreadProc(void* pParameter)
{
	CThread* pThread = (CThread*)pParameter;
	
	// run the pure virtual thread function
	pThread->m_nReturnValue = pThread->ThreadProcedure(pThread);
	
	// mark the status as completed (not 100% thread safe, because
	// this value could be modified by the thread managing the 
	// thread object itself as well)
	pThread->m_eStatus = CThread::EStatus_Completed;

	return pThread->m_nReturnValue;
}

bool CThread::Initialize(bool bAutoStart, size_t nStackSize)
{
	m_nStackSize = nStackSize;

	m_hThread = CreateThread(NULL, m_nStackSize, 
		ThreadProc, this, CREATE_SUSPENDED, NULL);
	if (m_hThread == 0)
	{
		m_eStatus = EStatus_Failed;
		return false;
	}

	m_eStatus = EStatus_InitAndWaiting;

	if (bAutoStart)
		Resume();

	return true;
}

//////////////////////////////////////////////////////////////////////////

int CThread::Suspend()
{
	if (m_hThread == 0)
		return -1;

	int returnValue = SuspendThread(m_hThread);
	if (returnValue != -1)
		m_eStatus = EStatus_Suspended;

	return returnValue;
}

int CThread::Resume()
{
	if (m_hThread == 0)
		return -1;

	int returnValue = ResumeThread(m_hThread);
	if (returnValue != -1)
		m_eStatus = EStatus_Running;
	
	return returnValue;
}

bool CThread::WaitForCompletion(int nMaxWaitTime)
{
	// don't wait for completion if we are not currently running!
	if (m_eStatus != EStatus_Running)
		return false;

	DWORD dwWaitTime = INFINITE;
	if (dwWaitTime >= 0)
		dwWaitTime = nMaxWaitTime;

	WaitForSingleObject(m_hThread, dwWaitTime);

	return true;
}

//////////////////////////////////////////////////////////////////////////