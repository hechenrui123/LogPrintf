#pragma once
#pragma execution_character_set("utf-8")
// 本文件为utf-8 编码格式

#include <string>
#include "UDPInterface.h"

using namespace std;

// 监视目标
class MonitorObj
{
public:
	// 从字符串中读取对象的参数，可能需要重载
	void load(unsigned char*);
	int unload(unsigned char*);
	// 判断一个目标是否满足监视标准：IP相等，level大于标准，type被掩码涵盖
	int isObj(MonitorObj);
	MonitorObj(unsigned char, unsigned char, string);
	MonitorObj(unsigned char *buf);
	MonitorObj();

	// 目标描述：包括目标地址、目标类型、目标级别
	unsigned char type;
	unsigned char level;
	string IP;
};
