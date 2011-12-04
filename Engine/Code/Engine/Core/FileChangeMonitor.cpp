#include "CorePCH.h"

#include "FileChangeMonitor.h"
#include "Thread.h"

#include <sys/types.h> 
#include <sys/stat.h> 

/*
//! Message used to communicate with internally created window
#define WM_FILEMONITORCHANGE (WM_APP + 10)

namespace FileChangeMonitor
{
// the time passed between two identical notifications for same file, used to avoid duplicate notifications
static const unsigned int kDefaultIgnoreTimeMsec = 100;
// the time passed until a file notification is taken out of the 'recently' watched files (used for duplicate avoidance also)
static const unsigned int kExpireFileChangeTimeMsec = 5000;
// buffer size used with ReadDirectoryChangesW to get file notifications in OVERLAPPED mode
static const unsigned int kFileChangeNotificationBufferSize = 4096;
};

// Window is used solely to receive notifications from different threads
class CFileChangeMonitorWindow : public CWnd
{
public:
	static const char* GetClassName() 
	{
		return "CryFileChangeMonitor";
	}

	CFileChangeMonitorWindow(CFileChangeMonitor* pMonitor)
		: m_pMonitor(pMonitor)
	{
		RECT rect = {0};

		RegisterWindowClass();
		VERIFY(CreateEx(0, GetClassName(), "Hidden File Monitor", 0, rect, 0, 0, 0) != FALSE);
	}

	DECLARE_MESSAGE_MAP()

protected:
	LRESULT OnFileMonitorChange(WPARAM wparam, LPARAM lparam)
	{
		if (m_pMonitor)
		{
			m_pMonitor->NotifyListeners();
		}

		return 0;
	}

	static bool RegisterWindowClass()
	{
		WNDCLASS windowClass = {0};

		windowClass.style = CS_DBLCLKS;
		windowClass.lpfnWndProc = &AfxWndProc;
		windowClass.hInstance = AfxGetInstanceHandle();
		windowClass.hIcon = NULL;
		windowClass.hCursor = NULL;
		windowClass.hbrBackground = NULL;
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = GetClassName();

		return AfxRegisterClass(&windowClass);
	}

	CFileChangeMonitor* m_pMonitor;
};

BEGIN_MESSAGE_MAP(CFileChangeMonitorWindow, CWnd)
	ON_MESSAGE(WM_FILEMONITORCHANGE, OnFileMonitorChange)
END_MESSAGE_MAP()
*/

// used to keep track of folder changes
struct SFolderWatchAsyncData
{
	SFolderWatchAsyncData()
		: hFolder(0)
	{
		memset(&overlapped, 0, sizeof(overlapped));
	}

	stl_string folderPath;
	HANDLE hFolder;
	std::vector<BYTE> data;
	OVERLAPPED overlapped;
};

// Directory monitoring thread.
class CFileChangeMonitorThread : public CThread
{
public:
	typedef std::map<stl_string/*filename*/,std::map<DWORD/*notify action id*/, unsigned int/*last notify time*/>> TFileChangeTimeMap;
	std::vector<HANDLE> m_handles;
	std::vector<stl_string> m_pendingItems;
	//MTQueue<stl_string> m_pendingItems;
	std::vector<SFolderWatchAsyncData*> m_monitorItems;
	std::vector<SFileChangeInfo> m_fileChanges;
	//MTQueue<SFileChangeInfo> m_fileChanges;
	TFileChangeTimeMap m_changedFilesTime;
	//CryMutex m_changedFilesTimeMutex;
	//HWND m_listeningWindow;
	static HANDLE m_killEvent;
	static HANDLE m_updateEvent;

	CFileChangeMonitorThread(/*HWND listeningWindow*/)
		: CThread("FileChangeMonitor") //, m_listeningWindow(listeningWindow)
	{
		m_killEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		m_handles.push_back(m_killEvent);
		m_updateEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		m_handles.push_back(m_updateEvent);
	}

	virtual ~CFileChangeMonitorThread()
	{
		if(m_killEvent)
		{
			::CloseHandle(m_killEvent);
			m_killEvent = 0;
		}

		if(m_updateEvent)
		{
			::CloseHandle(m_updateEvent);
			m_updateEvent = 0;
		}
	}

