#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "LogMessage.h"

class DispatchFilter
{
public:
	int Init(vector<MonitorSbj>*, CUDPInterface*);
	int dispatch(LogMessage, MonitorSbj, string);
	void process(unsigned char* buf, int buflen, string IP);
private:
	vector<MonitorSbj> *m_monitorSbjs;
	CUDPInterface *m_UDPInterface;
};