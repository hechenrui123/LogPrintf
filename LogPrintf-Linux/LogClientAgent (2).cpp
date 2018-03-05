#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#define _CRT_SECURE_NO_WARNINGS

#include "LogClientAgent.h"
#include "inifile.h"
// #include <WinSock2.h>
#include <time.h>
#include "UDPInterface.h"


#define INI_FILE_NAME "./client.ini"

void LogClientAgent::ProcessMessage()
{
	unsigned char buf[65536];
	string strSrcIP;
	unsigned short usSrcPort;

	int iLen = m_UDPInterface.RecvFrom(buf, 65536, strSrcIP, usSrcPort);
	if (iLen <= 0)
		return;
	// printf("LogClient: received length: %d\n", iLen);

	buf[iLen] = 0;
	if (buf[0] == 0)
	{
		// 普通调试信息
		unsigned char type = buf[1];
		unsigned char level = buf[2];
		in_addr intIP = *((in_addr *)(buf + 3));
		string strIP = inet_ntoa(intIP);
		string msg = (char *) buf + 7;
#ifdef WIN32
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		printf("[%d-%02d-%02d %02d:%02d:%02d]\n",
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond);
#else
		const time_t t = time(NULL);
		struct tm* current_time = localtime(&t);
		printf("[%d-%d-%d %d:%d:%d]\r\n",
        current_time->tm_year + 1900,
        current_time->tm_mon + 1,
        current_time->tm_mday,
        current_time->tm_hour,
        current_time->tm_min,
        current_time->tm_sec);
#endif

		printf("Log Message from %s: TYPE:%x LEVEL:%x\n", strIP.data(), type, level);
		printf("==> %s\n\n", msg.data());
	}
	else if (buf[0] == 1)
	{
		// 控制信息
		printf("Control Message from server: %x %x %x \n", buf[0], buf[1], buf[2]);
		if (iLen > 2 && buf[1] == 0 && buf[2] == 0)
			printf("Register successfully.\n");
	}
}

void LogClientAgent::Run()
{
	int iTimeElapse;
	while (true)
	{
		if (m_UDPInterface.Wait(1000, iTimeElapse))
		{
			// printf("the time elapse: %d\n", iTimeElapse);
			ProcessMessage();
		}
		// 处理定时器，时间到了重新注册
		m_TimerManager.UpdateTimer(iTimeElapse);
	}
}

int LogClientAgent::Init()
{
	readIniFile();

	for (int i = 0; i < 10; i++)
	{
		if (m_UDPInterface.Create(m_clientPort))
		{
			printf("LogClient: Init socket error at %d!\n", m_clientPort);
			m_clientPort++;
		}
		else
		{
			printf("LogClient: Init socket successfullly at %d!\n", m_clientPort);
			break;
		}
	}

	regst();

	m_TimerManager.Init(this);
	m_TimerManager.SetTimer(0, TimeToLive);

	return 0;
}

void LogClientAgent::regst()
{
	unsigned char buf[1024] = {0x01, 0x00};

	MonitorSbj *pTmpSbj = new MonitorSbj("127.0.0.1", m_clientPort, m_MonitorObjs);
	int buflen = pTmpSbj->unload(buf + 2) + 2;

	m_UDPInterface.SendTo(buf, buflen, m_serverIP, m_serverPort);
}

int LogClientAgent::readIniFile()
{
	char buf[255];

	// sector [Server]
	m_serverPort = (unsigned short)read_profile_int("Server", "ServerPort", 0, INI_FILE_NAME);
	if (m_serverPort == 0)
	{
		printf("LogClient: Read ServerPort from client.ini failed!\n");
		return -1;
	}

	if (!read_profile_string("Server", "IP", buf, 16, "127.0.0.1", INI_FILE_NAME))
	{
		printf("LogClient: Read Server IP from client.ini failed!\n");
		return -1;
	}
	m_serverIP = buf;

	// sector [Client]
	m_clientPort = (unsigned short)read_profile_int("Client", "Port", 0, INI_FILE_NAME);
	if (m_clientPort == 0)
	{
		printf("LogClient: Read ClientPort from client.ini failed!\n");
		return -1;
	}

	TimeToLive = read_profile_int("Client", "LiveTime", 0, INI_FILE_NAME);
	if (TimeToLive == 0)
	{
		printf("LogClient: Read TimeToLive from client.ini failed!\n");
		return -1;
	}

	objnum = read_profile_int("Client", "objnum", 0, INI_FILE_NAME);
	if (objnum == 0)
	{
		printf("LogClient: Read objnum from client.ini failed!\n");
		return -1;
	}

	// sector [Obj0] [Obj1] ......
	char secname[12];
	for (int i = 0; i < objnum; i++)
	{
		MonitorObj *pTmpObj = new MonitorObj();
		sprintf(secname, "Obj%d", i);

		if (!read_profile_string(secname, "IP", buf, 16, "127.0.0.1", INI_FILE_NAME))
		{
			printf("LogClient: Read Obj IP from client.ini failed!\n");
			return -1;
		}
		pTmpObj->IP = buf;

		pTmpObj->type = (unsigned char)read_profile_int(secname, "TYPE", 0, INI_FILE_NAME);
		pTmpObj->level = (unsigned char)read_profile_int(secname, "LEVEL", 0, INI_FILE_NAME);
		
		m_MonitorObjs.push_back(*pTmpObj);
	}
}
