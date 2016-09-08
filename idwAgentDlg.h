
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

protected:
#define TID	1

	void hideToTaskIcon();
	bool isDevReady();
public:
	// //log list control
	CListBox m_log;
	BOOL m_scroll;
	MifareUser m_mifare;
	HttpServer m_server;
	bool m_bServer;

	afx_msg void OnBnClickedMifareBeep();
};
