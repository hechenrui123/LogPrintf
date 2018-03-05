#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式]

#include "DispatchFilter.h"

int DispatchFilter::Init(vector<MonitorSbj>* vec, CUDPInterface* udp)
{
	m_monitorSbjs = vec;
	m_UDPInterface = udp;

	return 0;
}

int DispatchFilter::dispatch(LogMessage msg, MonitorSbj sbj, string ip)
{
	unsigned short port = sbj.port;
	string IP = ip;

	// MSG, TYPE, LEVEL, STRING
	unsigned char buf[65536];
	buf[0] = 0;
	buf[1] = msg.m_MonitorObj->type;
	buf[2] = msg.m_MonitorObj->level;
	
	unsigned int intIP = inet_addr(IP.data());
	intIP = ntohl(intIP);
	unsigned char *p = (unsigned char*)&intIP;
	for (int i = 0; i < 4; i++)
		buf[3 + i] = p[i];

	string tmpMsg = msg.msg;
	for (int i = 0; i < strlen(tmpMsg.data()); i++)
		buf[i + 7] = tmpMsg.data()[i];
	
	int buflen = strlen(tmpMsg.data()) + 7;
	buf[buflen] = 0;
	buflen++;

	return m_UDPInterface->SendTo(buf, buflen, sbj.IP, port);
}

void DispatchFilter::process(unsigned char* buf, int buflen, string IP)
{
	buf[buflen] = 0;
	MonitorObj *pTmpObj = new MonitorObj(buf[0], buf[1], IP);
	LogMessage *pTmpMessage = new LogMessage((char*)(buf+2), pTmpObj);

	vector<MonitorSbj>::iterator iterSjb;
	vector<MonitorObj>::iterator iterObj;

	const time_t t = time(NULL);
	struct tm* current_time = localtime(&t);
	printf("[%d-%d-%d %d:%d:%d]\r\n",
	current_time->tm_year + 1900,
	current_time->tm_mon + 1,
	current_time->tm_mday,
	current_time->tm_hour,
	current_time->tm_min,
	current_time->tm_sec);

	printf("DisPatchFilter: Log Message from %s.\n", IP.data());
	printf("==> %s\n\n", pTmpMessage->msg.data());

	for (iterSjb = m_monitorSbjs->begin(); iterSjb != m_monitorSbjs->end(); iterSjb++)
	{
		for (iterObj = iterSjb->vecMonitorObj.begin(); iterObj != iterSjb->vecMonitorObj.end(); iterObj++)
		{
			// 收到信息pTmpObj，是否满足要求iterObj
			if (!pTmpObj->isObj(*iterObj))
			{
				// 如果满足，则继续处理下个监听主体itersbj
				printf("DispatchFilter: for %s\r\n", iterSjb->IP.data());
				dispatch(*pTmpMessage, *iterSjb, IP);
				break;
			}
		}
	}
}
