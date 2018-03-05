#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#define _CRT_SECURE_NO_WARNINGS
#define INI_FILE_NAME "./device.ini"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "LogPrintInterface.h"
#include "inifile.h"

void LogPrintInterface::setInfo(unsigned char t, unsigned char l)
{
	type = t;
	level = l;
}

int LogPrintInterface::LogPrint(char *format, ...)
{
	// 把变长参数传递到sprintf
	char str[65536];
	va_list vl;
	va_start(vl, format);
	vsprintf(str, format, vl);
	va_end(vl);

	// 打上msg、type、level标记
	unsigned char buf[65536];
	buf[0] = 0;  buf[1] = type; buf[2] = level;
	for (int i = 0; i < strlen(str)+1; i++)
		buf[3 + i] = (unsigned char) str[i];

	printf((char*)buf);

	// 发送到服务器
	m_UDPInterface.SendTo((unsigned char *)buf, strlen(str) + 3, destIP, destPort);

	return 0;
}

int LogPrintInterface::Init()
{
	type = 0x01;
	level = 0x10;

	destPort = (unsigned short)read_profile_int("Server", "ServerPort", 0, INI_FILE_NAME);
	if (destPort == 0)
	{
		printf("LogPrintServer: Read Listen Port from device.ini failed!\n");
		return -1;
	}
	char buf[255];
	if (!read_profile_string("Server", "IP", buf, 16, "127.0.0.1", INI_FILE_NAME))
	{
		printf("LogPrintServer: Read Server IP from device.ini failed!\n");
		return -1;
	}
	destIP = buf;

	for (int i = 1; i < 10; i++) 
		if (m_UDPInterface.Create(destPort + i) == 0)
			break;

	return 0;
}

LogPrintInterface::LogPrintInterface()
{
	this->Init();
}