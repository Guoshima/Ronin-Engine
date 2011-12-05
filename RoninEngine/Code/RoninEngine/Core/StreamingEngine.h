#pragma once

#include "CoreMemObject.h"

class CStreamReadRequest : public CCoreMemObject
{
public:

protected:
};

#include "Thread.h"

// thread performing actual IO operation on a certain media device (Optical Disc, HDD, ...)
class StreamIOThread : public CThread
{
public:

	//////////////////////////////////////////////////////////////////////////
	// Implementation of CThread
	//////////////////////////////////////////////////////////////////////////

	virtual size_t ThreadProcedure(void* pParameter);

	//////////////////////////////////////////////////////////////////////////

protected:


};

class CStreamingEngine : public CCoreMemObject
{
	// only core engine can create or destroy the streaming engine
	friend class CCoreEngine;

public:

	enum EPriority
	{
		EPriority_Low = 0,
		EPriority_Medium,
		EPriority_High,
	};

	struct SReadParams
	{
		SReadParams()
		{
		}

		// actual file path to read
		stl_string m_sFilename;
		// size of the read operation (0 to read full file)
		size_t m_nSize;
		// offset into file (0 default)
		size_t m_nOffset;
		// priority of the read request
		EPriority m_ePriority;
		// max time we want to wait for result to be available
		// (should be used only when needed, for time critical reading operations)
		float m_fMaxWaitTime;
	};

	CStreamReadRequest* StartRead(const SReadParams& readParams);

private:

	CStreamingEngine();
	~CStreamingEngine();

	bool Initialize();
	bool Shutdown();

	//////////////////////////////////////////////////////////////////////////

};