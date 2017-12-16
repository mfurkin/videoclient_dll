/*
 * WorkingThreadEngine.cpp
 *
 *  Created on: 3 дек. 2017 г.
 *      Author: Алёна
 */

#include "WorkingThreadEngine.h"

WorkingThreadEngine::WorkingThreadEngine(std::string& aDestFileAccessName, std::string& aDataSharedMemoryName,
										 std::string& aHeaderDataWrittenName, unsigned aSize, volatile int* aFinishedPtr):
										 size(aSize), finished_ptr(aFinishedPtr) {
	/*
	int result = EngineCreatingTools::createEvent(FALSE,FALSE,aWriteCompletedName,&writeCompleted) &&
				 EngineCreatingTools::createEvent(FALSE,FALSE,aWriteEnabledName,&writeEnabled) &&
				 */
	int result = 	EngineCreatingTools::createMutex(FALSE,aDestFileAccessName,&destFileAccess) &&
					EngineCreatingTools::createEvent(FALSE,FALSE,aHeaderDataWrittenName,&headerDataWritten);
				 	EngineCreatingTools::createSharedMemory(aDataSharedMemoryName,size,PAGE_READWRITE,PAGE_READONLY,&dataSharedMemory,&data_ptr);
				 	DebuggingTools::logPtr("WorkingThreadEngine ctor headerDataWritten",(unsigned)headerDataWritten);
	inited = result;
	bytesToWrite = 0;
	frames = 0;
	offset.QuadPart = 0;
}

WorkingThreadEngine::~WorkingThreadEngine() {
	std::cerr<<"WorkingThreadEngine dtor enter \n";
	CloseHandle(destFileAccess);
	CloseHandle(dataSharedMemory);
}

void WorkingThreadEngine::receiveHeaderData() {
	unsigned res2;
	HANDLE event;
	std::cerr<<"WorkingThreadEngine::receiveHeaderData enter inited="<<inited<<"\n";
	if (inited) {
		unsigned i;
		i = 0;
		event = headerDataWritten;
		switch(i)
		for(;res2 != WAIT_OBJECT_0;) {
			case 0:
				DebuggingTools::logPtr("WorkingThreadEngine::receiveHeaderData pt1",(unsigned)headerDataWritten);
				res2 = WaitForSingleObjectEx(event,INFINITE,TRUE);
//				res2 = WaitForSingleObjectEx(headerDataWritten,INFINITE,TRUE);
				std::cerr<<"WorkingThreadEngine::receiveHeaderData pt1-1\n";
		}
		std::cerr<<"WorkingThreadEngine::receiveHeaderData pt2\n";
		unsigned result = WaitForSingleObjectEx(destFileAccess,INFINITE,TRUE);
		std::cerr<<"WorkingThreadEngine::receiveHeaderData pt3\n";
		if (result == WAIT_OBJECT_0) {
		std::cerr<<"WorkingThreadEngine::receiveHeaderData pt4\n";
		HeaderDataStruct* header_ptr = (HeaderDataStruct*)  data_ptr;
		frames = (*header_ptr).frames_qty;
		bytesToWrite = (*header_ptr).frame_size;
		ReleaseMutex(destFileAccess);
		std::cerr<<"WorkingThreadEngine::receiveHeaderData pt5\n";
	}
	std::cerr<<"WorkingThreadEngine::receiveHeaderData pt6\n";
	CloseHandle(headerDataWritten);
	}
	std::cerr<<"WorkingThreadEngine::receiveHeaderData exit\n";
}

int WorkingThreadEngine::waitData() {
	int result = WaitForSingleObjectEx(destFileAccess,INFINITE,TRUE);
//	int result = SignalObjectAndWait(writeEnabled,writeCompleted,INFINITE,TRUE);
	return (result == WAIT_OBJECT_0);
}

void WorkingThreadEngine::releaseAccess() {
	ReleaseMutex(destFileAccess);
}

void WorkingThreadEngine::checkThisWrite(unsigned long errorCode,	unsigned long bytesWritten) {
	if ((errorCode == 0) &&  (bytesWritten == bytesToWrite)) {
		offset.QuadPart +=  bytesWritten;
	}
	else {
		std::cerr<<"Error during file write error code="<<GetLastError()<<"\n";
		*finished_ptr = 0;
	}
}

void WorkingThreadEngine::writeToFile(HANDLE file) {
	OVERLAPPED ovl;
	ovl.Offset = offset.LowPart;
	ovl.OffsetHigh = offset.HighPart;
	ovl.hEvent = this;
	WriteFileEx(file,data_ptr,bytesToWrite,&ovl,WorkingThreadEngine::writeCompletedProc);
}

void __attribute__((__stdcall__)) WorkingThreadEngine::writeCompletedProc(DWORD dwErrorCode, DWORD dwNumberOfBytesTransferred,	LPOVERLAPPED lpOverlapped) {
	WorkingThreadEngine* ptr = (WorkingThreadEngine*) (*lpOverlapped).hEvent;
	(*ptr).checkThisWrite(dwErrorCode,dwNumberOfBytesTransferred);
}
