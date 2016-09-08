// HttpConnected.cpp : implementation file
//

#include "stdafx.h"
#include "idwAgent.h"
#include "HttpConnected.h"

#define RECVBUFF_LEN	4096
// HttpConnected

HttpConnected::HttpConnected(CListBox*p, MifareUser*pu) :m_pLog(p), m_mifare(pu)
{
}

HttpConnected::~HttpConnected()
{
}


// HttpConnected member functions


void HttpConnected::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class

	CSocket::OnClose(nErrorCode);

	TRACE0("deleted myselef");	
}


void HttpConnected::OnReceive(int nErrorCode)
{
	char buf[RECVBUFF_LEN];

	int n=Receive(buf, RECVBUFF_LEN);
	if (n > 0){

		int uid = 0;
		int utype = 0;

		char*p1 = strstr(buf, "utype=");
		if (p1){
			utype = atoi(p1 + strlen("utype="));
		}

		char*p2 = strstr(buf, "uid=");
		if (p1){
			uid = atoi(p2 + strlen("uid="));
		}

		if (utype>0 && uid>0){
			int n = m_mifare->WriteUserData(utype, uid);
			if (n==0){				

				unsigned long tagid=m_mifare->getSerialUL();
				char buf[64];
				sprintf_s(buf, 64, "%08X", tagid);
				httpRespon(buf);

				CString s;
				s.Format(_T("³É¹¦Ð´¿¨: %08X"), tagid);
				Longwan_log(s, m_pLog);
			}
			else{
				TCHAR buf2[100];
				swprintf_s(buf2,100,_T("Ð´¿¨´íÎó:%d"), n);
				Longwan_log(buf2, m_pLog);
				httpRespon("FAILED");

				m_mifare->Beep(100);
				Sleep(200);
				m_mifare->Beep(100);
				Sleep(200);
				m_mifare->Beep(100);
			}
		}
		else{			
			m_mifare->Beep(100);
			Sleep(200);
			m_mifare->Beep(100);
			Sleep(200);
			m_mifare->Beep(100);
			Longwan_log(_T("Ò»¸ö´íÎóµÄÐ´¿¨ÇëÇó"), m_pLog);
		}
	}

	CSocket::Close();
	delete this;
	//CSocket::OnReceive(nErrorCode);
}

void HttpConnected::httpRespon(const char*s)
{
	const char* fmt = "HTTP/1.1 200 OK\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Content-Length: %d\r\n"
		"Server: Apache\r\n"
		"\r\n"
		"%s";

	char buf[RECVBUFF_LEN];
	sprintf_s(buf, 1024, fmt, strlen(s), s);

	int len=strlen(buf);
	Send(buf,len );
	return;
}

