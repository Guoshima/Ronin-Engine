#include "CorePCH.h"

#include "FileSystem.h"

#include "FileChangeMonitor.h"

#include <direct.h>
#include <sys/types.h> 
#include <sys/stat.h> 

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

const char CFileSystem::ms_cPathDelimiterChar = '\\';
const stl_string CFileSystem::ms_sPathDelimiterString = "\\";
const CFileSystem::TFindDataHandle CFileSystem::ms_nInvalidFileHandle = -1;

//////////////////////////////////////////////////////////////////////////

bool CFileSystem::GetCurrentWorkingDirectory(char* pcPath, size_t nDestSize)
{
	return (_getcwd(pcPath, nDestSize) != NULL);
}

bool CFileSystem::IsRelative(const char *pcPath)
{
	// An absolute path is one that begins with either of the following:
	// [1] a forward or backward slash
	// [2] A drive letter followed by ":"

	unsigned int uiLen = strlen(pcPath);
	if (uiLen < 2)
	{
		// the smallest absolute path is slash followed by letter, so
		// this must be a relative path.
		return true;
	}

	// check for case 1
	char c1stChar = pcPath[0];   
	if (c1stChar == '\\' || c1stChar == '/')
	{
		// test for case 1 indicates this is an absolute path
		return false;
	}

	// check for case 2
	c1stChar = toupper(c1stChar);
	char c2ndChar = pcPath[1];
	if (c2ndChar == ':' && (c1stChar >= 'A' && c1stChar <= 'Z'))
	{   
		// test for case 2 indicates this is an absolute path
		return false;
	}

	return true;
}

size_t CFileSystem::ConvertToAbsolute(char* pcPath, size_t stPathBytes)
{
	char acCWD[_MAX_PATH];
	if (!GetCurrentWorkingDirectory(acCWD, _MAX_PATH))
		return 0;
	return ConvertToAbsolute(pcPath, stPathBytes, acCWD);
}

size_t CFileSystem::ConvertToAbsolute(char* pcPath, size_t stPathBytes,
								 const char* pcRelativeToHere)
{
	// This ConvertToAbsolute modifies pcPath in place. 
	const size_t stMaximumPathBufferSize = _MAX_PATH * 2 + 2;
	char acAbsPath[stMaximumPathBufferSize];
	size_t uiAbsBytes = ConvertToAbsolute(acAbsPath, stMaximumPathBufferSize, 
		pcPath, pcRelativeToHere);

	if (uiAbsBytes < stPathBytes)
	{
		strcpy_s(pcPath, stPathBytes, acAbsPath);
		return uiAbsBytes;
	}
	else
	{
		pcPath[0] = NULL;
		return 0;
	}

}

