/*
 * ClientRequestEngine.h
 *
 *  Created on: 2 дек. 2017 г.
 *      Author: Алёна
 */

#ifndef PINGTHREADENGINE_H_
#define PINGTHREADENGINE_H_
#include <algorithm>
#include <map>
#include <process.h>
#include <stdio.h>
#include <wtypes.h>
#include <winbase.h>
#include "DebuggingTools.h"
#include "EngineCreatingTools.h"
#include "AbortedStatusWriter.h"
#include "CompletedStatusWriter.h"
#include "PingThreadEngine.h"
#include "InProgressStatusWriter.h"
#include "ReceivedStatusWriter.h"
#include "server_names.h"
#include "WorkingThreadEngine.h"
enum {WAITING_PING_COUNT=10, PING_PERIOD=10000};
typedef unsigned WINAPI (*ProcType)(void* ptr);
class PingThreadEngine {
public:
	PingThreadEngine(unsigned short width, unsigned short height, unsigned short type);
	virtual ~PingThreadEngine();
	int initServer(std::string&sourceName);
	void startProcessingThread(ProcType proc, void* p);
	int getResponseMessage();
	static void deinitStatusWriter(std::pair<int,StatusWriter*> aPair);
	void processMessage(volatile int* finished_ptr);
	void outputMessage();
	WorkingThreadEngine getWorkingThreadEngine(volatile int* aFinishedPtr);
	void waitNext();
private:
	void initPingProcessing();
	void deinitPingProcessing();
	std::string generateName(std::string first,	std::string second);
	std::map<int, StatusWriter*> writers;
/*
	int createEvent(long manual, long initial, std::string& name, HANDLE* ev_ptr);
	int createSharedMemory(std::string name, unsigned sizeLow, unsigned rights, unsigned mapRights, HANDLE* filemap_ptr, uint8_t** pptr);

	void init();
	*/
	unsigned short width,height,type;
		std::string pingReqName,pingNotifyName,pingSharedMemoryName,writeCompletedName, destFileAccessName, dataSharedMemoryName,
					headerDataWrittenName;
//		writeEnabledName, dataSharedMemoryName;
	HANDLE pingReq, pingNotify,pingSharedMemory;
//		,writeCompleted,writeEnabled,dataSharedMemory;
//		uint8_t* data_ptr,*ping_ptr;
		uint8_t* ping_ptr;
		/*
		unsigned long bytesToWrite;
		LARGE_INTEGER offset;
		*/
	static volatile long count;
	volatile int inited;
	unsigned short timeout_count;
};

#endif /* PINGTHREADENGINE_H_ */
