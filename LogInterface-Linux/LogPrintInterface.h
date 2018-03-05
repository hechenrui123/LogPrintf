#include "UDPInterface.h"
#include "inifile.h"

using namespace std; 

class LogPrintInterface
{
public:
	int Init();
	int LogPrint(char *fmt, ...);
	// 1.sprintf(buf, *fmt, ...), 2.buf=type+level+buf, 3.sendto()
	LogPrintInterface();
	void setInfo(unsigned char, unsigned char);

	unsigned char type;
	unsigned char level;

private:
	string destIP;
	unsigned short destPort;
	CUDPInterface m_UDPInterface;

};