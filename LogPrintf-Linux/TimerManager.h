#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include <vector>
#include "TimerItem.h"

#define TIMER_NUM 10

using namespace std;

class TimerManager
{
public:
	TimerManager(void);
	~TimerManager(void);
	void UpdateTimer(int iTimeElapse);
	void SetTimer(int iTimeIndex, int iTimeout, int iRetry = 0);
	int Init(void *pAgent);

	enum{
		LOCK_TIMER = 0,
		REFRESH_TIMER = 1
	};
protected:
	void ProcessTimer(int iTimerIndex);
	vector<TimerItem *> m_TimerArray;

	void *m_pAgent;
};
