#include "DX11TestPCH.h"

#include "Window.h"

//////////////////////////////////////////////////////////////////////////

char CWindow::ms_acWindowClassName[] = "Kenzo Test App"; 

//////////////////////////////////////////////////////////////////////////

CWindow::SWindowDesc::SWindowDesc() :
	m_lWindowStyles(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
	m_hInstance(NULL), m_nCmdShow(TRUE), m_uiWidth(640), m_uiHeight(480),
	m_acWindowCaption(0)
{

}

CWindow::SWindowDesc::SWindowDesc(const SWindowDesc& kOther)
{
	m_lWindowStyles = kOther.m_lWindowStyles;
	m_hInstance = kOther.m_hInstance;
	m_nCmdShow = kOther.m_nCmdShow;
	m_uiWidth = kOther.m_uiWidth;
	m_uiHeight = kOther.m_uiHeight;
	m_acWindowCaption = 0;
	if (kOther.m_acWindowCaption)
		m_acWindowCaption = _strdup(kOther.m_acWindowCaption);
}

//////////////////////////////////////////////////////////////////////////

CWindow* CWindow::Create(SWindowDesc& kWindowDesc)
{
	CWindow* pkWindow = new CWindow;
	pkWindow->m_kWindowDesc = kWindowDesc;

	// Create window
	RECT rc = { 0, 0, kWindowDesc.m_uiWidth, kWindowDesc.m_uiHeight };
	AdjustWindowRect( &rc, kWindowDesc.m_lWindowStyles , FALSE );

	pkWindow->m_hWindow = CreateWindow( GetWindowClassName(), kWindowDesc.m_acWindowCaption, 
		kWindowDesc.m_lWindowStyles, CW_USEDEFAULT, CW_USEDEFAULT, 
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, kWindowDesc.m_hInstance,
		NULL );

	if( !pkWindow->m_hWindow )
	{
		delete pkWindow;
		return 0;
	}

	ShowWindow(pkWindow->m_hWindow, kWindowDesc.m_nCmdShow);
	UpdateWindow(pkWindow->m_hWindow);

	return pkWindow;
}