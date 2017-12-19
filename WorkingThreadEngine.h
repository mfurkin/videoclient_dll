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
#include "DebuggingTools.h"
#include "EngineCreatingTools.h"

class WorkingThreadEngine {
public:
	WorkingThreadEngine(std::string& aDestFileAccessName, std::string& aDataSharedMemoryName, std::string& aHeaderDataWritten,
						unsigned aSize,	std::string& aDestName, volatile int* aFinishedPtr);
	virtual ~WorkingThreadEngine();

	static VOID CALLBACK writeCompletedProc( DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
	void writeConvertedFile(volatile int& in_progress);
private:
	int fileCreated();
	int waitWriteComplete();
	int nextFrame();
	void receiveHeaderData();
	int waitData();
	void writeToFile();
	void releaseAccess();
	void checkThisWrite(unsigned long errorCode,	unsigned long bytesWritten);
	unsigned size,frames,bytesToWrite;
	volatile int* finished_ptr;
	LARGE_INTEGER offset;
//	HANDLE writeCompleted,writeEnabled,dataSharedMemory;
	HANDLE destFileAccess,dataSharedMemory,headerDataWritten,fileDest;
	uint8_t* data_ptr;
	int inited;
};

#endif /* WORKINGTHREADENGINE_H_ */
