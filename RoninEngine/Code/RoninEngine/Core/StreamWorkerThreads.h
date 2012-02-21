#pragma once

#include "CoreMemObject.h"

#include "StreamDefines.h"

#include "Thread.h"
#include "ThreadSyncPrimitives.h"

#include "StreamReadRequest.h"

//////////////////////////////////////////////////////////////////////////

// worker threads used by the streaming engine
class CStreamWorkerThread : public CThread
{
public:

	CStreamWorkerThread(const stl_string& sName);

	void RequestShutDown();
	bool PushRequest(CStreamReadRequest* pRequest);

	//////////////////////////////////////////////////////////////////////////
	// Implementation of CThread
	//////////////////////////////////////////////////////////////////////////

	virtual size_t ThreadProcedure(void* pParameter);

	//////////////////////////////////////////////////////////////////////////

protected:

	void ProcessNewRequests();

	// process a single request
	virtual void ProcessRequest(CStreamReadRequest* pRequest) = 0;

	//////////////////////////////////////////////////////////////////////////

	volatile bool m_bRequestShutdown;

	CCriticalSection m_RequestCS;
	typedef stl_vector<CStreamReadRequestPtr> TStreamReadRequestPtrVector;
	TStreamReadRequestPtrVector m_vNewRequests;

	TStreamReadRequestPtrVector m_vOpenRequests;
};

//////////////////////////////////////////////////////////////////////////

// thread performing actual IO operation on a certain media device (Optical Disc, HDD, ...)
class CStreamIOThread : public CStreamWorkerThread
{
public:


	CStreamIOThread(EStreamMediaType eMediaType);

protected:

	virtual void ProcessRequest(CStreamReadRequest* pRequest);
};

//////////////////////////////////////////////////////////////////////////

// thread executing the async callbacks
class CStreamAsyncProcessorThread : public CStreamWorkerThread
{
public:

	CStreamAsyncProcessorThread();

protected:
	virtual void ProcessRequest(CStreamReadRequest* pRequest);
};

//////////////////////////////////////////////////////////////////////////

// thread performing the decompression
class CStreamDecompressionThread : public CStreamWorkerThread
{
public:

	CStreamDecompressionThread();

protected:
	virtual void ProcessRequest(CStreamReadRequest* pRequest);
};

//////////////////////////////////////////////////////////////////////////
