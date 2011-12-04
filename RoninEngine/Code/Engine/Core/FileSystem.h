#pragma once

#include "CoreMemObject.h"

#include "CoreEngine.h"

#include "File.h"
#include "FileChangeInfo.h"

class CFileChangeMonitor;

class CFileSystem : public CCoreMemObject, ISystemEventListener
{
	// only core engine can create or destroy the filesystem
	friend class CCoreEngine;

public:

	//////////////////////////////////////////////////////////////////////////
	// Static helper functions
	//////////////////////////////////////////////////////////////////////////

	static const char ms_cPathDelimiterChar;
	static const stl_string ms_sPathDelimiterString;

	static bool GetCurrentWorkingDirectory(char* pcPath, size_t nDestSize);

	static bool IsRelative(const char *pcPath);
	static bool IsUniqueAbsolute(const char *pcPath);

	static size_t ConvertToAbsolute(char* pcPath, size_t stBytes);
	static size_t ConvertToAbsolute(char* pcPath, size_t stBytes, 
		const char* pcRelativeToHere);
	static size_t ConvertToAbsolute(char* pcAbsolutePath, size_t stBytes, 
		const char* pcRelativePath, const char* pcRelativeToHere);

	// check the OS file system only in these functions, no virtual files 
	static bool IsRealDirectory(const char* pFilename);
	static bool IsRealFile(const char* pFilename);

	static char* RemoveBackSlash(char* pPath);
	static stl_string GetExtension(const stl_string& sPath);

	//////////////////////////////////////////////////////////////////////////
	// Implementation of ISystemEventListeren
	//////////////////////////////////////////////////////////////////////////

	virtual void OnSystemEvent(ESystemEvent eEvent);

	//////////////////////////////////////////////////////////////////////////

	void SetDataPath(const stl_string& sDataPath);
	const stl_string& GetDataPath() const { return m_sDataPath; }

	//////////////////////////////////////////////////////////////////////////

	stl_string Convert(const stl_string& sFilePath);
	bool FileExcists(const char* pPath) const;

	CFile* OpenFile(const stl_string& sFilePath, CFile::EOpenMode eOpenMode);
	bool CloseFile(CFile* pFile);

	//////////////////////////////////////////////////////////////////////////

	struct SFindData 
	{
		stl_string m_sPath;
		size_t m_nSize;
	};
	typedef size_t TFindDataHandle;

	const static TFindDataHandle ms_nInvalidFileHandle;

	TFindDataHandle FindFirstFile(const stl_string& sPath, SFindData& findData);
	bool FindNextFile(TFindDataHandle eHandle, SFindData& findData);
	bool FindClose(TFindDataHandle eHandle);

	//////////////////////////////////////////////////////////////////////////

	void CheckModifiedFiles();

	void RegisterFileExtensionChangeListener(IFileChangeListener* pFileChangeListener,
		const TStringVector& fileTypes);
	void RegisterFolderChangeListener(IFileChangeListener* pFileChangeListener,
		const TStringVector& folders);
	void RegisterFileChangeListener(IFileChangeListener* pFileChangeListener,
		const TStringVector& files);
	bool UnregisterFileChangeListener(IFileChangeListener* pFileChangeListener);

private:

	CFileSystem();
	~CFileSystem();

	void ProcessFileChange(const SFileChangeInfo& fileChangeInfo);

	//////////////////////////////////////////////////////////////////////////

	// the default data path
	stl_string m_sDataPath;

	CFileChangeMonitor* m_pFileChangeMonitor;

	// active open find results
	typedef stl_vector<TFindDataHandle> TFindDataList;
	TFindDataList m_vOpenFindFileResults;
	typedef stl_vector<CFile*> TFileList;
	TFileList m_vOpenFiles;

	//////////////////////////////////////////////////////////////////////////

	struct SFileChangeListenerData
	{
		TStringSet m_Files;
		TStringSet m_FileExtensions;
		TStringSet m_Folders;
	};
	// use map because iterator speed isn't crucial and easier for lookup code 
	typedef stl_map<IFileChangeListener*, SFileChangeListenerData> TFileChangeListeners;

	TFileChangeListeners m_FileChangeListeners;


};