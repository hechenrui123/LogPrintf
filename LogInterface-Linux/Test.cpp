#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include "LogPrintInterface.h"

int main(int argc, char* argv[])
{
	int a = 31, b = 92;

	LogPrintInterface *p = new LogPrintInterface();

	while(1)
	{
		for (int j = 0; j < 5; j++) {
			// 设置为j-type，j-level
			p->type = (0x01 << j);
			p->level = 0x10 * ( j + 1);
			for (int i = 0; i < 2; i++)
			{
				printf("Send Log Info: TYPE:%x LEVEL:%x  ", p->type, p->level);
				p->LogPrint("Debug Info: a=%d, b=%d", a - i * 7, b - i * 3);
				printf("Debug Info : a = %d, b = %d\r\n", a - i * 7, b - i * 3);
			}
				
		}
		sleep(5);
	}
	
	
	system("pause");
}