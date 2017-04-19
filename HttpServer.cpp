// HttpServer.cpp : implementation file
//

#include "stdafx.h"
#include "idwAgent.h"
#include "HttpServer.h"


// HttpServer

HttpServer::HttpServer()
{
}

HttpServer::~HttpServer()
{
}


void HttpServer::OnAccept(int nErrorCode)
{
	HttpConnected* conn = new HttpConnected();
	
	if (Accept(*conn)){
		TRACE0("accepted");
	}
	else{
		TRACE0("accept failed");
		delete conn;
	}

	//CSocket::OnAccept(nErrorCode);
}

BOOL HttpServer::init(int port)
{
	BOOL b = CSocket::Create(port);
	if (!b){
		return FALSE;
	}	

	return TRUE;
}
