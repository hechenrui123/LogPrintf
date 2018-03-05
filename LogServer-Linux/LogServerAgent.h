#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "TimerManager.h"
#include "FunctionManager.h"
#include "DispatchFilter.h"

class LogServerAgent
{
public:
	// 初始化socket、监听主体
	int Init();
	// 大循环等待输入事件，进行处理：ProcessMessage、UpdateTimer
	void Run();
	// device：调用dispatch模块进行转发操作；client：调用FunctionManager维护vector<MonitorSbj>
	void ProcessMessage();

private:
	unsigned short m_Port;
	int TimeToLive;
	vector<MonitorSbj> m_MonitorSbjs;
	TimerManager m_TimerManager;
	FunctionManager m_FunctionManager;
	CUDPInterface m_UDPInterface;
	DispatchFilter m_DispatchFilter;
};
