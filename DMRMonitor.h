#pragma once

#include <string>

#if defined(_WIN32) || defined(_WIN64)
// Don't ask me why but there's some include-hell if Windows.h is included before WinSock2.h :-(
#include <WinSock2.h>
#endif

#include "Defines.h"
#include "Conf.h"
#include "DMRNetwork.h"
#include "DMRLookup.h"

using namespace std;

class CDMRMonitor
{
public:
	CDMRMonitor(const string& configFile, const bool useColorizedConsole);
	~CDMRMonitor();
	bool Init();
	void Run();
	void Stop();

private:
	string LookupDmrId(int id);
	void AnalyzeAndReportData(const CDMRData& data);

	CConf mConfig;
	IDMRNetwork* mNetwork;
	CDMRLookup* mCallsignLookup;
	
	bool mRun;
	bool mUseColorizedConsole;

	static const bool mDuplex = false;
};
