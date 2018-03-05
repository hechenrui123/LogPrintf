#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "UDPInterface.h"
#include "MonitorSbj.h"

#include <string>

using namespace std;

class LogMessage
{
public:
	LogMessage(string, MonitorObj*);

	MonitorObj *m_MonitorObj;
	string msg;

};