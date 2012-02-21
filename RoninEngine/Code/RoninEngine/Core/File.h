#pragma once

#include "CoreMemObject.h"

#define INVALID_SIZE -1

class CFile : public CCoreMemObject
{
	friend class CFileSystem;

public:

	enum EOpenMode
	{
		eOpenMode_Invalid,
		eOpenMode_Read,
		eOpenMode_Write,
		eOpenMode_Append,
	};
		
	EOpenMode GetOpenMode() const { return m_eOpenMode; }
	const stl_string& GetPath() const { return m_sPath; }
	size_t GetSize() const;

	size_t ReadData(char* acData, size_t stSize);
	size_t WriteData(const char* acData, size_t stSize);

protected:

	CFile();
	~CFile();

	bool OpenFile(const stl_string& sPath, EOpenMode eOpenMode);
	bool CloseFile();

private:

	static const char* GetOpenMode(EOpenMode eOpenMode);

	//////////////////////////////////////////////////////////////////////////

	EOpenMode m_eOpenMode;
	stl_string m_sPath;

	FILE* m_pFile;
	int m_nSize;
};