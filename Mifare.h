/*!
 * \file Mifare.h
 * \date 2016/01/16 14:47
 *
 * \author liuhuisong
 * Contact: user@company.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
#pragma once

namespace Mifare {
	class Key{
	public:
		Key(byte b1, byte b2, byte b3, byte b4, byte b5, byte b6);
		operator char*(){
			return (char*)m_b;
		};
		bool operator == (const Key t){
			return (m_b[0] == t.m_b[0] &&
				m_b[1] == t.m_b[1] &&
				m_b[2] == t.m_b[2] &&
				m_b[3] == t.m_b[3] &&
				m_b[4] == t.m_b[4] &&
				m_b[5] == t.m_b[5]);
		}
	private:
		byte m_b[6];
	};

	class Serial
	{	
	public:
		Serial();
		Serial(unsigned long);
		Serial(byte b1, byte b2, byte b3, byte b4);
		operator char*(){
			return (char*)m_b;
		}

		unsigned long getSerial();

		bool operator == (const Serial t){
			return (m_b[0] == t.m_b[0] &&
				m_b[1] == t.m_b[1] &&
				m_b[2] == t.m_b[2] &&
				m_b[3] == t.m_b[3] &&
				m_b[4] == t.m_b[4]);
		}

		Key getLongWanKeyA(int index);
		Key getLongWanKeyB(int index);
	private:
		byte m_b[4];
	};
	
	typedef Serial Ac;

	class data_block{
	public:
		data_block();
		data_block(byte b0, DWORD id);
		data_block(Key ka, Ac ac, Key kb);
		operator char*(){
			return (char*)m_b;
		}

		bool operator ==(data_block b){
			return (memcmp((char*)b, (char*)b, 16) == 0);
		}

		int HexString(TCHAR*buf, size_t len,int sp=0);
		int FromHexString(TCHAR*s);
	private:
		//byte t;
		byte m_b[16];
	};

	Key getDefaultKey();
	Ac getLongWanAC();

#if defined(TEST)
	bool Mifare_Test();
#endif
};