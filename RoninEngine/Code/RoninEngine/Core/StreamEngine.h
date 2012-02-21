#pragma once

#include "CoreMemObject.h"

#include "StreamDefines.h"

class CStreamReadRequest;
class CStreamIOThread;
class CStreamAsyncProcessorThread;
class CStreamDecompressionThread;

class CStreamEngine : public IStreamEngine, CCoreMemObject
{
	// only core engine can create or destroy the streaming engine
	friend class CCoreEngine;

	// only stream processor should be able to start reading requests
	friend class CStreamProcessor;

	// the worker threads are allowed to push requests through the streaming
	// engine to the other threads
	friend class CStreamWorkerThread;

public:


private:

	// called from the core engine
	CStreamEngine();
	~CStreamEngine();
	bool Initialize();
	bool Shutdown();

	// called from the stream processors
	CStreamReadRequest* StartRead(const SStreamReadParams& readParams);

	// called from the worker threads
	bool ProcessNextRequestStep(CStreamReadRequest* pReadRequest);

	//////////////////////////////////////////////////////////////////////////

	typedef stl_map<EStreamMediaType, CStreamIOThread*> TIOThreadMap;
	typedef stl_vector<CStreamAsyncProcessorThread*> TAsyncProcessorVector;
	typedef stl_vector<CStreamDecompressionThread*> TDecompressionThreadVector;

	TIOThreadMap m_IOThreads;
	TAsyncProcessorVector m_AsyncProcessors;
	TDecompressionThreadVector m_Decompressors;
};