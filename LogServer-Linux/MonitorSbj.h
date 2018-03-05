#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "MonitorObj.h"
#include <vector>


// 监视主体：发起监视行为的主机
class MonitorSbj
{
public:
	// 包括自己的IP、需要观察的目标列表
	string IP;
	unsigned short port;
	vector<MonitorObj> vecMonitorObj;

	MonitorSbj(string str, unsigned short p, vector<MonitorObj> vec);

	int unload(unsigned char* buf);
};