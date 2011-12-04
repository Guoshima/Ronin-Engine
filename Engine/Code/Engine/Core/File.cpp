#include "CorePCH.h"

#include "File.h"

//////////////////////////////////////////////////////////////////////////

const char* CFile::GetOpenMode(EOpenMode eOpenMode)
{
	switch (eOpenMode)
	{
	case OM_Read:
		return "rb";
	case OM_Write:
		return "wb";
	case OM_Append:
		return "ab";
	}

	// unknown open mode
	assert(0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////

CFile::CFile() : m_eOpenMode(OM_Invalid), m_pFile(0), m_nSize(INVALID_SIZE)
{

}

CFile::~CFile()
{
	CloseFile();
}

//////////////////////////////////////////////////////////////////////////

bool CFile::OpenFile(const stl_string& sPath, EOpenMode eOpenMode)
{
	m_eOpenMode = eOpenMode;
	m_sPath = sPath;

	m_pFile = fopen(sPath.c_str(), GetOpenMode(eOpenMode));

	return (m_pFile != NULL);
}

bool CFile::CloseFile()
{
	if (m_pFile)
		fclose(m_pFile);
	m_pFile = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////////

size_t CFile::GetSize() const
{
	int iCurrent = ftell(m_pFile);
	if (iCurrent < 0)
		return 0;
	fseek(m_pFile, 0, SEEK_END);
	size_t nSize = ftell(m_pFile);
	fseek(m_pFile, iCurrent, SEEK_SET);
	if (nSize < 0)
		return 0;
	return nSize;
}

size_t CFile::ReadData(char* acData, size_t stSize)
{
	if (m_pFile)
		return fread(acData, 1, stSize, m_pFile);

	return 0;
}

size_t CFile::WriteData(const char* acData, size_t stSize)
{
	// not implemented yet
	assert(0);
	return 0;
}

