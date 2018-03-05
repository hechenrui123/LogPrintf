#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

class TimerItem
{
public:
	TimerItem(void);
	~TimerItem(void);
	void UpdateTimer(int iTimeElapse);
	void SetTimer(int iTimeout, int iRetry = 0);
	void StopTimer(void);
	int IsTimeout(void);
	int IsStart(void);
protected:
	int m_iTimeout;
	int m_iTimeoutInit;
	int m_iRetry;
	int m_iRetryInit;
	int m_iIsStart;
public:
	void ResetTimer(void);
};
