#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

// UDPInterface.h: interface for the CUDPInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef yina_2010_01_22_UDPInterface_H
#define yina_2010_01_22_UDPInterface_H

#define _LINUX

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/types.h> 
#endif

#include <string>
#include <string.h>

using namespace std;


class CUDPInterface
{
public:
	int Wait(int nTimeout, int &nTimeElapse);
	int RecvFrom(unsigned char *Buf, int nBufLen, string &strSrcIP, unsigned short &usSrcPort);
	int SendTo(unsigned char *Buf, int nBufLen, string &strDstIP, unsigned short usDstPort);
	void Close(void);
	int Create(unsigned short usPort);
	inline  short GetSockFD(void){
		return m_usSocket;
	};

	inline unsigned char GetIFStatus(void){
		return m_ucIFStatus;
	};

	inline void SetIFStatus(unsigned char ucIFStatus){
		m_ucIFStatus = ucIFStatus;
	};

	CUDPInterface();
	virtual ~CUDPInterface();
	short m_usSocket;
	unsigned char m_ucIFStatus;

	enum UDP_INTERFACE_STATUS{ UDP_INTERFACE_IDLE = 0, UDP_MSG_ARRIVAL = 1 };



};

#endif // !defined(yina_2010_01_22_UDPInterface_H)
