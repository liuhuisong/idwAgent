#pragma once

// HttpServer command target
#include "HttpConnected.h"

#define SERVER_PORT	4050

class HttpServer : public CSocket
{
public:
	HttpServer();
	virtual ~HttpServer();
	virtual void OnAccept(int nErrorCode);

	BOOL Create(CListBox*p,MifareUser*pu);

protected:
	CListBox*m_pLog;
	MifareUser*m_mifare;
};


