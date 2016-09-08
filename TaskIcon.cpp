#include "stdafx.h"
#include "TaskIcon.h"


TaskIcon::TaskIcon() :m_hWnd(NULL)
{
	memset(&m_tids, 0, sizeof(m_tids));
}


TaskIcon::~TaskIcon()
{
}

BOOL TaskIcon::addIcon(int tid,UINT icon,LPCTSTR s,UINT msgid)
{
	if (m_hWnd == NULL) {
		TRACE0("not initalize");
		return false;
	}

	if (tid<0 || tid>TID_MAX){
		TRACE0("tid invalid");
		return false;
	}

	if (m_tids[tid]){
		return updateIcon(tid, icon, s,msgid);
	}

	return iconRequest(NIM_ADD, tid, icon, s, msgid);
}

BOOL TaskIcon::deleteIcon(int tid)
{

	if (tid<0 || tid>TID_MAX){
		TRACE0("tid invalid");
		return false;
	}

	m_tids[tid] = 0;

	return iconRequest(NIM_DELETE, tid, 0, NULL, 0);
}

BOOL TaskIcon::updateIcon(int tid/*0-12*/, UINT icon/*resource id*/, LPCTSTR s/*128 char,may be null*/, UINT msgid)
{

	if (tid<0 || tid>TID_MAX){
		TRACE0("tid invalid");
		return false;
	}

	if (!m_tids[tid]){
		TRACE0("tid not exists");
		return false;
	}

	return iconRequest(NIM_MODIFY, tid, icon, s, 0);
}

BOOL TaskIcon::initalize(HWND hwnd)
{
	if (m_hWnd == NULL){
		m_hWnd = hwnd;
		return true;
	}

	return false;
}

BOOL TaskIcon::iconRequest(UINT nim, int tid/*0-12*/, UINT icon/*resource id*/, LPCTSTR s/*128 char,may be null*/, UINT msgid/*send MESSAGE when it clicked*/)
{


	NOTIFYICONDATA nid = { 0 };
	nid.cbSize = sizeof(nid);

	nid.hWnd = m_hWnd;
	nid.uID = tid;      // 

	nid.uFlags = NIF_ICON;

	if (icon){
		//del command icon=0;
		nid.hIcon = AfxGetApp()->LoadIcon(icon);
	}

	if (s){
		nid.uFlags |= NIF_TIP;

#if (_WIN32_IE < 0x0500)
		strcpy_s(nid.szTip, 64, S_OFFLINE);
#else
		wcscpy_s(nid.szTip, 128, s);
#endif
	}

	if (msgid > 0){

		nid.uFlags |= NIF_MESSAGE;

		nid.uCallbackMessage = msgid;
	}

	//Add the notification to the tray
	return Shell_NotifyIcon(nim, &nid);
}


