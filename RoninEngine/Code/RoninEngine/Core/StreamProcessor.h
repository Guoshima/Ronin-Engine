#pragma once

#include "CoreMemObject.h"

#include "StreamDefines.h"
#include "ThreadSyncPrimitives.h"

class CStreamReadRequest;
CoreSmartPointer(CStreamReadRequest);

// helper used to start and receive streaming requests
class CStreamProcessor : public CCoreMemObject, public IStreamProcessor
{
	friend class CStreamEngine;

public:

	// process all streaming request related tasks
	void ProcessStreamRequests();

	// start a streaming request from this stream processor
	bool StartStreamingRequest(const SStreamReadParams& readParams);

protected:

	void AddFinishedRequest(CStreamReadRequest* pReadRequest);
	virtual void ProcessFinishedRequest(CStreamReadRequest* pReadRequest);

private:

	void PushOpenRequest(CStreamReadRequest* pRequest);
	bool RemoveOpenRequest(CStreamReadRequest* pRequest);

	void PushFinishedRequest(CStreamReadRequest* pRequest);
	CStreamReadRequest* PopFinishedRequest();

	//////////////////////////////////////////////////////////////////////////

	typedef stl_list<CStreamReadRequestPtr> TStreamReadRequestPtrList;
	CCriticalSection m_OpenRequestsCS;
	TStreamReadRequestPtrList m_OpenRequests;

	CCriticalSection m_FinishedRequestsCS;
	TStreamReadRequestPtrList m_FinishedRequests;
};