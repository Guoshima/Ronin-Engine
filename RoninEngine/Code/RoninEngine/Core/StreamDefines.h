#pragma once

#include "CoreMemObject.h"

class IStreamProcessor;
class IStreamEngine;
class IStreamReadRequest;

// callback object on which sync and async processing can be performed
// on an active streaming requests
class IStreamCallback
{
public:
	virtual ~IStreamCallback() {}

	virtual void ProcessSyncCallback(IStreamReadRequest* pRequest) = 0;
	virtual void ProcessAsyncCallback(IStreamReadRequest* pRequest) = 0;
};

enum EStreamPriority
{
	eStreamPriority_Low = 0,
	eStreamPriority_Medium,
	eStreamPriority_High,
};

struct SStreamReadParams
{
	SStreamReadParams()
	{
		Reset();
	}

	void Reset()
	{
		m_sFilename = "";
		m_nSize = 0;
		m_nOffset = 0;
		m_ePriority = eStreamPriority_Medium;
		m_fMaxWaitTime = 0.0f;
		m_pData = 0;
		m_nMaxDataSize = 0;
		m_pRequestProcessor = 0;
		m_pReceiveProcessor = 0;
		m_pCallback = 0;
	}

	// actual file path to read
	stl_string m_sFilename;
	// size of the read operation (0 to read full file)
	size_t m_nSize;
	// offset into file (0 default)
	size_t m_nOffset;
	// priority of the read request
	EStreamPriority m_ePriority;
	// max time we want to wait for result to be available
	// (should be used only when needed, for time critical reading operations)
	float m_fMaxWaitTime;

	// the data used to store the streaming result - optional
	void* m_pData;
	// max size of the allocated data (ideally should be the same as m_nSize)
	size_t m_nMaxDataSize;

	// stream processor who requested the read request
	IStreamProcessor* m_pRequestProcessor;
	// stream processor who should receive the request
	IStreamProcessor* m_pReceiveProcessor;
	// the callback object used to process this request
	IStreamCallback* m_pCallback;
};

// stream processor which can start and manage streaming requests
class IStreamProcessor
{
public:
	virtual ~IStreamProcessor() {}

	// start a streaming request from this stream processor
	virtual bool StartStreamingRequest(const SStreamReadParams& readParams) = 0;
};

// interface to an active streaming request
class IStreamReadRequest
{
public:

	// the different steps of a streaming requests (handy for debugging)
	enum EStatus
	{
		eStatus_Invalid = 0,
		eStatus_Init,
		eStatus_WaitingForIO,
		eStatus_Reading,
		eStatus_WaitingForDecompression,
		eStatus_Decompressing,
		eStatus_WaitingForAsyncCallback,
		eStatus_ProcessingAsyncCallback,
		eStatus_WaitingForFinalization,
		eStatus_ProcessingSyncCallback,
		eStatus_Finished,
	};

	// the different possible error types on a read request
	enum EError
	{
		eError_None = 0,
		eError_Aborted,
		eError_FileNotFound,
	};

	virtual ~IStreamReadRequest() {}

	virtual const SStreamReadParams& GetParams() const = 0;
	virtual EStatus GetStatus() const = 0;
	virtual EError GetError() const = 0;
	virtual void* GetData() = 0;
	virtual size_t GetDataSize() const = 0;


	virtual bool Abort() = 0;
};

typedef stl_vector<IStreamReadRequest*> TStreamReadRequestVector;
typedef stl_list<IStreamReadRequest*> TStreamReadRequestList;

enum EStreamMediaType
{
	eStreamMediaType_HDD = 0,
	eStreamMediaType_Memory,
	eStreamMediaType_Optical,
	eStreamMediaType_Max,
};

class IStreamEngine
{
public:

	virtual ~IStreamEngine() {}


};