size_t CFileSystem::ConvertToAbsolute(
	char* pcAbsolutePath, size_t stAbsBytes,
	const char* pcRelativePath, const char* pcRelativeToHere)
{
	// This function takes pcRelativePath and converts it to an absolute path 
	// by concatenating it with pcRelativeToHere and removing dotdots.

	// The resulting absolute path is stored in pcAbsolutePath, which is 
	// assumed to have been allocated with size stAbsBytes. 
	// The function returns the number of bytes written.

	assert(pcAbsolutePath && pcRelativePath);
	//assert(IsRelative(pcRelativePath));
	assert(pcAbsolutePath != pcRelativePath); // destination cannot be source

	// If pcRelativeToHere is null or an empty string, use the current working
	// directory.
	if (!pcRelativeToHere)
	{
		pcRelativeToHere = "";
	}
	size_t stLenRelativeToHere = strlen(pcRelativeToHere);
	if (stLenRelativeToHere == 0)
	{
		char acCWD[_MAX_PATH];
		if (!GetCurrentWorkingDirectory(acCWD, _MAX_PATH))
		{
			if (stAbsBytes > 0)
				pcAbsolutePath[0] = NULL;
			return 0;
		}
		assert(strlen(acCWD) != 0);
		return ConvertToAbsolute(pcAbsolutePath, stAbsBytes, pcRelativePath,
			acCWD);
	}

	size_t stLenRelativePath = strlen(pcRelativePath);

	// Concatenate a delimiter if necessary
	bool bInsertDelimiter = 
		(pcRelativeToHere[stLenRelativeToHere-1] != ms_cPathDelimiterChar);

	size_t stRequiredSize = 1 // null termination
		+ stLenRelativeToHere 
		+ stLenRelativePath
		+ ((bInsertDelimiter) ? 1 : 0);

	// Fail if not enough storage
	if (stAbsBytes < stRequiredSize)
	{
		if (stAbsBytes > 0)
			pcAbsolutePath[0] = NULL;
		return 0;
	}

	// Store pcRelativeToHere into pcAbsolutePath
	strcpy_s(pcAbsolutePath, stAbsBytes, pcRelativeToHere);

	// Concatenate a delimiter if necessary
	if (bInsertDelimiter)
		strcat_s(pcAbsolutePath, stAbsBytes, ms_sPathDelimiterString.c_str());

	// Concatenate pcRelativePath
	strcat_s(pcAbsolutePath, stAbsBytes, pcRelativePath);

	//RemoveDotDots(pcAbsolutePath);
	return strlen(pcAbsolutePath);
}

char* CFileSystem::RemoveBackSlash(char* pPath)
{
	size_t nLen = strlen(pPath);
	if (nLen == 0)
		return pPath;

	bool bEndWithSlash = (pPath[nLen-1] == ms_cPathDelimiterChar);
	if (bEndWithSlash)
		pPath[nLen - 1] = 0;

	return pPath;
}

stl_string CFileSystem::GetExtension(const stl_string& sPath)
{
	stl_string result("");

	size_t pos = sPath.find_last_of('.');
	if (pos == stl_string::npos)
		return result;

	result = sPath.substr(pos + 1);

	return result;
}

//////////////////////////////////////////////////////////////////////////

bool CFileSystem::IsRealDirectory(const char* sFileName)
{
	struct __stat64 my_stat;

	if (_stat64(sFileName, &my_stat) != 0)
	{
		return false;
	}

	return ((my_stat.st_mode & S_IFDIR) != 0);
}

