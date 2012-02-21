#pragma once

#include "CoreMemObject.h"

#include "StreamDefines.h"

class CStreamReadRequest : public CCoreRefMTObject, public IStreamReadRequest
{
public:
	CStreamReadRequest();
	~CStreamReadRequest();

	bool Init(const SStreamReadParams& readParams);
	void Reset();

	void SetStatus(EStatus eStatus) { m_eStatus = eStatus; }
	void SetAsyncCallbackCalled() { m_bAsyncCallbackCalled = true; }
	
	EStreamMediaType GetMediaType() const { return m_eMediaType; }
	bool IsCompressed() const { return m_bIsCompressed; }
	bool IsDecompressed() const { return m_bDecompressed; }
	bool IsAsyncCallbackCalled() const { return m_bAsyncCallbackCalled; }
	bool IsAborted() const { return m_bAborted; }

	uint64 GetSortPriority() const { return m_nSortPriority; }

	// perform the actual IO operation
	bool ReadData();
	// perform decompression on the data
	bool Decompress();

	//////////////////////////////////////////////////////////////////////////
	//	Implementation of IStreamReadRequest
	//////////////////////////////////////////////////////////////////////////

	virtual const SStreamReadParams& GetParams() const { return m_readParams; }
	virtual EStatus GetStatus() const { return m_eStatus; }
	virtual EError GetError() const { return m_eError; }
	virtual void* GetData() { return m_acData; }
	virtual size_t GetDataSize() const { return m_nAllocatedSize; }

	virtual bool Abort();

protected:

	volatile EStatus m_eStatus;
	volatile EError m_eError;
	volatile bool m_bAsyncCallbackCalled;
	volatile bool m_bDecompressed;
	volatile bool m_bAborted;
	SStreamReadParams m_readParams;

	uint64 m_nSortPriority;
	EStreamMediaType m_eMediaType;
	char* m_acData;
	size_t m_nAllocatedSize;
	size_t m_nReadSize;
	bool m_bIsCompressed;
	bool m_bTempAllocated;
};
CoreSmartPointer(CStreamReadRequest);