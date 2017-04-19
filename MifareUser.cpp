#include "stdafx.h"
#include "MifareUser.h"

#ifndef TRACE
#define TRACE0(x)
#define TRACE1(x,y)
#endif

MifareUser::MifareUser() :
m_hDllLibrary(NULL),
m_piccrequest(NULL),
m_piccauthkey1(NULL),
m_piccread(NULL),
m_piccwrite(NULL),
m_picchalt(NULL),
m_pcdbeep(NULL),
m_picc_error(0)
{
}

MifareUser::~MifareUser()
{
	Uninitialize();
}

bool MifareUser::Initialize()
{
	if (m_hDllLibrary){
		TRACE0("load library already\n");
		return true;
	}

	m_hDllLibrary = LoadLibrary(DLL_NAME);
	if (m_hDllLibrary == NULL){
		TRACE1("load library failed:0x%X\n", GetLastError());
		return false;
	}

	m_piccrequest = (piccrequest_t)GetProcAddress(m_hDllLibrary, "piccrequest");
	m_piccauthkey1 = (piccauthkey1_t)GetProcAddress(m_hDllLibrary, "piccauthkey1");
	m_piccread = (piccread_t)GetProcAddress(m_hDllLibrary, "piccread");
	m_piccwrite = (piccwrite_t)GetProcAddress(m_hDllLibrary, "piccwrite");
	m_picchalt = (picchalt_t)GetProcAddress(m_hDllLibrary, "picchalt");
	m_pcdbeep = (pcdbeep_t)GetProcAddress(m_hDllLibrary, "pcdbeep");

	if (m_piccrequest == NULL ||
		m_piccauthkey1 == NULL ||
		m_piccread == NULL ||
		m_piccwrite == NULL ||
		m_picchalt == NULL||
		m_pcdbeep==NULL){

		TRACE0("function address not found\n");
		FreeLibrary(m_hDllLibrary);

		m_hDllLibrary = NULL;
		return false;
	}

	//Beep(400);
	return true;
}

bool MifareUser::Uninitialize()
{
	m_piccrequest = NULL;
	m_piccauthkey1 = NULL;
	m_piccread = NULL;
	m_piccwrite = NULL;
	m_picchalt = NULL;

	if (m_hDllLibrary){
		FreeLibrary(m_hDllLibrary);
		m_hDllLibrary = NULL;
		return true;
	}

	return false;
}

bool MifareUser::FindCard()
{
	if (m_hDllLibrary == NULL){
		TRACE0("not load library\n");
		return false;
	}

	if (m_piccrequest == NULL){
		TRACE0("not function address\n");
		return false;
	}

	uchar t=m_piccrequest((uchar*)(char*)m_serial);
	if (t != 0){
		TRACE1("request failed:%d\n", t);
		m_picc_error = t;
		return false;
	}

	return true;
}

bool MifareUser::AuthA(Mifare::Key keya, int section/*0-15*/)
{
	if (m_hDllLibrary == NULL){
		TRACE0("not load library\n");
		return false;
	}

	if (m_piccauthkey1 == NULL){
		TRACE0("not function address\n");
		return false;
	}
	
	uchar t = m_piccauthkey1((uchar*)(char*)m_serial, section, 1, (uchar*)(char*)keya);
	if (t != 0){
		TRACE1("autha failed:%d", t);
		m_picc_error = t;
		return false;
	}

	return true;
}

bool MifareUser::ReadData(Mifare::data_block*data, int section, int block)
{
	if (m_hDllLibrary == NULL){
		TRACE0("not load library\n");
		return false;
	}

	if (m_piccread == NULL){
		TRACE0("not function address\n");
		return false;
	}

	uchar t = m_piccread(section * 4 + block, (uchar*)(char*)data);
	if (t != 0){
		TRACE1("read failed:%d\n", t);
		m_picc_error = t;
		return false;
	}

	return true;
}

