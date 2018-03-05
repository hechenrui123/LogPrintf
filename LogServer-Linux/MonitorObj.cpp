#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "MonitorObj.h"
// #include <WinSock2.h>


// return 0： 满足条件
int MonitorObj::isObj(MonitorObj standard)
{
	if ((type & standard.type) == 0)
		return 1;
	if (level < standard.level)
		return 2;
	if (strcmp(IP.data(), standard.IP.data()) != 0)
		return 3;
	return 0;
}

// 那对象MonitorObj转化为字符串
int MonitorObj::unload(unsigned char *buf)
{
	buf[0] = type;
	buf[1] = level;
	unsigned int intIP = inet_addr(IP.data());
	intIP = ntohl(intIP);
	unsigned char *p = (unsigned char*)  &intIP;
	buf[2] = p[0];
	buf[3] = p[1];
	buf[4] = p[2];
	buf[5] = p[3];
	return 6;
}

// 把字符串转化为MonitorObj对象
void MonitorObj::load(unsigned char *buf)
{
	type = buf[0];
	level = buf[1];
	// in_addr 为网络字节序，buf直接从网络上收到，所以不需要转换
	in_addr intIP = *((in_addr*) (buf+2));
	IP = inet_ntoa(intIP);
}

MonitorObj::MonitorObj(unsigned char t, unsigned char l, string s)
{
	type = t;
	level = l;
	IP = s;
}

MonitorObj::MonitorObj(unsigned char *buf)
{
	this->load(buf);
}