	//! \return changed file count 
	size_t FindChangedFiles(SFolderWatchAsyncData* pFolder)
	{
		DWORD byteCount = 0;
		//ASSERT(pFolder);

		if (!pFolder->overlapped.hEvent || !pFolder->hFolder)
			return 0;

		size_t fileCount = 0;

		if (GetOverlappedResult(pFolder->hFolder, &pFolder->overlapped, &byteCount, TRUE))
		{
			FILE_NOTIFY_INFORMATION *lpInfo = (FILE_NOTIFY_INFORMATION*)&pFolder->data[0];

			while (lpInfo)
			{
				const int kMaxFilenameSize = 512;
				char wcFileName[kMaxFilenameSize + 1] = {0};

				if (lpInfo->FileNameLength)
				{
					//memcpy(wcFileName, lpInfo->FileName, min(kMaxFilenameSize * sizeof(WCHAR), lpInfo->FileNameLength));
					stl_string filename = pFolder->folderPath + stl_string(wcFileName);

					AddChangedFile(filename.c_str(), lpInfo->Action);
					++fileCount;
				}

				if (lpInfo->NextEntryOffset)
				{
					lpInfo = (FILE_NOTIFY_INFORMATION*)(((BYTE*)lpInfo) + lpInfo->NextEntryOffset);
				}
				else
				{
					lpInfo = NULL;
				}
			}

			// arm the watcher's event
			ActivateFileWatcher(pFolder);
		}

		return fileCount;
	}

	void AddChangedFile(const char* pFilename, DWORD aAction)
	{
		//CryAutoLock<CryMutex> lock(m_changedFilesTimeMutex);
		char strCanonicalPath[MAX_PATH] = {0};

		/*
		if (FALSE == PathCanonicalize(strCanonicalPath, pFilename))
		{
			//ASSERT(!"Could not canonize the path, using the incoming path");
			strcpy_s(strCanonicalPath, MAX_PATH, pFilename);
		}
		*/

		TFileChangeTimeMap::iterator iter = m_changedFilesTime.find(strCanonicalPath);
		unsigned int timeMsec = ::GetTickCount();

		// for each type of file notification action, we keep the type and time when the last was issued
		// this happens for each monitored file, and if the same action was issued in a short delta time, we skip it

		// we have this is the queue, check the time diff
		if (iter != m_changedFilesTime.end())
		{
			// if we have an action of this type in the file's times map,
			// then check to see if the last notify time is recent
			if( iter->second.find(aAction) != iter->second.end() )
			{
				unsigned int diff = timeMsec - iter->second[aAction];

				/*
				if (diff <= FileChangeMonitor::kDefaultIgnoreTimeMsec)
				{
					// skipped file change, duplicate or changed too fast
					return;
				}
				*/
			}
		}
		
		SFileChangeInfo change;
		stl_string actionName;

		change.filename = strCanonicalPath;

		switch (aAction)
		{
		case FILE_ACTION_ADDED:
			{
				change.changeType = SFileChangeInfo::eChangeType_Created;
				actionName = "created";
				break;
			}
		case FILE_ACTION_REMOVED:
			{
				change.changeType = SFileChangeInfo::eChangeType_Deleted;
				actionName = "deleted";
				break;
			}
		case FILE_ACTION_MODIFIED:
			{
				change.changeType = SFileChangeInfo::eChangeType_Modified;
				actionName = "modified";
				break;
			}
		case FILE_ACTION_RENAMED_OLD_NAME:
			{
				change.changeType = SFileChangeInfo::eChangeType_RenamedOldName;
				actionName = "old name";
				break;
			}
		case FILE_ACTION_RENAMED_NEW_NAME:
			{
				change.changeType = SFileChangeInfo::eChangeType_RenamedNewName;
				actionName = "new name";
				break;
			}
		default:
			{
				actionName = "unknown";
				break;
			}
		}

		//TODO: remove this log msg when the monitor code is proven stable and reliable enough
		//Log("File changed [%s]: '%s'", (LPCSTR)actionName, strCanonicalPath);
		m_fileChanges.push_back(change);
		m_changedFilesTime[strCanonicalPath][aAction] = timeMsec;
	}

	void CleanupExpiredFileTimes()
	{
		/*
		//CryAutoLock<CryMutex> lock(m_changedFilesTimeMutex);
		
		TFileChangeTimeMap::iterator iter = m_changedFilesTime.begin();
		//unsigned int timeMsec = ::GetTickCount();

		while (iter != m_changedFilesTime.end())
		{
			// check all file's notify action times for expiration
			std::map<DWORD, unsigned int>::iterator iterTime = iter->second.begin();
			
			while (iterTime != iter->second.end())
			{
				unsigned int diff = timeMsec - iterTime->second;

				// is this notify action expired ?
				if (diff > FileChangeMonitor::kExpireFileChangeTimeMsec)
				{
					// take it out from the notify action times map
					iter->second.erase(iterTime++);
					continue;
				}

				++iterTime;
			}			
			
			if (iter->second.empty())
			{
				m_changedFilesTime.erase(iter++);
				continue;
			}

			++iter;
		}
		*/
	}

