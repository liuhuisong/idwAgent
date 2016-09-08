
// idwAgentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "idwAgent.h"
#include "idwAgentDlg.h"
#include "afxdialogex.h"
#include "MifareUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "resource.h"


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CidwAgentDlg dialog



CidwAgentDlg::CidwAgentDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CidwAgentDlg::IDD, pParent)
	, m_scroll(FALSE), m_bServer(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CidwAgentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG, m_log);
	DDX_Check(pDX, IDC_CHECK1, m_scroll);
}

BEGIN_MESSAGE_MAP(CidwAgentDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CidwAgentDlg::OnBnClickedOk)
	ON_MESSAGE(WM_USER, &CidwAgentDlg::OnUser)
	ON_BN_CLICKED(IDC_MIFARE_BEEP, &CidwAgentDlg::OnBnClickedMifareBeep)
END_MESSAGE_MAP()


// CidwAgentDlg message handlers

BOOL CidwAgentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_taskIcon.initalize(GetSafeHwnd());

	if (m_mifare.Initialize()){
		Longwan_log(_T("初始化成功"),&m_log);
		isDevReady();
	}
	else{
		Longwan_log(_T("初始化失败"),&m_log);
		GetDlgItem(IDC_MIFARE_BEEP)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CidwAgentDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}

	else if (nID == SC_CLOSE){
		CDialogEx::OnCancel();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CidwAgentDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CidwAgentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CidwAgentDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	hideToTaskIcon();
}


void CidwAgentDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnCancel();
}


void CidwAgentDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	hideToTaskIcon();
}

void CidwAgentDlg::hideToTaskIcon()
{
	this->ShowWindow(SW_HIDE);
	m_taskIcon.addIcon(TID, IDR_MAINFRAME, _T("点击显示写卡代理"), WM_USER);
}


afx_msg LRESULT CidwAgentDlg::OnUser(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN){
		this->ShowWindow(SW_SHOW);
	}
	return 0;
}

void CidwAgentDlg::OnBnClickedMifareBeep()
{
	isDevReady();
}

bool CidwAgentDlg::isDevReady()
{
	if (m_mifare.Beep(400)){
		if (FORMAT_MIFARE){
			Longwan_log(_T("写卡设备已经就绪"), &m_log);
		}
		else{
			Longwan_log(_T("写卡设备已经就绪(非格式化模式)"), &m_log);
		}
		if (!m_bServer){
			BOOL b = AfxSocketInit();			
			b=m_server.Create(&m_log,&m_mifare);
			if (!b){
				TRACE0("http server failed");
			}
			else{
				b=m_server.Listen();
				if (!b){
					TRACE0("listen failed");
				}
				else{
					m_bServer = true;
					Longwan_log(_T("OK"),&m_log);
				}
			}
		}
		return true;
	}
	else{
		Longwan_log(_T("写卡设备未连接"),&m_log);
	}

	return false;
}
