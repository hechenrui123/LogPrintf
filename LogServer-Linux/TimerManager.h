#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include <vector>
using namespace std;
#include "TimerItem.h"
#include "MonitorSbj.h"

#define TIMER_NUM 100

class TimerManager
{
public:
	TimerManager(void);
	~TimerManager(void);
	void UpdateTimer(int iTimeElapse);
	void SetTimer(int iTimeIndex, int iTimeout, int iRetry = 0);
	// 重载一个绑定clientIP的定时器设置函数
	void SetTimer(string ip, int iTimeout);
	void SetTimer(string ip);
	void ResetTimer(string ip);
	int Init(void *pAgent, vector<MonitorSbj> *, int);

	enum{
		LOCK_TIMER = 0,
		REFRESH_TIMER = 1
	};
protected:
	int TimeToLive;
	void ProcessTimer(int iTimerIndex);
	vector<TimerItem *> m_TimerArray;

	void *m_pAgent;
	vector<MonitorSbj> *m_MonitorSbjs;
};
