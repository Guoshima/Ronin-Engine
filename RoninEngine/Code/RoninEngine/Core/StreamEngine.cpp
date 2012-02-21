#include "CorePCH.h"

#include "StreamEngine.h"

#include "StreamWorkerThreads.h"
#include "StreamProcessor.h"
#include "StreamReadRequest.h"

//////////////////////////////////////////////////////////////////////////

CStreamEngine::CStreamEngine()
{

}

CStreamEngine::~CStreamEngine()
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////

bool CStreamEngine::Initialize()
{
	// start the worker threads
	m_IOThreads[eStreamMediaType_HDD] = IntNew CStreamIOThread(
		eStreamMediaType_HDD);
	m_Decompressors.push_back(IntNew CStreamDecompressionThread());
	m_AsyncProcessors.push_back(IntNew CStreamAsyncProcessorThread());

	return true;
}

bool CStreamEngine::Shutdown()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////

CStreamReadRequest* CStreamEngine::StartRead(
	const SStreamReadParams& readParams)
{
	CStreamReadRequest* pRequest = IntNew CStreamReadRequest();
	if (pRequest->Init(readParams))
	{
		IntDelete pRequest;
		return 0;
	}

	TIOThreadMap::iterator it = m_IOThreads.find(pRequest->GetMediaType());
	if (it == m_IOThreads.end())
	{
		IntDelete pRequest;
		return 0;
	}

	CStreamIOThread* pIOThread = it->second;
	pIOThread->PushRequest(pRequest);

	return pRequest;
}

//////////////////////////////////////////////////////////////////////////

bool CStreamEngine::ProcessNextRequestStep(
	CStreamReadRequest* pReadRequest)
{
	// has there been an error ?
	if (pReadRequest->GetError() > 0)
	{
		CStreamProcessor* pReceiver = (CStreamProcessor*)
			pReadRequest->GetParams().m_pReceiveProcessor;
		pReceiver->AddFinishedRequest(pReadRequest);
		return false;
	}

	// is the data still compressed ?
	if (pReadRequest->IsCompressed() && 
		!pReadRequest->IsDecompressed())
	{
		pReadRequest->SetStatus(IStreamReadRequest::eStatus_WaitingForDecompression);
		return m_Decompressors[0]->PushRequest(pReadRequest);
	}

	// has the async callback been called already ?
	if (pReadRequest->GetParams().m_pCallback && 
		!pReadRequest->IsAsyncCallbackCalled())
	{
		pReadRequest->SetStatus(IStreamReadRequest::eStatus_WaitingForAsyncCallback);
		return m_AsyncProcessors[0]->PushRequest(pReadRequest);
	}

	// we are done with the request, push to final processor
	pReadRequest->SetStatus(IStreamReadRequest::eStatus_WaitingForFinalization);
	CStreamProcessor* pReceiver = (CStreamProcessor*)
		pReadRequest->GetParams().m_pReceiveProcessor;
	pReceiver->AddFinishedRequest(pReadRequest);

	return true;
}

//////////////////////////////////////////////////////////////////////////