	BOOL ActivateFileWatcher(SFolderWatchAsyncData* pFolder)
	{
		//ASSERT(pFolder);
		//ASSERT(pFolder->data.size());
		ResetEvent(pFolder->overlapped.hEvent);

		return ReadDirectoryChangesW(
			pFolder->hFolder, &pFolder->data[0], pFolder->data.size(), TRUE,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME,
			NULL, &pFolder->overlapped, NULL);
	}

	SFolderWatchAsyncData* FindMonitoredItemByEventHandle(HANDLE hnd)
	{
		for (int i = 0, iCount = m_monitorItems.size(); i < iCount; ++i)
		{
			if (m_monitorItems[i]->overlapped.hEvent == hnd)
			{
				return m_monitorItems[i];
			}
		}

		return NULL;
	}

	void DeleteAllMonitoredItems()
	{
		for (size_t i = 0, iCount = m_monitorItems.size(); i < iCount; ++i)
		{
			SFolderWatchAsyncData* pItem = m_monitorItems[i];

			::CloseHandle(pItem->hFolder);
			::CloseHandle(pItem->overlapped.hEvent);
			delete pItem;
		}

		m_monitorItems.clear();

		// clear other buffers
		{
			//CryAutoLock<CryMutex> lock(m_changedFilesTimeMutex);
			m_changedFilesTime.clear();
		}

		m_handles.clear();
	}

	void CheckMonitoredItems()
	{
		// check other event handles if they're signalled, skip first two
		for (int i = 0, iCount = m_monitorItems.size(); i < iCount; ++i)
		{
			SFolderWatchAsyncData* pItem = m_monitorItems[i];

			if (pItem && WAIT_OBJECT_0 == WaitForSingleObject(pItem->overlapped.hEvent, 1))
			{
				// gather the changed files
				size_t fileChangeCount = FindChangedFiles(pItem);

				if (fileChangeCount)
				{
					// notify listening window that something has changed.
					//PostMessage(m_listeningWindow, WM_FILEMONITORCHANGE, 0, 0);
				}
			}
		}
	}

