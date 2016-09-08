
// stdafx.cpp : source file that includes just the standard includes
// idwAgent.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


void Longwan_log(CString s, CListBox*p)
{

	CTime time;
	time = CTime::GetCurrentTime();

	CString s2;
	s2.Format(_T("%4d-%.2d-%.2d %.2d:%.2d:%.2d"),
		time.GetYear(), time.GetMonth(), time.GetDay(),
		time.GetHour(), time.GetMinute(), time.GetSecond());
	s2 += " : ";
	s2 += s;

	p->AddString(s2);
}