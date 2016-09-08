#pragma once

// HttpConnected command target
#include "MifareUser.h"

class HttpConnected : public CSocket
{
public:
	HttpConnected(CListBox*p,MifareUser*pu);
	virtual ~HttpConnected();
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);

protected:
	CListBox* m_pLog;
	MifareUser*m_mifare;

protected:
	void httpRespon(const char*s);
};


