#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "FunctionManager.h"

// #include <winsock2.h>

int FunctionManager::Init(vector<MonitorSbj>* vec, CUDPInterface* udp, TimerManager *tmm)
{
	m_MonitorSbjs = vec;
	m_UDPInterface = udp;
	m_TimerManager = tmm;

	return 0;
}

void FunctionManager::query(string ip)
{
	unsigned char buf[1024] = { 0x01, 0x10 };

	// 先查询列表中含有该项
	vector<MonitorSbj>::iterator iterSbj;
	for (iterSbj = m_MonitorSbjs->begin(); iterSbj != m_MonitorSbjs->end(); iterSbj++)
	{
		if (iterSbj->IP == ip)
			break;
	}
	// 查询对象不存在,没有注册端口，无法回复信息
	if (iterSbj == m_MonitorSbjs->end())
	{
		printf("FuncitonManager: query error. not found.\n");
	}
	else
	{
		// 装载从 PORT,NUM字段开始
		int buflen = iterSbj->unload(buf+2);
		m_UDPInterface->SendTo(buf, buflen, ip, iterSbj->port);
	}

}

void FunctionManager::remove(string ip)
{
	unsigned char buf[3] = {0x01, 0x20, 0x00};

	// 先查询列表中含有该项
	vector<MonitorSbj>::iterator iterSbj;
	for (iterSbj = m_MonitorSbjs->begin(); iterSbj != m_MonitorSbjs->end(); iterSbj++)
	{
		if (iterSbj->IP == ip)
			break;
	}
	if (iterSbj != m_MonitorSbjs->end())
	{
		m_MonitorSbjs->erase(iterSbj);
		m_UDPInterface->SendTo(buf, 4, ip, iterSbj->port);

		const time_t t = time(NULL);
		struct tm* current_time = localtime(&t);
		printf("[%d-%d-%d %d:%d:%d]\r\n",
		current_time->tm_year + 1900,
		current_time->tm_mon + 1,
		current_time->tm_mday,
		current_time->tm_hour,
		current_time->tm_min,
		current_time->tm_sec);
		printf("FunctionManger: Contor Message(len=3) [remove] sendto %s.\r\n", ip.data());
		printf("==> %x %x %x\r\n", buf[0], buf[1], buf[2]);
	}
	else
	{
		printf("FunctionManager: remove error. not register.\n");
	}
	

}

void FunctionManager::addToMonitorSbjs(string ip, unsigned char* msg, int buflen)
{
	// msg 以字段PORT， NUM开始
	string IP = ip;
	unsigned short port = *(unsigned short*) msg;
	port = htons(port);
	unsigned short numObj = *(unsigned short*) (msg + 2);
	numObj = htons(numObj);

	// 读取监听对象列表
	vector<MonitorObj> tmpObjs;
	unsigned char *buf = msg + 4;
	for (int i = 0; i < numObj; i++)
	{
		if (4 + i * 6 >buflen)
			break;
		// 读四字节IP地址
		MonitorObj *pTmpObj = new MonitorObj(buf + i*6);
		tmpObjs.push_back(*pTmpObj);
	}

	MonitorSbj *pTmpSbj = new MonitorSbj(IP, port, tmpObjs);
	m_MonitorSbjs->push_back(*pTmpSbj);

	const time_t t = time(NULL);
	struct tm* current_time = localtime(&t);
	printf("[%d-%d-%d %d:%d:%d]\r\n",
	current_time->tm_year + 1900,
	current_time->tm_mon + 1,
	current_time->tm_mday,
	current_time->tm_hour,
	current_time->tm_min,
	current_time->tm_sec);
	printf("FunctionManger: add success for (%s,%d).\r\n", IP.data(), port);
}

void FunctionManager::regst(string ip, unsigned char* msg, int buflen)
{
	// 先查询列表中含有该项
	vector<MonitorSbj>::iterator iterSbj;
	for (iterSbj = m_MonitorSbjs->begin(); iterSbj != m_MonitorSbjs->end(); iterSbj++)
	{
		if (strcmp(iterSbj->IP.data(), ip.data()) == 0)
			break;
	}


	if (iterSbj != m_MonitorSbjs->end())
	{
		// 列表汇总含有申请项
		m_MonitorSbjs->erase(iterSbj);
	}
	addToMonitorSbjs(ip, msg, buflen);
	m_TimerManager->SetTimer(ip);

	// 注册成功的回复
	unsigned char buf[3] = {0x01, 0x00, 0x00};
	unsigned short port = *((unsigned short *)msg);
	m_UDPInterface->SendTo(buf, 3, ip, htons(port));

	const time_t t = time(NULL);
	struct tm* current_time = localtime(&t);
	printf("[%d-%d-%d %d:%d:%d]\r\n",
	current_time->tm_year + 1900,
	current_time->tm_mon + 1,
	current_time->tm_mday,
	current_time->tm_hour,
	current_time->tm_min,
	current_time->tm_sec);
	printf("FunctionManger: Control Message(len=3)[regst] sendto %s.\r\n", ip.data());
	printf("==> %x %x %x\r\n", buf[0], buf[1], buf[2]);
		
}

void FunctionManager::process(unsigned char* buf, int buflen, string IP)
{

	const time_t t = time(NULL);
	struct tm* current_time = localtime(&t);
	printf("[%d-%d-%d %d:%d:%d]\r\n",
	current_time->tm_year + 1900,
	current_time->tm_mon + 1,
	current_time->tm_mday,
	current_time->tm_hour,
	current_time->tm_min,
	current_time->tm_sec);
	printf("FunctionManager: Control Message from %s.\n", IP.data());
	printf("==> ");
	for(int i=0; i<buflen; i++)
		printf("%x", buf[i]);
		printf("\r\n");


	switch (buf[0])
	{
	// 注册, 字段：PORT, NUM, TYPEi, LEVELi, IPi
	case 0x00:
		regst(IP, buf + 1, buflen - 1);
		break;
	// 查询
	case 0x10:
		query(IP);
		break;
	//删除
	case 0x20:
		remove(IP);
		break;
	default:
		break;
	}
}
