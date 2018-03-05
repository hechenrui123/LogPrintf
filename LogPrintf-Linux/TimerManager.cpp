#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "TimerManager.h"
#include "LogClientAgent.h"


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

int TimerManager::Init(void *pAgent)
{
	m_pAgent = pAgent;
	m_TimerArray.resize(TIMER_NUM);

	for (int i = 0; i<TIMER_NUM; i++)
	{
		TimerItem *pItem = new TimerItem;
		m_TimerArray.at(i) = pItem;
		m_TimerArray.at(i)->StopTimer();
	}

	return 1;

}
void TimerManager::ProcessTimer(int iTimerIndex)
{
	// 再次向服务器注册
	if (iTimerIndex == 0)
	{
		m_TimerArray.at(iTimerIndex)->ResetTimer();
		((LogClientAgent*)m_pAgent)->regst();
	}
}

void TimerManager::SetTimer(int iTimeIndex, int iTimeout, int iRetry)
{
	m_TimerArray.at(iTimeIndex)->SetTimer(iTimeout, iRetry);
}

