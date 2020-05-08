//liuhuisong@hotmail.com
#include "stdafx.h"
#include "Mifare.h"

namespace Mifare {
	Key::Key(byte b1, byte b2, byte b3, byte b4, byte b5, byte b6){
		m_b[0] = b1;
		m_b[1] = b2;
		m_b[2] = b3;
		m_b[3] = b4;
		m_b[4] = b5;
		m_b[5] = b6;
	};

	Serial::Serial(byte b1, byte b2, byte b3, byte b4){
		m_b[0] = b1;
		m_b[1] = b2;
		m_b[2] = b3;
		m_b[3] = b4;
	}

	Serial::Serial()
	{
		m_b[0] = 0;
		m_b[1] = 0;
		m_b[2] = 0;
		m_b[3] = 0;
	}

	Serial::Serial(unsigned long lt)
	{
		m_b[0] = (lt >> 24) & 0xff;
		m_b[1] = (lt >> 16) & 0xff;
		m_b[2] = (lt >> 8) & 0xff;
		m_b[3] = (lt >> 0) & 0xff;
	}

	Key Serial::getLongWanKeyA(int index/*section*/){
		int i = index % 4;
		byte t1 = m_b[i];
		byte t2 = index;
		return Key(m_b[0], m_b[1], m_b[2], m_b[3], t1, t2);
	}

	Key Serial::getLongWanKeyB(int index){
		byte b1 = 0;
		byte b2 = 0;
		for (int i = 0; i < 4; i++) {
			b1 += m_b[i];
			b2 -= m_b[i];
		}

		return Key(b1^m_b[0], b2^m_b[1], b1^m_b[2], b2^m_b[3], b1, b2);
	}

	unsigned long Serial::getSerial()
	{
		unsigned lt = m_b[0] << 24 | m_b[1] << 16 | m_b[2] << 8 | m_b[3];
		return lt;
	}

	data_block::data_block(Key ka, Ac ac, Key kb){
		memcpy((char*)m_b, (char*)ka, 6);
		memcpy((char*)(m_b + 6), (char*)ac, 4);
		memcpy((char*)(m_b + 10), (char*)kb, 6);
	}

	data_block::data_block(byte b0, DWORD id)
	{
		memset(m_b, -1, sizeof(m_b));
		m_b[0] = b0;
		m_b[1] = ((id >> 24) & 0xff);
		m_b[2] = ((id >> 16) & 0xff);
		m_b[3] = ((id >> 8) & 0xff);
		m_b[4] = ((id >> 0) & 0xff);
	}

	data_block::data_block()
	{
		memset(m_b, -1, sizeof(m_b));
	}

	int data_block::HexString(TCHAR*buf, size_t len,int sp)
	{
		int n = 0;
		int i = 0;
		for (; n < 16 && i+2 < (int)len;){

			if (sp>0 && n>0 && n%sp == 0){
				swprintf_s(buf + i, 2, _T(" "));
				i ++;
			}

			if (i + 2 >= (int)len){
				break;
			}

			swprintf_s(buf + i, 3, _T("%02X"), m_b[n]);
			i += 2;
			n++;
		}

		return i;
	}

	int data_block::FromHexString(TCHAR*s)
	{
		size_t len = wcslen(s);
		int i = 0;
		int j = 0;
		while (i < 16 && j < (int)len){
			int ch;

			if (s[j] == ' '){
				if (++j == len)
				{
					break;
				}
			}

			if (isdigit(s[j])){
				ch = s[j] - '0';
			}
			else if (s[j] >= 'A'&&s[j] <= 'F'){
				ch = 10 + s[j] - 'A';
			}
			else if (s[j] >= 'a'&&s[j] <= 'f'){
				ch = 10 + s[j] - 'a';
			}
			else{
				return -1;
			}			

			if (++j == len){
				return -1;
			}

			ch *= 16;
			if (isdigit(s[j])){
				ch |= s[j] - '0';
			}
			else if (s[j] >= 'A'&&s[j] <= 'F'){
				ch |= 10 + s[j] - 'A';
			}
			else if (s[j] >= 'a'&&s[j] <= 'f'){
				ch |= 10 + s[j] - 'a';
			}
			else{
				return -1;
			}
			j++;

			m_b[i] = ch;
			i++;
		}

		while (i < 16){
			m_b[i++] = 0;
		}

		return i;
	}

	Key getDefaultKey(){
		return Key(-1, -1, -1, -1, -1, -1);
	}

	Ac getLongWanAC() {
		/*
		100:
		1111 0000
		1111 1111
		0000 1111
		1000 1000
		*/
		return Ac((byte)0xff, (byte)0x07, (byte)0x80, (byte)0x69);
	}

#if defined(TEST)
	bool Mifare_Test(){
		//a: -91, 80, -20, -74, -20, 10
		//b : 50, 57, 123, -33, -105, 105
		//tag : -91, 80, -20, -74
		Key ka_old(-91, 80, -20, -74, -20, 10);
		Key kb_old(50, 57, 123, -33, -105, 105);

		int index = 10;
		Serial tag(-91,80,-20,-74);
		Key ka=tag.getLongWanKeyA(index);
		Key kb = tag.getLongWanKeyB(index);

		assert(ka_old == ka && kb_old == kb);

		CString s = _T("a1a2a333347803");
		data_block db;
		db.FromHexString((TCHAR*)(LPCTSTR)s);
		TCHAR buf[256];
		db.HexString(buf, 256);
		if (s != buf){
			printf("failed\n");
			return false;
		}

		data_block *p = &db;
		unsigned char*p1 = (unsigned char*)(char*)p;
		unsigned char*p2 = (unsigned char*)p->operator LPSTR();
		unsigned char*p3 = (unsigned char*)(char*)(db);

		return true;
	}
#endif
}