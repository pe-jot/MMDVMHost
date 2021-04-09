#include "DMRMonitor.h"
#include "DMRDirectNetwork.h"
#include "Version.h"
#include "StopWatch.h"
#include "ConsoleCodes.h"
#include <string>

CDMRMonitor::CDMRMonitor(const string& configFile, const bool useColorizedConsole) :
	mConfig(configFile),
	mNetwork(nullptr),
	mCallsignLookup(nullptr),
	mRun(true),
	mUseColorizedConsole(useColorizedConsole)
{
}

CDMRMonitor::~CDMRMonitor()
{
	if (mNetwork)
	{
		delete mNetwork;
		mNetwork = NULL;
	}

	if (mCallsignLookup)
	{
		delete mCallsignLookup;
		mCallsignLookup = NULL;
	}

	if (mUseColorizedConsole)
	{
		LogInfo(CONSOLE_SHOW_CURSOR);
	}
	LogFinalise();
}

bool CDMRMonitor::Init()
{
	if (!mConfig.read())
	{
		fprintf(stderr, "MMDVM Monitor: cannot read the .ini file\n");
		return false;
	}

	LogInitialise(
		false,
		mConfig.getLogFilePath(),
		mConfig.getLogFileRoot(),
		mConfig.getLogFileLevel(),
		mConfig.getLogDisplayLevel(),
		mConfig.getLogFileRotate()
	);

	if (mUseColorizedConsole && mConfig.getLogFileLevel() > 0)
	{
		// Don't use colorized console if logging to file is enabled to avoid messed up logfile texts
		LogWarning("Disabling colorized console due to log file level setting!");
		mUseColorizedConsole = 0;
	}
	
	if (mUseColorizedConsole)
	{
		LogInfo(CONSOLE_HIDE_CURSOR);
		LogInfo(CONSOLE_CLEARSCREEN);
	}

	string lookupFile = mConfig.getDMRIdLookupFile();
	if (!lookupFile.empty())
	{
		LogInfo("Loading DMR ID lookup data");
		// We don't cyclically reload the lookup file
		mCallsignLookup = new CDMRLookup(lookupFile, 0);
		mCallsignLookup->read();
	}

	LogInfo("Initializing DMR Network");

	mNetwork = new CDMRDirectNetwork(
		mConfig.getDMRNetworkAddress(),
		mConfig.getDMRNetworkPort(),
		mConfig.getDMRNetworkLocal(),
		mConfig.getDMRId(),
		mConfig.getDMRNetworkPassword(),
		mDuplex,
		VERSION,
		mConfig.getDMRNetworkSlot1(),
		mConfig.getDMRNetworkSlot2(),
		HW_TYPE::HWT_UNKNOWN,
		mConfig.getDMRNetworkDebug()
	);

	mNetwork->setConfig(
		mConfig.getCallsign(),
		mConfig.getRXFrequency(),
		mConfig.getTXFrequency(),
		mConfig.getPower(),
		mConfig.getDMRColorCode(),
		mConfig.getLatitude(),
		mConfig.getLongitude(),
		mConfig.getHeight(),
		mConfig.getLocation(),
		mConfig.getDescription(),
		mConfig.getURL()
	);

	string options = mConfig.getDMRNetworkOptions();
	if (!options.empty())
	{
		mNetwork->setOptions(options);
	}

	return true;
}

void CDMRMonitor::Run()
{
	LogInfo("Starting DMR Network");

	if (!mNetwork)
	{
		LogError("Network not initialized");
		return;
	}

	auto ret = mNetwork->open();
	if (!ret)
	{
		LogError("Unable to open DMR network");
		return;
	}

	mNetwork->enable(true);

	CStopWatch stopWatch;
	stopWatch.start();

	while (mRun)
	{
		unsigned int ms = stopWatch.elapsed();
		stopWatch.start();
		mNetwork->clock(ms);
		//mNetwork->writeHomePosition(mConfig.getLatitude(), mConfig.getLongitude());

		CDMRData data;
		auto readSuccess = mNetwork->read(data);
		if (readSuccess)
		{
			AnalyzeAndReportData(data);
		}

		if (ms < 5)
		{
			CThread::sleep(5);
		}
	}

	mNetwork->close();
}

void CDMRMonitor::Stop()
{
	mRun = false;
}

string CDMRMonitor::LookupDmrId(int id)
{
	if (!mCallsignLookup)
	{
		mCallsignLookup = new CDMRLookup("", 0);
	}

	return mCallsignLookup->find(id);
}

void CDMRMonitor::AnalyzeAndReportData(const CDMRData& data)
{
	static CStopWatch reportTimeout;

	unsigned int destinationId = data.getDstId();
	unsigned int sourceId = data.getSrcId();
	unsigned int slot = data.getSlotNo();
	unsigned char datatype = data.getDataType();

	// User IDs seem to start at 1000000
	if (sourceId >= 1000000)
	{
		if (datatype == 1 && reportTimeout.elapsed() > 1000) // 1 seems to be the start
		{
			reportTimeout.start(); // Use the timer to prevent multiple entries within a short time

			string callsign;
			callsign = LookupDmrId(sourceId);

			if (mUseColorizedConsole)
			{
				LogMessage("Slot: %s%d%s Source: %s%d%s %s%s%s Destination: %s%d%s", 
					CONSOLE_TEXT_MAGENTA, slot, CONSOLE_DEFAULT,
					CONSOLE_TEXT_BRIGHT, sourceId, CONSOLE_DEFAULT,
					CONSOLE_TEXT_GREEN, callsign.c_str(), CONSOLE_DEFAULT,
					CONSOLE_TEXT_BLUE, destinationId, CONSOLE_DEFAULT
				);
			}
			else
			{
				LogMessage("Slot: %d Source: %d (%s) Destination: %d Datatype: %d", slot, sourceId, callsign.c_str(), destinationId, datatype);
			}
		}
	}
	// Everything else is stuff from the Reflector / Talkgroup which we aren't interested in at the moment
}
