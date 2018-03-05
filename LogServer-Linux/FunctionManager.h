#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "MonitorSbj.h"
#include "UDPInterface.h"
#include "TimerManager.h"

class FunctionManager{
public:
	// 注册，查询，删除函数
	void regst(string ip, unsigned char* msg, int buflen);
	void query(string ip);
	void remove(string ip);
	void process(unsigned char* buf, int buflen, string IP);
	void addToMonitorSbjs(string ip, unsigned char* msg, int buflen);
	
	int Init(vector<MonitorSbj>*, CUDPInterface*, TimerManager*);
private:
	CUDPInterface *m_UDPInterface;
	// 包含了一个监视主体的全部信息
	vector<MonitorSbj> *m_MonitorSbjs;
	TimerManager *m_TimerManager;

};
