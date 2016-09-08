// HttpServer.cpp : implementation file
//

#include "stdafx.h"
#include "idwAgent.h"
#include "HttpServer.h"


// HttpServer

HttpServer::HttpServer() :m_pLog(NULL)
{
}

HttpServer::~HttpServer()
{
}


void HttpServer::OnAccept(int nErrorCode)
{
	HttpConnected* conn = new HttpConnected(m_pLog,m_mifare);
	
	if (Accept(*conn)){
		TRACE0("accepted");
	}
	else{
		TRACE0("accept failed");
		delete conn;
	}

	//CSocket::OnAccept(nErrorCode);
}

BOOL HttpServer::Create(CListBox*p,MifareUser*pu)
{
	BOOL b = CSocket::Create(SERVER_PORT);
	if (!b){
		return FALSE;
	}

	m_pLog = p;
	m_mifare = pu;

	return TRUE;
}
