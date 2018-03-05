#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "MonitorSbj.h"

MonitorSbj::MonitorSbj(string str, unsigned short p, vector<MonitorObj> vec) {
	IP = str;
	port = p;
	vecMonitorObj = vec;
}

// 字段依次为： PORT,NUM, ...
int MonitorSbj::unload(unsigned char* buf) {

	unsigned short tmpport = ntohs(port);
	unsigned char* p = (unsigned char*)&tmpport;
	buf[0] = p[0]; buf[1] = p[1];

	unsigned short pos = 0;
	vector<MonitorObj>::iterator iterObj;
	for (iterObj = vecMonitorObj.begin(); iterObj != vecMonitorObj.end(); iterObj++)
	{
		iterObj->unload(buf + 4 + pos * 6);
		pos++;
	}

	int buflen = 4 + pos * 6;
	pos = htons(pos);
	p = (unsigned char*)&pos;
	buf[2] = p[0]; buf[3] = p[1];
	return buflen;
}