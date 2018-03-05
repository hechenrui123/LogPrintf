#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "LogMessage.h"


LogMessage::LogMessage(string m, MonitorObj *obj)
{
	msg = m;
	m_MonitorObj = obj;
}
