#include "CorePCH.h"

#include "FileSystem.h"

#include "FileChangeMonitor.h"

//////////////////////////////////////////////////////////////////////////

CFileSystem::CFileSystem()
{
	m_pFileChangeMonitor = IntNew CFileChangeMonitor;
}

CFileSystem::~CFileSystem()
{
	assert(m_vOpenFiles.size() == 0);
	assert(m_vOpenFindFileResults.size() == 0);

	IntDelete m_pFileChangeMonitor;
}

//////////////////////////////////////////////////////////////////////////

CFile* CFileSystem::OpenFile(const stl_string& sFilePath, CFile::EOpenMode eOpenMode)
{
	CFile* pFile = IntNew CFile;
	if (!pFile->OpenFile(Convert(sFilePath), eOpenMode))
	{
		IntDelete pFile;
		return 0;
	}

	m_vOpenFiles.push_back(pFile);
	return pFile;
}

bool CFileSystem::CloseFile(CFile* pFile)
{
	// make sure handle is valid
	TFileList::iterator findRes = stl_find(
		m_vOpenFiles.begin(), m_vOpenFiles.end(), pFile);
	assert(findRes != m_vOpenFiles.end());
	if (findRes == m_vOpenFiles.end())
		return false;

	m_vOpenFiles.erase(findRes);

	IntDelete pFile;
	return true;
}

//////////////////////////////////////////////////////////////////////////

stl_string CFileSystem::Convert(const stl_string& sFilePath)
{
	return GetDataPath() + sFilePath;
}

bool CFileSystem::FileExcists() const
{
	// use fast windows method here, instead of opening file
	return true;
}

//////////////////////////////////////////////////////////////////////////

CFileSystem::TFindDataHandle CFileSystem::FindFirstFile(
	const stl_string& sPath, SFindData& findData)
{
	TFindDataHandle eResult = INVALID_FILE_HANDLE;

	WIN32_FIND_DATA kFindFileData;
	HANDLE hFind = ::FindFirstFile(Convert(sPath).c_str(), &kFindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		findData.m_sPath = kFindFileData.cFileName;
		findData.m_nSize = kFindFileData.nFileSizeHigh;

		eResult = (TFindDataHandle) hFind;

		m_vOpenFindFileResults.push_back(eResult);
	} else {
		::FindClose(hFind);
	}

	return eResult;
}

bool CFileSystem::FindNextFile(
	TFindDataHandle eHandle, SFindData& findData)
{
	// make sure handle is valid
	TFindDataList::iterator findRes = stl_find(
		m_vOpenFindFileResults.begin(), m_vOpenFindFileResults.end(), eHandle);
	assert(findRes != m_vOpenFindFileResults.end());
	if (findRes == m_vOpenFindFileResults.end())
		return false;

	// perform actual find in the windows file system
	WIN32_FIND_DATA kFindFileData;
	HANDLE hFind = (HANDLE) eHandle;
	if (::FindNextFile(hFind, &kFindFileData) == TRUE)
	{
		findData.m_sPath = kFindFileData.cFileName;
		findData.m_nSize = kFindFileData.nFileSizeHigh;

		return true;
	}

	return false;
}

bool CFileSystem::FindClose(TFindDataHandle eHandle)
{
	// make sure handle is valid
	TFindDataList::iterator findRes = stl_find(
		m_vOpenFindFileResults.begin(), m_vOpenFindFileResults.end(), eHandle);
	assert(findRes != m_vOpenFindFileResults.end());
	if (findRes == m_vOpenFindFileResults.end())
		return false;

	m_vOpenFindFileResults.erase(findRes);

	HANDLE hFind = (HANDLE) eHandle;
	return (::FindClose(hFind) == TRUE);
}

//////////////////////////////////////////////////////////////////////////