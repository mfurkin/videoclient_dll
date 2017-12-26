/*
 * WorkingThreadEngine.h
 *
 *  Created on: 3 дек. 2017 г.
 *      Author: Алёна
 */

#ifndef WORKINGTHREADENGINE_H_
#define WORKINGTHREADENGINE_H_
// #include <iostream>
#include <stdint.h>
#include <wtypes.h>
#include <winbase.h>
#include "windef.h"

#include "EngineCreatingTools.h"
#include "LOggerEngine.h"
class WorkingThreadEngine {
public:
	WorkingThreadEngine(std::string& aWriteCompletedName, std::string& aWriteEnabledName, std::string& aDataSharedMemoryName,
						unsigned aSize,	std::string& aDestName, volatile int* aFinishedPtr,LoggerEngine* aLoggerPtr);
	virtual ~WorkingThreadEngine();

	static VOID CALLBACK writeCompletedProc( DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
	void writeConvertedFile(volatile int& in_progress);
private:
	void log(std::string msg);
	void logPtr(std::string msg, unsigned ptr);
	void logString(std::string msg, std::string& msg2);
	int fileCreated();
	int waitWriteComplete();
	int nextFrame();
	void receiveHeaderData(volatile int& in_progress);
	int waitData();
	void writeToFile();
	void releaseAccess();
	void checkThisWrite(unsigned long errorCode,	unsigned long bytesWritten);
	unsigned size,frames,bytesToWrite;
	volatile int* finished_ptr;
	LARGE_INTEGER offset;
	HANDLE writeCompleted,writeEnabled,dataSharedMemory,fileDest;
	static std::string WORKING_THREAD_TAG;
	uint8_t* data_ptr;
	LoggerEngine* logger_ptr;
	int inited;
};

#endif /* WORKINGTHREADENGINE_H_ */
