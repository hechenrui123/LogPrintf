#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "TimerItem.h"

TimerItem::TimerItem(void)
{
	m_iTimeout = 0;
	m_iRetry = 0;
	m_iIsStart = 0;
}

TimerItem::~TimerItem(void)
{
}
void TimerItem::UpdateTimer(int iTimeElapse)
{
	m_iTimeout -= iTimeElapse;
}
void TimerItem::SetTimer(int iTimeout, int iRetry)
{
	m_iTimeout = iTimeout;
	m_iTimeoutInit = iTimeout;
	m_iRetry = iRetry;
	m_iRetryInit = iRetry;
	m_iIsStart = 1;
}

void TimerItem::StopTimer(void)
{
	m_iIsStart = 0;
}

int TimerItem::IsTimeout(void)
{
	if (m_iTimeout <= 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int TimerItem::IsStart(void)
{
	return m_iIsStart;
}

void TimerItem::ResetTimer(void)
{
	m_iTimeout = m_iTimeoutInit;
	m_iRetry = m_iRetryInit;
	m_iIsStart = 1;
}
