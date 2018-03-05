#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "TimerManager.h"
#include "UDPInterface.h"
#include "MonitorSbj.h"


class LogClientAgent
{
public:
	// 初始化socket、监听主体
	int Init();
	// 大循环等待输入事件，进行处理：ProcessMessage、UpdateTimer
	void Run();
	// device：调用dispatch模块进行转发操作；client：调用FunctionManager维护vector<MonitorSbj>
	void ProcessMessage();

	void regst();
private:
	string m_serverIP;
	unsigned short m_serverPort, m_clientPort;
	int TimeToLive, objnum;
	TimerManager m_TimerManager;
	CUDPInterface m_UDPInterface;
	vector<MonitorObj> m_MonitorObjs;

	int readIniFile();
};