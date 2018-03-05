#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "LogServerAgent.h"
#include "inifile.h"

#define INI_FILE_NAME "./server.ini"

void LogServerAgent::ProcessMessage()
{
	unsigned char buf[65536];
	string strSrcIP;
	unsigned short usSrcPort;

	int iLen = m_UDPInterface.RecvFrom(buf, 65536, strSrcIP, usSrcPort);
	// printf("received length: %d\n", iLen);

	switch (buf[0])
	{
	// 普通调试信息, 来自Device：MSG,TYPE,LEVEL,STRING
	case 0: m_DispatchFilter.process(buf + 1, iLen-1, strSrcIP);
			break;
	// 控制信息, 来自Client, MSG, OP, PORT, NUM, ...
	case 1: m_FunctionManager.process(buf + 1, iLen-1, strSrcIP);
			break;
	default: printf("Recv frame error\n");
	break;
	}
}

void LogServerAgent::Run()
{
	int iTimeElapse;
	while (true)
	{
		if (m_UDPInterface.Wait(1000, iTimeElapse))
		{
			// printf("the time elapse: %d\n", iTimeElapse);
			ProcessMessage();
		}
		m_TimerManager.UpdateTimer(iTimeElapse);
	}
}

int LogServerAgent::Init()
{
	m_Port = (unsigned short)read_profile_int("Server", "Port", 0, INI_FILE_NAME);
	if (m_Port == 0)
	{
		printf("LogServer: Read Port from server.ini failed!\n");
		return -1;
	}

	TimeToLive = read_profile_int("Server", "LiveTime", 0, INI_FILE_NAME);
	if (TimeToLive == 0)
	{
		printf("LogServer: Read TimeToLive from server.ini failed!\n");
		return -1;
	}

	if (m_UDPInterface.Create(m_Port))
	{
		printf("LogServer: Init socket error!\n");
		return -1;
	}
	printf("LogServer: Init socket successfully!\n");
	printf("LogServer: UDP Listen Port = %u\n", m_Port);

	m_TimerManager.Init((void *)this, &m_MonitorSbjs, TimeToLive);
	m_FunctionManager.Init(&m_MonitorSbjs, &m_UDPInterface, &m_TimerManager);
	m_DispatchFilter.Init(&m_MonitorSbjs, &m_UDPInterface);
	printf("LogServer: Init LogServer Agent successfully!\n");

	return 0;
}