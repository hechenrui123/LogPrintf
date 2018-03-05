#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#define _CRT_SECURE_NO_WARNINGS

#include "LogServerAgent.h"

using namespace std;

int main()
{
	LogServerAgent *pAgent = new LogServerAgent();

	pAgent->Init();

	pAgent->Run();

	system("pause");
}