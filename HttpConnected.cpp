// HttpConnected.cpp : implementation file
//

#include "stdafx.h"
#include "idwAgent.h"
#include "HttpConnected.h"

#include "idwAgentDlg.h"

#define RECVBUFF_LEN	4096
#define SENDBUFF_LEN	1024
// HttpConnected

HttpConnected::HttpConnected()
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
	char buf2[SENDBUFF_LEN];
	char ori[128] = "*";

	const char*origin_header = "rigin:";

	unsigned long tagid;

	const char*perr="1=network error,2=load dll error,3=can't find card,4=auth error,5=write data error,6=change ac byte error";

	bool bReply = false;

	int n=Receive(buf, RECVBUFF_LEN);
	if (n > 0) {
		char*p1 = strstr(buf, origin_header);
		if (p1) {
			p1+=strlen(origin_header);
			while (*p1 == ' ') {
				p1++;
			}

			char*p2 = strstr(p1, "\r");
			if (p2) {
				p2--;
				memcpy(ori, p1, p2 - p1 + 1);
			}
		}

		n = ((CidwAgentDlg*)AfxGetMainWnd())->onRecvReq(buf, &tagid);
		if (n == 0) {
			sprintf_s(buf2, SENDBUFF_LEN, "{\"error\":\"OK\",\"value\":\"%08X\"}", tagid);
			httpRespon(buf2, ori);
			bReply = true;
		}
	}

	if(!bReply){
			sprintf_s(buf2, SENDBUFF_LEN, "{\"error\":\"FAILED\",\"value\":%d,\"desc\":\"%s\"}",n,perr);
			httpRespon(buf2, ori);
	}

	CSocket::Close();
	delete this;
	//CSocket::OnReceive(nErrorCode);
}

void HttpConnected::httpRespon(const char*json,const char*ori)
{
	const char* fmt = "HTTP/1.1 200 OK\r\n"
		"Access-Control-Allow-Origin: %s\r\n"
		"Content-Type: application/json; charset=utf-8\r\n"
		"Content-Length: %d\r\n"
		"Server: Apache/2.4\r\n"
		"Set-Cookie: Mifare=Longwan; path=/, domain=localhost\r\n"
		"Cache-Control: no-cache, must-revalidate\r\n"
		"Pragma: no-cache\r\n"
		"\r\n"
		"%s";

	char buf[SENDBUFF_LEN];
	sprintf_s(buf, SENDBUFF_LEN, fmt, ori, strlen(json), json);

	int len=strlen(buf);
	Send(buf,len);

	return;
}

