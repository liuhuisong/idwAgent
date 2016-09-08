#pragma once

#include "Mifare.h"

//#define DLL_NAME	_T("C:\\Windows\\SysWOW64\\OUR_MIFARE")
#define DLL_NAME	_T("OUR_MIFARE")
//_T("c:\\windows\\system32\\OUR_MIFARE")
typedef unsigned char uchar;

#define SECTION			1
#define BLOCK			1
#define USER_SAFER		2
#define USER_MAINT		3
#define FORMAT_MIFARE	0

class MifareUser
{
public:
	MifareUser();
	~MifareUser();

	bool Initialize();
	bool Uninitialize();

	int WriteUserData(int user_type, int id);

public:
	bool FindCard();
	bool AuthA(Mifare::Key keya,int section);
	bool ReadData(Mifare::data_block*data, int section, int block);
	bool WriteData(Mifare::data_block*data, int section, int block);
	bool Halt();
	bool Beep(int ms);

	Mifare::Serial getSerial();
	unsigned long getSerialUL();
	int getLastPiccError();

protected:
	HMODULE m_hDllLibrary;

	typedef uchar(__stdcall* piccrequest_t)(uchar *serial);
	typedef uchar(__stdcall* piccauthkey1_t)(uchar *serial,
		uchar area, uchar keyA1B0, uchar *picckey);
	typedef uchar(__stdcall* piccread_t)(uchar block, uchar *piccdata);
	typedef uchar(__stdcall* piccwrite_t)(uchar block, uchar *piccdata);
	typedef uchar(__stdcall* picchalt_t)();
	typedef uchar(__stdcall* pcdbeep_t)(unsigned long xms);

	piccrequest_t m_piccrequest;
	piccauthkey1_t m_piccauthkey1;
	piccread_t m_piccread;
	piccwrite_t m_piccwrite;
	picchalt_t m_picchalt;
	pcdbeep_t m_pcdbeep;

	Mifare::Serial m_serial;

	int m_picc_error;
};

