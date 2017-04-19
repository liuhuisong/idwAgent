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

	BOOL init(int port=SERVER_PORT);

protected:
};


