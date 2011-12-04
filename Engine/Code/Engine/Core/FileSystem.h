#pragma once

#include "CoreMemObject.h"

#include "File.h"

class CFileChangeMonitor;

class CFileSystem : public CCoreMemObject
{

	// only core engine can create or destroy the filesystem
	friend class CCoreEngine;

public:

	struct SFindData 
	{
		stl_string m_sPath;
		size_t m_nSize;
	};
	typedef size_t TFindDataHandle;
	#define INVALID_FILE_HANDLE ((CFileSystem::TFindDataHandle)-1)

	void SetDataPath(const stl_string& sDataPath) { m_sDataPath = sDataPath; }
	const stl_string& GetDataPath() const { return m_sDataPath; }

	TFindDataHandle FindFirstFile(const stl_string& sPath, SFindData& findData);
	bool FindNextFile(TFindDataHandle eHandle, SFindData& findData);
	bool FindClose(TFindDataHandle eHandle);

	stl_string Convert(const stl_string& sFilePath);
	bool FileExcists() const;

	CFile* OpenFile(const stl_string& sFilePath, CFile::EOpenMode eOpenMode);
	bool CloseFile(CFile* pFile);

protected:

	CFileSystem();
	~CFileSystem();

	//////////////////////////////////////////////////////////////////////////

	// the default data path
	stl_string m_sDataPath;

	// active find results
	typedef stl_vector<TFindDataHandle> TFindDataList;
	TFindDataList m_vOpenFindFileResults;

	typedef stl_vector<CFile*> TFileList;
	TFileList m_vOpenFiles;

	CFileChangeMonitor* m_pFileChangeMonitor;

};