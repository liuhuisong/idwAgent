#pragma once

// HttpConnected command target
#include "MifareUser.h"

class HttpConnected : public CSocket
{
public:
	HttpConnected();
	virtual ~HttpConnected();
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);

protected:
	void httpRespon(const char*s,const char*ori);
};