	void AddPendingItem(const char* pItem)
	{
		HANDLE hDir = CreateFile(
			pItem, FILE_LIST_DIRECTORY|GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

		if (hDir)
		{
			SFolderWatchAsyncData* pFolder = new SFolderWatchAsyncData();

			//ASSERT(pFolder);
			pFolder->folderPath = pItem;
			pFolder->data.resize(4048);//FileChangeMonitor::kFileChangeNotificationBufferSize);
			pFolder->hFolder = hDir;
			pFolder->overlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

			if (ActivateFileWatcher(pFolder))
			{
				m_monitorItems.push_back(pFolder);
				m_handles.push_back(pFolder->overlapped.hEvent);
				//Log("Adding for file change notifications: '%s'", pItem);
			}
			else
			{
				// abort, delete
				::CloseHandle(pFolder->hFolder);
				::CloseHandle(pFolder->overlapped.hEvent);
				delete pFolder;
			}
		}
	}

	bool MonitoredItemExists(const stl_string& sItem)
	{
		/*
		// test for duplicate
		for (std::vector<SFolderWatchAsyncData*>::iterator iter = m_monitorItems.begin();
			iter != m_monitorItems.end(); ++iter)
		{
			if (sItem.CompareNoCase((*iter)->folderPath) == 0)
			{
				return true;
			}
		}
		*/

		return false;
	}

	size_t ThreadProcedure(void* pParameter)
	{
		//CryThreadSetName(-1, "FileChangeMonitor");

		while (true) 
		{ 
			UINT handleCount = m_handles.size();
			DWORD dwWaitStatus;
			bool bKill = false;

			dwWaitStatus = WaitForMultipleObjects(
												handleCount, &m_handles[0], FALSE, INFINITE);

			if (dwWaitStatus >= WAIT_OBJECT_0 && dwWaitStatus < WAIT_OBJECT_0 + handleCount)
			{
				int triggeredHandleIndex = dwWaitStatus - WAIT_OBJECT_0;

				// this is the kill event, so break out
				if (!triggeredHandleIndex)
				{
					bKill = true;
				}

				// this is the update event
				if (triggeredHandleIndex == 1)
				{
					// add pending items to list
					while(!m_pendingItems.empty())
					{
						const stl_string item = m_pendingItems.back();

						m_pendingItems.pop_back();

						if (!MonitoredItemExists(item))
						{
							AddPendingItem(item.c_str());
						}
					}

					// prepare the update event for another triggering
					ResetEvent(m_updateEvent);
				}

				CheckMonitoredItems();

				if (bKill)
				{
					break;
				}
			}
		}

		DeleteAllMonitoredItems();
		m_fileChanges.clear();
		m_pendingItems.clear();

		return 1;
	}
};

HANDLE CFileChangeMonitorThread::m_killEvent = 0;
HANDLE CFileChangeMonitorThread::m_updateEvent = 0;

//////////////////////////////////////////////////////////////////////////
CFileChangeMonitor::CFileChangeMonitor()
{
	//m_window.reset(new CFileChangeMonitorWindow(this));
	m_pThread = IntNew CFileChangeMonitorThread();//m_window->GetSafeHwnd());
	m_pThread->Initialize(true);
}

//////////////////////////////////////////////////////////////////////////
CFileChangeMonitor::~CFileChangeMonitor()
{
	for (TListeners::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		CFileChangeMonitorListener* pListener = *it;

		if (pListener)
		{
			pListener->SetMonitor(NULL);
		}
	}

	// Send to thread a kill event.
	StopMonitor();

	IntSafeDelete(m_pThread);
}

//////////////////////////////////////////////////////////////////////////
bool CFileChangeMonitor::IsDirectory(const char* sFileName)
{
	struct __stat64 my_stat;

	if (_stat64(sFileName, &my_stat) != 0)
	{
		return false;
	}

	return ((my_stat.st_mode & S_IFDIR) != 0);
}

//////////////////////////////////////////////////////////////////////////
bool CFileChangeMonitor::IsFile(const char* sFileName)
{
	struct __stat64 my_stat;

	if (_stat64(sFileName, &my_stat) != 0)
	{	
		return false;
	}

	return ((my_stat.st_mode & S_IFDIR) == 0);
}

//////////////////////////////////////////////////////////////////////////
bool CFileChangeMonitor::MonitorItem(const stl_string &sItem)
{
	bool bRet = false;
	bool bIsDirectory = IsDirectory(sItem.c_str());//Path::RemoveBackslash(sItem));
	bool bIsFile = IsFile(sItem.c_str());

	if ((bIsDirectory || bIsFile)
		&& m_pThread
		&& CFileChangeMonitorThread::m_updateEvent != 0)
	{
		m_pThread->m_pendingItems.push_back(bIsDirectory ? sItem : sItem);//Path::RemoveBackslash(Path::GetPath(sItem)));
		SetEvent(CFileChangeMonitorThread::m_updateEvent);
		bRet = true;
	}

	return bRet;
}

void CFileChangeMonitor::StopMonitor()
{
	if (m_pThread && CFileChangeMonitorThread::m_killEvent != 0)
	{
		SetEvent(CFileChangeMonitorThread::m_killEvent);
	}
}
	
bool CFileChangeMonitor::HaveModifiedFiles() const
{
	return !m_pThread->m_fileChanges.empty();
}

bool CFileChangeMonitor::PopNextFileChange(SFileChangeInfo& rPoppedChange)
{
	if (!m_pThread->m_fileChanges.empty())
	{
		rPoppedChange = m_pThread->m_fileChanges.back();
		m_pThread->m_fileChanges.pop_back();
		m_pThread->CleanupExpiredFileTimes();

		return true;
	}

	return false;
}

void CFileChangeMonitor::Subscribe(CFileChangeMonitorListener* pListener)
{
	//ASSERT(pListener);
	pListener->SetMonitor(this);
	m_listeners.insert(pListener);
}

void CFileChangeMonitor::Unsubscribe(CFileChangeMonitorListener* pListener)
{
	//ASSERT(pListener);
	m_listeners.erase(pListener);
	pListener->SetMonitor(NULL);
}

void CFileChangeMonitor::NotifyListeners()
{
	TListeners::iterator it;

	for (it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		CFileChangeMonitorListener* pListener = *it;

		if (pListener)
		{
			pListener->OnFileMonitorChange();
		}
	}
}
