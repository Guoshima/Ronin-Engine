#pragma once

class CFileChangeMonitorThread;
//class CFileChangeMonitorWindow;

#include "FileChangeInfo.h"

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

private:
	void NotifyListeners();

	//! Pointer to implementation class.
	CFileChangeMonitorThread* m_pThread;
	//stl_auto_ptr<CFileChangeMonitorWindow> m_window;
};