bool MifareUser::WriteData(Mifare::data_block*data, int section, int block)
{
	if (m_hDllLibrary == NULL){
		TRACE0("not load library\n");
		return false;
	}

	if (m_piccwrite == NULL){
		TRACE0("not function address\n");
		return false;
	}

	uchar*p = (uchar*)(data->operator LPSTR());
	uchar t = m_piccwrite(section * 4 + block, p);
	if (t != 0){
		TRACE1("write failed:%d", t);
		m_picc_error = t;
		return false;
	}

	return true;
}

bool MifareUser::Halt()
{
	if (m_hDllLibrary == NULL){
		TRACE0("not load library\n");
		return false;
	}

	if (m_picchalt == NULL){
		TRACE0("not function address\n");
		return false;
	}

	uchar t = m_picchalt();
	if (t != 0){
		TRACE1("halt failed:%d\n", t);
		m_picc_error = t;
		return false;
	}

	return true;
}

bool MifareUser::Beep(int ms){
	if (m_hDllLibrary == NULL){
		TRACE0("not load library\n");
		return false;
	}

	if (m_pcdbeep == NULL){
		TRACE0("not function address\n");
		return false;
	}

	uchar t = m_pcdbeep(ms);
	if (t != 0){
		TRACE1("beep failed:%d\n", t);
		m_picc_error = t;
		return false;
	}

	return true;
}

Mifare::Serial MifareUser::getSerial()
{
	return m_serial;
}

int MifareUser::getLastPiccError()
{
	return m_picc_error;
}

/*
info: 2=load dll error,3=can't find card,4=auth error,5=write data error,6=change ac byte error
*/
PTSTR MifareUser::perror(int error) {
	if (error == 2) {
		return _T("load dll error");
	}
	
	if (error == 3) {
		return _T("no device, or no card found");
	}
	
	if (error == 4) {
		return _T("auth error");
	}
	
	if (error == 5) {
		return _T("write data error");
	}
	
	if (error == 6) {
		return _T("change ac byte error");
	}

	return _T("na");
}

int MifareUser::WriteUserData(int user_type, int id,int nBlock,int acChange)
{
	/*
	if (user_type != USER_MAINT && user_type != USER_SAFER){
		return 1;
	}*/

	if (m_hDllLibrary == NULL){
		return 2;
	}

	if (!FindCard()){
		return 3;
	}

	int section = SECTION;
	int block = BLOCK;
	int format_mifare = FORMAT_MIFARE;

	if (nBlock >= 0 && nBlock<=63) {
		section = (nBlock / 4) % 16;
		block = nBlock % 4;
	}
	if (acChange != -1) {
		format_mifare = acChange;
	}

	Mifare::Key key = Mifare::getDefaultKey();
	if (!AuthA(key, section)){
		return 4;
	}

	byte b0 = (byte)user_type;
	Mifare::data_block db(b0, id);
	if (!WriteData(&db, section, block)){
		return 5;
	}

	if (format_mifare){
		Mifare::Key keya = m_serial.getLongWanKeyA(section);
		Mifare::Key Keyb = m_serial.getLongWanKeyB(section);
		Mifare::Ac ac = Mifare::getLongWanAC();

		Mifare::data_block db3(keya, ac, Keyb);
		if (!WriteData(&db3, section, 3)){
			TRACE0("changed passwod failed\n");
			return 6;
		}
	}

	Beep(100);
	Halt();

	return 0;
}

int MifareUser::ReadUserData(int block, TCHAR*p, int len, long*uid) {
	if (m_hDllLibrary == NULL) {
		return 2;
	}

	if (!FindCard()) {
		return 3;
	}

	if (block < 0 || block>63) {
		return 4;
	}

	int section = block/4;
	block = block % 4;

	Mifare::Key key = Mifare::getDefaultKey();
	if (!AuthA(key, section)) {
		return 4;
	}

	Mifare::data_block data;
	if (!ReadData(&data, section, block)) {
		return 6;
	}
	
	if (uid) {
		*uid =  getSerialUL();
	}

	if (p && len > 8) {
		data.HexString(p, len);
	}

	return 0;
}

unsigned long MifareUser::getSerialUL()
{
	Mifare::Serial serial = getSerial();
	return serial.getSerial();
}



