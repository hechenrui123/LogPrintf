#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "LogClientAgent.h"

int main()
{
	LogClientAgent *clientAgent = new LogClientAgent();

	clientAgent->Init();
	clientAgent->Run();

	system("pause");
}