bool CFileSystem::IsRealFile(const char* sFileName)
{
	struct __stat64 my_stat;

	if (_stat64(sFileName, &my_stat) != 0)
	{	
		return false;
	}

	return ((my_stat.st_mode & S_IFDIR) == 0);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CFileSystem::CFileSystem()
{
	m_pFileChangeMonitor = IntNew CFileChangeMonitor;
}

CFileSystem::~CFileSystem()
{
	assert(m_vOpenFiles.size() == 0);
	assert(m_vOpenFindFileResults.size() == 0);
	assert(m_FileChangeListeners.size() == 0);

	IntDelete m_pFileChangeMonitor;
}

//////////////////////////////////////////////////////////////////////////

void CFileSystem::SetDataPath(const stl_string& sDataPath)
{
	m_sDataPath = sDataPath;

	if (m_pFileChangeMonitor)
	{
		char acAbsPath[_MAX_PATH];
		strcpy_s(acAbsPath, _MAX_PATH, sDataPath.c_str());
		size_t nLen = ConvertToAbsolute(acAbsPath, _MAX_PATH);

		m_pFileChangeMonitor->MonitorItem(RemoveBackSlash(acAbsPath));
	}
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

bool CFileSystem::FileExcists(const char* pPath) const
{
	// first check the virtual file system if available

	// chech the real OS file system
	if (IsRealFile(pPath) || IsRealDirectory(pPath)) 
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////

CFileSystem::TFindDataHandle CFileSystem::FindFirstFile(
	const stl_string& sPath, SFindData& findData)
{
	TFindDataHandle eResult = ms_nInvalidFileHandle;

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

void CFileSystem::OnSystemEvent(ESystemEvent eEvent)
{
	/*
	if (eEvent == ESystemEvent_FocusChange)
	{
		// disable direct processing of file change event
		if (m_pFileChangeMonitor)
		{
			bool bHasFocus = CCoreEngine::Get()->HasFocus();
			
			char acTmp[64];
			sprintf_s(acTmp, 64, "Activate app: %d \n", bHasFocus);
			OutputDebugString(acTmp);
		}
	}
	*/
}

//////////////////////////////////////////////////////////////////////////


void CFileSystem::RegisterFileExtensionChangeListener(
	IFileChangeListener* pFileChangeListener, 
	const TStringVector& fileTypes)
{
	TFileChangeListeners::iterator it = m_FileChangeListeners.find(pFileChangeListener);
	if (it == m_FileChangeListeners.end())
	{
		m_FileChangeListeners[pFileChangeListener] = SFileChangeListenerData();
		it = m_FileChangeListeners.find(pFileChangeListener);
	}

	SFileChangeListenerData& entry = it->second;
	for (size_t i = 0; i < fileTypes.size(); ++i)
	{
		entry.m_FileExtensions.insert(fileTypes[i]);
	}
}

void CFileSystem::RegisterFolderChangeListener(
	IFileChangeListener* pFileChangeListener,
	const TStringVector& folders)
{
	TFileChangeListeners::iterator it = m_FileChangeListeners.find(pFileChangeListener);
	if (it == m_FileChangeListeners.end())
	{
		m_FileChangeListeners[pFileChangeListener] = SFileChangeListenerData();
		it = m_FileChangeListeners.find(pFileChangeListener);
	}

	SFileChangeListenerData& entry = it->second;
	for (size_t i = 0; i < folders.size(); ++i)
	{
		entry.m_Folders.insert(folders[i]);
	}
}

void CFileSystem::RegisterFileChangeListener(
	IFileChangeListener* pFileChangeListener,
	const TStringVector& files)
{
	TFileChangeListeners::iterator it = m_FileChangeListeners.find(pFileChangeListener);
	if (it == m_FileChangeListeners.end())
	{
		m_FileChangeListeners[pFileChangeListener] = SFileChangeListenerData();
		it = m_FileChangeListeners.find(pFileChangeListener);
	}

	SFileChangeListenerData& entry = it->second;
	for (size_t i = 0; i < files.size(); ++i)
	{
		entry.m_Files.insert(files[i]);
	}
}

bool CFileSystem::UnregisterFileChangeListener(IFileChangeListener* pFileChangeListener)
{
	TFileChangeListeners::iterator it = m_FileChangeListeners.find(pFileChangeListener);
	if (it == m_FileChangeListeners.end())
		return false;

	m_FileChangeListeners.erase(it);

	return true;
}

void CFileSystem::CheckModifiedFiles()
{
	if (m_pFileChangeMonitor == NULL || !m_pFileChangeMonitor->HaveModifiedFiles())
		return;

	SFileChangeInfo info;
	while (m_pFileChangeMonitor->PopNextFileChange(info))
	{
		ProcessFileChange(info);
	}
}

void CFileSystem::ProcessFileChange(const SFileChangeInfo& fileChangeInfo)
{
	for (TFileChangeListeners::iterator it = m_FileChangeListeners.begin();
		it != m_FileChangeListeners.end(); ++it)
	{
		IFileChangeListener* pListeners = it->first;
		SFileChangeListenerData& entryData = it->second;

		// check file extensions
		stl_string sExtension = GetExtension(fileChangeInfo.m_sFilename);
		for (TStringSet::iterator it = entryData.m_FileExtensions.begin(); 
			it != entryData.m_FileExtensions.end(); ++it)
		{
			if (sExtension == *it)
				pListeners->ProcessFileChange(fileChangeInfo);
		}
	}
}

//////////////////////////////////////////////////////////////////////////