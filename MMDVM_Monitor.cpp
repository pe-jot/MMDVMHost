#include <iostream>
#include "DMRMonitor.h"

#if !defined(_WIN32) && !defined(_WIN64)
#include <signal.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
const char* DEFAULT_INI_FILE = "MMDVM_Monitor.ini";
#define COLORIZED_CONSOLE 0
#else
const char* DEFAULT_INI_FILE = "/etc/MMDVM_Monitor.ini";
#define COLORIZED_CONSOLE 1
#endif


static CDMRMonitor *monitor = NULL;


#if defined(_WIN32) || defined(_WIN64)
BOOL WINAPI ctrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	// See https://docs.microsoft.com/en-us/windows/console/handlerroutine
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		if (monitor)
		{
			monitor->Stop();
		}
		return true;

	default:
		return false;
	}
}
#else
static void signalHandler(int signum)
{
	if (monitor)
	{
		monitor->Stop();
	}
}
#endif


int main(int argc, char** argv)
{
#if defined(_WIN32) || defined(_WIN64)
	if (!SetConsoleCtrlHandler(ctrlHandler, true))
	{
		return -1;
	}
#else
	::signal(SIGINT, signalHandler);
	::signal(SIGTERM, signalHandler);
	::signal(SIGHUP, signalHandler);
#endif

	monitor = new CDMRMonitor(DEFAULT_INI_FILE, COLORIZED_CONSOLE);
	bool initSuccess = monitor->Init();

	if (initSuccess)
	{
		monitor->Run();
	}
	
	delete monitor;
	monitor = NULL;

	return 0;		
}
