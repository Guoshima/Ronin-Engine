#pragma once

#include "CoreMemObject.h"

class CThread : public CCoreMemObject
{
public:

	enum EPriorityClass
	{
		EPriotiyClass_Normal = 0,
	};

	enum EPriority
	{
		EPriority_Normal = 0,
	};

	enum EStatus
	{
		EStatus_PreInit = 0,
		EStatus_InitAndWaiting,
		EStatus_Running,
		EStatus_Suspended,
		EStatus_Completed,
		EStatus_Failed,
	};

	CThread(const stl_string& sName);
	virtual ~CThread();

	bool Initialize(bool bAutoStart = false, size_t nStackSize = 0);

	// the actual thread procedure which should be implemented 
	virtual size_t ThreadProcedure(void* pParameter) = 0;

	int Suspend();
	int Resume();
	bool WaitForCompletion(int nMaxWaitTime = -1);

	size_t GetStackSize() const { return m_nStackSize; }
	const stl_string& GetName() const { return m_sName; }
	size_t GetReturnValue() const { return m_nReturnValue; }
	EStatus GetStatus() const { return m_eStatus; }

protected:

	static DWORD WINAPI ThreadProc(void* pParameter);

	//////////////////////////////////////////////////////////////////////////

	size_t m_nStackSize;
	stl_string m_sName;

	volatile EStatus m_eStatus;
	volatile size_t m_nReturnValue;

	HANDLE m_hThread;
};
