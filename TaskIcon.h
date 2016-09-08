#pragma once


#define TID_MAX	12

class TaskIcon
{
public:
	TaskIcon();
	virtual ~TaskIcon();


protected:
	HWND m_hWnd;
	int m_tids[TID_MAX+1];

protected:
	BOOL iconRequest(UINT nim, int tid/*0-12*/, UINT icon/*resource id*/, LPCTSTR s/*128 char,may be null*/, UINT msgid/*send MESSAGE when it clicked*/);

public:

	BOOL addIcon(int tid/*0-12*/,
		UINT icon/*resource id*/,
		LPCTSTR s/*128 char,may be null*/,
		UINT msgid/*send MESSAGE,map it to (WPARAM,LPARAM/WM_LBUTTONDOWN/)*/);

	BOOL deleteIcon(int tid);
	BOOL updateIcon(int tid, UINT icon, LPCTSTR s, UINT msgid);

	BOOL initalize(HWND hwnd);
};

