
// idwAgentDlg.h : header file
//

#pragma once

#include "TaskIcon.h"
#include "afxwin.h"

#include "MifareUser.h"
#include "HttpServer.h"

// CidwAgentDlg dialog
class CidwAgentDlg : public CDialogEx
{
// Construction
public:
	CidwAgentDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IDWAGENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	TaskIcon m_taskIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();

public:
	afx_msg void OnBnClickedOk();

	/*0=ok,1 param error,3 device error or card not found,4 auth,5 write error
	*/
	int onRecvReq(char*p,unsigned long*tagid);

protected:
#define TID	1

	void hideToTaskIcon();
	bool detectMifareDev();
	void log(LPCTSTR fmt,...);

public:
	// //log list control
	CListBox m_log;
	BOOL m_scroll;

	MifareUser m_mifare;
	HttpServer m_server;

	afx_msg void OnBnClickedMifareBeep();
	afx_msg void OnBnClickedReadMifare();
};
