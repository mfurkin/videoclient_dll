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

// #include "EngineCreatingTools.h"
#include "AbortedStatusWriter.h"
#include "CompletedStatusWriter.h"
#include "LoggerEngine.h"
#include "InProgressStatusWriter.h"
#include "ReceivedStatusWriter.h"
#include "server_names.h"
#include "WorkingThreadEngine.h"

enum {WAITING_PING_COUNT=10, PING_PERIOD=10000};
typedef unsigned WINAPI (*ProcType)(void* ptr);

class WorkingThreadEngine;


class PingThreadEngine {
public:
	PingThreadEngine(unsigned short width, unsigned short height, unsigned short type, LoggerEngine* aLoggerPtr);
	virtual ~PingThreadEngine();
	int initServer(std::string&sourceName);
	void startProcessingThread(ProcType proc, void* p);
	int getResponseMessage(unsigned short* answered_ptr);
	static void deinitStatusWriter(std::pair<int,StatusWriter*> aPair);
	void processMessage(volatile int* finished_ptr);
	void outputMessage();
	WorkingThreadEngine getWorkingThreadEngine(volatile int* aFinishedPtr, std::string& destName);
	void waitNext();
private:
	void log(std::string& tag, std::string msg);
	void logPtr(std::string& tag, std::string msg, unsigned ptr);
	void logString(std::string& tag, std::string msg, std::string& msg2);
	void initPingProcessing();
	void deinitPingProcessing();
	std::string generateName(std::string first,	std::string second);
	int checkTimeouts(unsigned short& count);
	std::map<int, StatusWriter*> writers;
	unsigned short width,height,type;
	std::string pingReqName,pingNotifyName,pingSharedMemoryName,writeCompletedName, destFileAccessName, dataSharedMemoryName,
					writeEnabledName;
	static std::string PING_THREAD_ENGINE_CTOR_TAG,PING_THREAD_ENGINE_DTOR_TAG,PING_THREAD_TAG;
	HANDLE pingReq, pingNotify,pingSharedMemory;
	uint8_t* ping_ptr;
	static volatile long count;
	volatile int inited;
	LoggerEngine* logger_ptr;
	unsigned short timeout_count;
};

#endif /* PINGTHREADENGINE_H_ */
