#include "CorePCH.h"

#include "StreamProcessor.h"

#include "StreamEngine.h"
#include "StreamReadRequest.h"

//////////////////////////////////////////////////////////////////////////

void CStreamProcessor::ProcessStreamRequests()
{
	CStreamReadRequest* pRequest = PopFinishedRequest();
	while (pRequest)
	{
		ProcessFinishedRequest(pRequest);
		pRequest = PopFinishedRequest();
	}	
}

void CStreamProcessor::ProcessFinishedRequest(
	CStreamReadRequest* pReadRequest)
{
	// process the sync callback if there is any
	if (pReadRequest->GetParams().m_pCallback)
		pReadRequest->GetParams().m_pCallback->ProcessSyncCallback(pReadRequest);
	
	// remove from the open request list
	bool bRes = RemoveOpenRequest(pReadRequest);
	assert(bRes);
}

bool CStreamProcessor::StartStreamingRequest(
	const SStreamReadParams& _readParams)
{
	SStreamReadParams readParams(_readParams);
	readParams.m_pRequestProcessor = this;

	// make sure a valid receive processor is set!
	if (readParams.m_pReceiveProcessor == NULL)
		readParams.m_pReceiveProcessor = this;

	// start the actual read operation
	CStreamReadRequest* pRequest = STREAM_ENGINE->StartRead(readParams);
	if (pRequest)
	{
		// push request to open request on the receive processor
		CStreamProcessor* pProcessor = (CStreamProcessor*)
			pRequest->GetParams().m_pReceiveProcessor;
		pProcessor->PushOpenRequest(pRequest);

		return true;
	}

	return false;
}

void CStreamProcessor::AddFinishedRequest(
	CStreamReadRequest* pReadRequest)
{
	// simply push into the streaming request list, will be processed
	// on the desired thread during ProcessStreamRequests
	PushFinishedRequest(pReadRequest);
}

//////////////////////////////////////////////////////////////////////////

void CStreamProcessor::PushOpenRequest(CStreamReadRequest* pRequest)
{
	CAutoCriticalSection autoCS(m_OpenRequestsCS);
	m_OpenRequests.push_back(pRequest);
}

bool CStreamProcessor::RemoveOpenRequest(CStreamReadRequest* pRequest)
{
	CAutoCriticalSection autoCS(m_OpenRequestsCS);
	if (m_OpenRequests.size() > 0)
	{
		TStreamReadRequestPtrList::iterator itFindRes =
			stl_find(m_OpenRequests.begin(), m_OpenRequests.end(), pRequest);
		if (itFindRes == m_OpenRequests.end())
			return false;

		m_OpenRequests.erase(itFindRes);
		return true;
	}

	return 0;
}

void CStreamProcessor::PushFinishedRequest(CStreamReadRequest* pRequest)
{
	CAutoCriticalSection autoCS(m_FinishedRequestsCS);
	m_FinishedRequests.push_back(pRequest);
}

CStreamReadRequest* CStreamProcessor::PopFinishedRequest()
{
	CAutoCriticalSection autoCS(m_FinishedRequestsCS);
	if (m_FinishedRequests.size() > 0)
	{
		CStreamReadRequest* pResult = m_FinishedRequests.front();
		m_FinishedRequests.pop_front();
		return pResult;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////


