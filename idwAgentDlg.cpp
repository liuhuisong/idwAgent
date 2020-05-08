
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
#include <string>


//CAboutDlg dialog used for App About

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
	, m_scroll(FALSE)
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
	ON_BN_CLICKED(IDC_READ_MIFARE, &CidwAgentDlg::OnBnClickedReadMifare)
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

	AfxSocketInit();
	BOOL b = m_server.init();
	if (!b) {
		log(_T("http server failed"));
		return false;
	}

	b = m_server.Listen();
	if (!b) {
		log(_T("listen failed"));
		return false;
	}

	if (b) {
		if (m_mifare.Initialize()) {
			log(_T("函数库初始化成功"));
			detectMifareDev();
		}
		else {
			log(_T("函数库初始化失败"));
			GetDlgItem(IDC_MIFARE_BEEP)->EnableWindow(FALSE);
		}
	}
	else {
		GetDlgItem(IDC_MIFARE_BEEP)->EnableWindow(FALSE);
	}

	SetDlgItemInt(IDC_BLOCK, 5);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CidwAgentDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}

	else if (nID == SC_CLOSE) {
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


int CidwAgentDlg::onRecvReq(char * buf, unsigned long*tagid)
{
	if (buf == NULL) {
		return -1;
	}

	int uid = -1;
	int utype = -1;
	int block = -1;//default
	int close = -1;

	std::string str = buf;
	size_t t1 = str.find_first_of("?");
	if (t1 == str.npos) {
		return -2;
	}
	size_t t2 = str.find_first_of(" ", t1);
	if (t2 == str.npos) {
		return -3;
	}

	std::string str2 = str.substr(t1 + 1, t2 - t1 - 1);
	t1 = 0;

	std::string str3;

	while (t1 < str2.length()) {
		t2 = str2.find_first_of("&", t1);
		if (t2 == str2.npos) {
			str3 = str2.substr(t1);
			t1 = str2.length();
		}
		else {
			str3 = str2.substr(t1, t2 - t1);
			t1 = t2 + 1;
		}


		const char*tag = "utype=";
		if (str3.substr(0, strlen(tag)) == tag) {
			utype = atoi(str3.c_str() + strlen(tag));
			continue;
		}

		tag = "uid=";
		if (str3.substr(0, strlen(tag)) == tag) {
			uid = atoi(str3.c_str() + strlen(tag));
			continue;
		}

		tag = "block=";
		if (str3.substr(0, strlen(tag)) == tag) {
			block = atoi(str3.c_str() + strlen(tag));
			continue;
		}

		tag = "close=";
		if (str3.substr(0, strlen(tag)) == tag) {
			close = atoi(str3.c_str() + strlen(tag));
			continue;
		}

	}

	if (utype == -1) {
		log(_T("一个错误的写卡请求(no utype)"));
		return 1;
	}

	if (uid == -1) {
		log(_T("uid=%d,invalid"), uid);
		return 1;
	}

	if (block == -1) {
	
		if (m_mifare.FindCard()) {
			*tagid = m_mifare.getSerialUL();			
			log(_T("成功读卡: <%08X>"), *tagid);
			return 0;
		}
		log(_T("没有找到卡"));
		return 1;
	}

	int n = m_mifare.WriteUserData(utype, uid, block, close);
	if (n == 0) {
		*tagid = m_mifare.getSerialUL();

		log(_T("成功写卡: <%08X>"), *tagid);
		return 0;
	}
	
	log(_T("写卡错误:%d,%s"), n,m_mifare.perror(n));

	for (int i = 0; i < 3; i++) {
		m_mifare.Beep(100);
		Sleep(100);
	}

	return n;
}

void CidwAgentDlg::hideToTaskIcon()
{
	this->ShowWindow(SW_HIDE);
	m_taskIcon.addIcon(TID, IDR_MAINFRAME, _T("点击显示写卡代理"), WM_USER);
}


afx_msg LRESULT CidwAgentDlg::OnUser(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN) {
		this->ShowWindow(SW_SHOW);
	}
	return 0;
}

void CidwAgentDlg::OnBnClickedMifareBeep()
{
	detectMifareDev();
}

bool CidwAgentDlg::detectMifareDev()
{
	if (m_mifare.Beep(300)) {
		log(_T("写卡设备已经就绪"));
		return true;
	}
	else {
		log(_T("写卡设备未连接"));
	}

	return false;
}


void CidwAgentDlg::log(LPCTSTR fmt, ...)
{
	CTime time;
	time = CTime::GetCurrentTime();

	va_list vl;
	va_start(vl, fmt);

	TCHAR buf[1024];
	vswprintf_s(buf, fmt, vl);

	CString s2;
	s2.Format(_T("%.2d:%.2d:%.2d"),
		time.GetHour(), time.GetMinute(), time.GetSecond());

	m_log.AddString(s2 + ": " + buf);
}

void CidwAgentDlg::OnBnClickedReadMifare()
{
	// TODO: Add your control notification handler code here
	TCHAR buf[32];
	long uid;

	int block = GetDlgItemInt(IDC_BLOCK);
	if (block < 0) {
		block = 0;
		SetDlgItemInt(IDC_BLOCK, block);
	}
	else if (block > 63) {
		block = 63;
		SetDlgItemInt(IDC_BLOCK, block);
	}

	int n = m_mifare.ReadUserData(block, buf, 32, &uid);
	if ( n== 0) {
		log(_T("read <%08X>, %s"), uid, buf);

		m_mifare.Beep(200);
	}
	else {
		log(_T("read error=%d"), n);
		m_mifare.Beep(100);
		Sleep(50);
		m_mifare.Beep(100);
		Sleep(50);
		m_mifare.Beep(100);
	}
}
