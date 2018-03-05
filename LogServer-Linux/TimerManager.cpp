#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "TimerManager.h"
#include "LogServerAgent.h"

TimerManager::TimerManager(void)
{
}

TimerManager::~TimerManager(void)
{
}

void TimerManager::UpdateTimer(int iTimeElapse)
{
	for (int i = 0; i<TIMER_NUM; i++)
	{
		if (m_TimerArray.at(i)->IsStart())
		{
			m_TimerArray.at(i)->UpdateTimer(iTimeElapse);
			if (m_TimerArray.at(i)->IsTimeout())
				ProcessTimer(i);
		}

	}
}

int TimerManager::Init(void *pAgent, vector<MonitorSbj> *Sbjs, int ttl)
{
	m_pAgent = pAgent;
	m_MonitorSbjs = Sbjs;
	m_TimerArray.resize(TIMER_NUM);

	for (int i = 0; i<TIMER_NUM; i++)
	{
		TimerItem *pItem = new TimerItem;
		m_TimerArray.at(i) = pItem;
		m_TimerArray.at(i)->StopTimer();
	}

	TimeToLive = ttl;

	return 1;
}

void TimerManager::SetTimer(string ip)
{
	SetTimer(ip, TimeToLive);
}

void TimerManager::SetTimer(string ip, int iTimeout)
{
	vector<TimerItem *>::iterator iterTI;
	for (iterTI = m_TimerArray.begin(); iterTI != m_TimerArray.end(); iterTI ++)
	{
		if ((*iterTI)->IP == ip && (*iterTI)->IsStart() == 1)
			break;
	}
	if (iterTI == m_TimerArray.end())
	{
		// 为客户端新建一个定时器
		for (iterTI = m_TimerArray.begin(); iterTI != m_TimerArray.end(); iterTI++)
		{
			if ((*iterTI)->IsStart() == 0)
			{
				(*iterTI)->IP = ip;
				(*iterTI)->SetTimer(iTimeout);
				break;
			}
		}
		if (iterTI == m_TimerArray.end())
		{
			printf("TimerManager: SetTimer failed. No available timer.\n");
		}
	}
	else
	{
		(*iterTI)->StopTimer();
		SetTimer(ip, iTimeout);
	}
}

void TimerManager::ResetTimer(string ip)
{
	vector<TimerItem *>::iterator iterTI;
	for (iterTI = m_TimerArray.begin(); iterTI != m_TimerArray.end(); iterTI++)
	{
		if ((*iterTI)->IP == ip)
			break;
	}
	if (iterTI == m_TimerArray.end())
	{
		printf("TimerManager: Reset error. Not found.\n");
		return;
	}

	(*iterTI)->ResetTimer();

}

void TimerManager::ProcessTimer(int iTimerIndex)
{
	string ip = m_TimerArray.at(iTimerIndex)->IP;
	vector<MonitorSbj>::iterator iterSbj;
	for (iterSbj = m_MonitorSbjs->begin(); iterSbj != m_MonitorSbjs->end(); iterSbj++)
	{
		if (iterSbj->IP == ip)
			break;
	}
	// 如果没找到对应注册项，则不做处理
	if (iterSbj != m_MonitorSbjs->end())
	{
		m_MonitorSbjs->erase(iterSbj);
		const time_t t = time(NULL);
		struct tm* current_time = localtime(&t);
		printf("[%d-%d-%d %d:%d:%d]\r\n",
		current_time->tm_year + 1900,
		current_time->tm_mon + 1,
		current_time->tm_mday,
		current_time->tm_hour,
		current_time->tm_min,
		current_time->tm_sec);
		printf("TimerManager: Client connect [%s] timeout!\r\n", iterSbj->IP.data());
	}
	m_TimerArray.at(iTimerIndex)->StopTimer();
}

// 为一个定时器设置时间并启动
void TimerManager::SetTimer(int iTimeIndex, int iTimeout, int iRetry)
{
	m_TimerArray.at(iTimeIndex)->SetTimer(iTimeout, iRetry);
}

