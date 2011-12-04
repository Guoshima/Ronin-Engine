#pragma once

class CWindow 
{
public:

	struct SWindowDesc {
		SWindowDesc();
		SWindowDesc(const SWindowDesc& kOther);

		HINSTANCE m_hInstance;
		int m_nCmdShow;
		unsigned int m_uiWidth;
		unsigned int m_uiHeight;
		unsigned long m_lWindowStyles;
		const char* m_acWindowCaption;			
	};

	static CWindow* Create(SWindowDesc& kWindowDesc);

	HWND GetWindowHandle() const { return m_hWindow; }
	static const char* GetWindowClassName() { return ms_acWindowClassName; } 

private:

	// private constructor
	CWindow() : m_hWindow(NULL) {}

	//////////////////////////////////////////////////////////////////////////

	// description given during creation of window
	SWindowDesc m_kWindowDesc;

	static char ms_acWindowClassName[];

	HWND m_hWindow;
};