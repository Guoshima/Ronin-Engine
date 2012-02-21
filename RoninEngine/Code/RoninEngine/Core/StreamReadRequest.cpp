#include "CorePCH.h"

#include "StreamReadRequest.h"

#include "FileSystem.h"

//////////////////////////////////////////////////////////////////////////

CStreamReadRequest::CStreamReadRequest()
{
	Reset();
}

CStreamReadRequest::~CStreamReadRequest()
{
	Reset();
}

//////////////////////////////////////////////////////////////////////////

bool CStreamReadRequest::Init(const SStreamReadParams& readParams)
{
	m_readParams = readParams;

	// only support HDD media type for now
	m_eMediaType = eStreamMediaType_HDD;
	// no support for compressed files for now
	m_bIsCompressed = false;

	// simply use the global priority as sorting priority for now
	m_nSortPriority = readParams.m_ePriority << 32;
	
	m_eStatus = eStatus_Init;
	return true;
}

void CStreamReadRequest::Reset()
{
	m_readParams.Reset();
	m_nSortPriority = 0;
	m_eMediaType = eStreamMediaType_Max;
	m_bIsCompressed = false;
	m_eStatus = eStatus_Invalid;
	m_eError = eError_None;
	if (m_acData && m_bTempAllocated)
		IntFree(m_acData);
	m_acData = 0;
	m_bTempAllocated = false;
	m_bAsyncCallbackCalled = false;
	m_bDecompressed = false;
	m_bAborted = false;
	m_nAllocatedSize = 0;
	m_nReadSize = 0;
}

//////////////////////////////////////////////////////////////////////////

bool CStreamReadRequest::ReadData()
{
	if (m_bAborted)
	{
		m_eError = eError_Aborted;
		return true;
	}

	CFileSystem* pFileSystem = FILE_SYSTEM;
	
	CFile* pFile = pFileSystem->OpenFile(
		m_readParams.m_sFilename, CFile::eOpenMode_Read);
	if (pFile == 0)
	{
		m_eError = eError_FileNotFound;
		return false;
	}

	// retrieve total file size of the file
	size_t nFileSize = pFile->GetSize();

	// compute size to read taking care of the input parameters
	size_t nRequestReadSize = nFileSize;
	if (m_readParams.m_nSize > 0)
	{
		nRequestReadSize = m_readParams.m_nSize;
	}
	nRequestReadSize -= m_readParams.m_nOffset;
	assert(nRequestReadSize > 0 && nRequestReadSize <= nFileSize);
	if (nRequestReadSize <= 0 || nRequestReadSize > nFileSize)
	{
		pFileSystem->CloseFile(pFile);
		return false;
	}

	// has there been data provided, and if so make sure it's big enough
	m_acData = 0;
	if (m_readParams.m_pData)
	{
		assert(m_readParams.m_nMaxDataSize >= nRequestReadSize);
		if (m_readParams.m_nMaxDataSize >= nRequestReadSize)
		{
			m_acData = (char*)m_readParams.m_pData;
			m_bTempAllocated = false;
		}
	} else {
		m_acData = IntAlloc(char, nRequestReadSize);
		m_bTempAllocated = true;
	}
	assert(m_acData);
	if (m_acData == 0)
	{
		pFileSystem->CloseFile(pFile);
		return false;
	}

	m_nReadSize = pFile->ReadData(m_acData, nRequestReadSize);
	assert(m_nReadSize == nRequestReadSize);
	
	return true;
}

bool CStreamReadRequest::Decompress()
{
	if (m_bAborted)
	{
		m_eError = eError_Aborted;
		return true;
	}

	// no compression supported yet
	m_bDecompressed = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CStreamReadRequest::Abort()
{
	// forward abort to the streaming engine itself
	//CStreamEngine* pStreamEngine = (CStreamEngine*) STREAM_ENGINE;
	//return pStreamEngine->Abort(this);
	m_bAborted = true;
	// increase the streaming priority to max, so it will be processed
	// first
	m_nSortPriority = -1;

	return true;
}

//////////////////////////////////////////////////////////////////////////
