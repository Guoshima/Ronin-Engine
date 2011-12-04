#pragma once

class CFileChangeMonitorListener;
class CFileChangeMonitorThread;
//class CFileChangeMonitorWindow;

#include <set>
#include <queue>
#include <vector>
#include <map>
#include <string>

struct SFileChangeInfo
{
	enum EChangeType
	{
		//! error or unknown change type
		eChangeType_Unknown,
		//! the file was created
		eChangeType_Created,
		//! the file was deleted
		eChangeType_Deleted,
		//! the file was modified (size changed,write)
		eChangeType_Modified,
		//! this is the old name of a renamed file
		eChangeType_RenamedOldName,
		//! this is the new name of a renamed file
		eChangeType_RenamedNewName
	};

	SFileChangeInfo()
		: changeType(eChangeType_Unknown)
	{
	}

	stl_string filename;
	EChangeType changeType;
};

// Monitors directory for any changed files
class CFileChangeMonitor : public CCoreMemObject
{
public:

	CFileChangeMonitor();
	~CFileChangeMonitor();

	bool MonitorItem(const stl_string& sItem);
	void StopMonitor();
	//! check if any files where modified,
	//! this is a polling function, call it every frame or so
	bool HaveModifiedFiles() const;
	//! get next modified file, this file will be delete from list after calling this function,
	//! call it until HaveModifiedFiles return true or this function returns false
	bool PopNextFileChange(SFileChangeInfo& /*out*/rPoppedChange);
	void Subscribe(CFileChangeMonitorListener* pListener);
	void Unsubscribe(CFileChangeMonitorListener* pListener);
	bool IsDirectory(const char* pFilename);
	bool IsFile(const char* pFilename);

private:
	void NotifyListeners();

	typedef std::set<CFileChangeMonitorListener*> TListeners;
	TListeners m_listeners;

	//! Pointer to implementation class.
	CFileChangeMonitorThread* m_pThread;
	//std::auto_ptr<CFileChangeMonitorWindow> m_window;
};

// Used as base class (aka interface) to subscribe for file change events
class CFileChangeMonitorListener
{
public:
	CFileChangeMonitorListener()
		: m_pMonitor(NULL)
	{
	}
	
	virtual ~CFileChangeMonitorListener()
	{
		if (m_pMonitor)
		{
			m_pMonitor->Unsubscribe(this);
		}
	}

	virtual void OnFileMonitorChange() = 0;

	void SetMonitor(CFileChangeMonitor* pMonitor) 
	{
		m_pMonitor = pMonitor; 
	}
	
private:

	CFileChangeMonitor* m_pMonitor;
};

