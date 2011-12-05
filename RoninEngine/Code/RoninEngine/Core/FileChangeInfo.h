#pragma once

#include "CoreMemObject.h"

struct SFileChangeInfo : public CCoreMemObject
{
	enum EType
	{
		EType_Unknown = 0,
		EType_Created,
		EType_Deleted,
		EType_Modified,
		EType_RenamedOldName,
		EType_RenamedNewName
	};

	SFileChangeInfo()
		: m_eChangeType(EType_Unknown)
	{
	}

	stl_string m_sFilename;
	EType m_eChangeType;
};

class IFileChangeListener
{
public:
	virtual ~IFileChangeListener();

	virtual void ProcessFileChange(const SFileChangeInfo& fileChangeInfo) = 0;
};