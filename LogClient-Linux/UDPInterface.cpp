#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

// UDPInterface.cpp: implementation of the CUDPInterface class.
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>

#include "UDPInterface.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUDPInterface::CUDPInterface()
{

}

CUDPInterface::~CUDPInterface()
{

}

int CUDPInterface::Create(unsigned short usPort)
{
	struct sockaddr_in	addr;
#ifdef  WIN32
#ifndef WIN32_SOCK_INIT
#define WIN32_SOCK_INIT
	WSADATA       wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
	{
		fprintf(stderr, "WSAStartup failed: %d\n", GetLastError());
		return -1;
	}
#endif
#endif

	m_usSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_usSocket<0)
	{
		fprintf(stderr, "udp socket init error.\n");
		return -1;
	}

	if (usPort == 0)
		return 0;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(usPort);

	if (bind(m_usSocket, (struct sockaddr *)&addr, sizeof(addr))<0)
	{
		fprintf(stderr, "udp socket bind error.\n");
		return -1;
	}

	return 0;

}

void CUDPInterface::Close()
{
#ifdef WIN32
	closesocket(m_usSocket);
#else
	close(m_usSocket);
#endif

	return;
}

int CUDPInterface::SendTo(unsigned char *Buf, int nBufLen, string &strDstIP, unsigned short usDstPort)
{
	struct sockaddr_in DstAddr;

	DstAddr.sin_family = AF_INET;
	DstAddr.sin_port = htons(usDstPort);
	DstAddr.sin_addr.s_addr = inet_addr(strDstIP.c_str());

	return sendto(m_usSocket, (char *)Buf, nBufLen, 0, (struct sockaddr *)&DstAddr, sizeof(DstAddr));
}

int CUDPInterface::RecvFrom(unsigned char *Buf, int nBufLen, string &strSrcIP, unsigned short &usSrcPort)
{
	int nLen;
	struct sockaddr_in from_addr;
#ifndef _LINUX		 
	int	from_addr_len = sizeof(struct sockaddr);
#else 	
	socklen_t from_addr_len = sizeof(struct sockaddr);
#endif

	nLen = recvfrom(m_usSocket, (void *)Buf, nBufLen, 0, (struct sockaddr *)&from_addr, &from_addr_len);

	if (nLen>0)
	{
		strSrcIP = inet_ntoa(from_addr.sin_addr);
		usSrcPort = ntohs(from_addr.sin_port);
	}

	return nLen;
}

int CUDPInterface::Wait(int nTimeout, int &nTimeElapse)
{
	int nResult;
	fd_set ReadFDs;
	struct timeval TimeVal;

	FD_ZERO(&ReadFDs);
	FD_SET((unsigned int)m_usSocket, &ReadFDs);

	TimeVal.tv_sec = nTimeout / 1000;
	TimeVal.tv_usec = 1000 * (nTimeout % 1000);

	nResult = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
	if (nResult == -1)
	{
		fprintf(stderr, "udp socket select error.\n");
	}
	else
	{
		if (FD_ISSET(m_usSocket, &ReadFDs))
		{
#ifdef WIN32
			nTimeElapse = nTimeout - (1000 * TimeVal.tv_sec + TimeVal.tv_usec / 1000);
#else 
			// nTimeElapse = nTimeout;  [hcr] 2018-01-23 edit for _LINUX
			nTimeElapse = nTimeout - (1000 * TimeVal.tv_sec + TimeVal.tv_usec / 1000);
#endif
			nResult = 1;
		}
		else
		{
			nTimeElapse = nTimeout;
			nResult = 0;
		}
	}

	return nResult;
}
