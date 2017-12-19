/*
 * WorkingThreadEngine.cpp
 *
 *  Created on: 3 дек. 2017 г.
 *      Author: Алёна
 */

#include "WorkingThreadEngine.h"

WorkingThreadEngine::WorkingThreadEngine(std::string& aDestFileAccessName, std::string& aDataSharedMemoryName,
										 std::string& aHeaderDataWrittenName, unsigned aSize, std::string& aDestName,
										 volatile int* aFinishedPtr): size(aSize), finished_ptr(aFinishedPtr) {
	int result = 	EngineCreatingTools::createMutex(FALSE,aDestFileAccessName,&destFileAccess) &&
					EngineCreatingTools::createEvent(FALSE,FALSE,aHeaderDataWrittenName,&headerDataWritten) &&
				 	EngineCreatingTools::createSharedMemory(aDataSharedMemoryName,size,PAGE_READWRITE,PAGE_READONLY,&dataSharedMemory,&data_ptr);
	DebuggingTools::logPtr("WorkingThreadEngine ctor headerDataWritten",(unsigned)headerDataWritten);
	fileDest = CreateFile(aDestName.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_FLAG_OVERLAPPED,NULL);
	DebuggingTools::logPtr("WorkingThreadEngine ctor pt1",(unsigned)fileDest);
	DebuggingTools::logPtr("WorkingThreadEngine ctor pt2",(unsigned)destFileAccess);
	inited = result;
	bytesToWrite = 0;
	frames = 0;
	offset.QuadPart = 0;
}

WorkingThreadEngine::~WorkingThreadEngine() {
	std::cerr<<"WorkingThreadEngine dtor enter \n";
	UnmapViewOfFile(data_ptr);
	CloseHandle(destFileAccess);
	CloseHandle(dataSharedMemory);
	CloseHandle(fileDest);
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
	std::cerr<<"WorkingThreadEngine::checkThisWrite enter errorCode="<<errorCode<<" bytesWritten="<<bytesWritten<<" bytesToWrite ="<<bytesToWrite<<"\n";
	if ((errorCode == 0) &&  (bytesWritten == bytesToWrite)) {
		offset.QuadPart +=  bytesWritten;
	}
	else {
		std::cerr<<"Error during file write error code="<<errorCode<<"\n";
		*finished_ptr = 0;
	}
	std::cerr<<"WorkingThreadEngine::checkThisWrite exit\n";
}

void WorkingThreadEngine::writeConvertedFile(volatile int& in_progress) {
	int result,res = fileCreated();
	std::cerr<<"WorkingThreadEngine:writeConvertedFile enter res="<<res<<" inited="<<inited<<"\n";
//	if (fileCreated())
	if ((!(inited)) || (!(res)))
		in_progress = 0;
	else {
		std::cerr<<"WorkingThreadEngine:writeConvertedFile pt1 in_progress="<<in_progress<<"\n";
		for (receiveHeaderData();in_progress && (nextFrame()) && (result = waitData());) {
			std::cerr<<"WorkingThreadEngine:writeConvertedFile pt2 result="<<result<<"\n";
			if (result) {
				writeToFile();
				waitWriteComplete();
				releaseAccess();
			}
			std::cerr<<"WorkingThreadEngine:writeConvertedFile pt3\n";
		}
		std::cerr<<"WorkingThreadEngine:writeConvertedFile pt4\n";
	}
	std::cerr<<"WorkingThreadEngine:writeConvertedFile exits\n";
}

void WorkingThreadEngine::writeToFile() {
		OVERLAPPED ovl;
		ovl.Offset = offset.LowPart;
		ovl.OffsetHigh = offset.HighPart;
		ovl.hEvent = this;
		WriteFileEx(fileDest,data_ptr,bytesToWrite,&ovl,WorkingThreadEngine::writeCompletedProc);
}

void __attribute__((__stdcall__)) WorkingThreadEngine::writeCompletedProc(DWORD dwErrorCode, DWORD dwNumberOfBytesTransferred,	LPOVERLAPPED lpOverlapped) {
	WorkingThreadEngine* ptr = (WorkingThreadEngine*) (*lpOverlapped).hEvent;
	(*ptr).checkThisWrite(dwErrorCode,dwNumberOfBytesTransferred);
}

int WorkingThreadEngine::fileCreated() {
	return (fileDest != INVALID_HANDLE_VALUE);
}

int WorkingThreadEngine::waitWriteComplete() {
	unsigned result = WaitForSingleObjectEx(fileDest,INFINITE,TRUE);
	return ((result == WAIT_IO_COMPLETION) || (result == WAIT_OBJECT_0));
}

int WorkingThreadEngine::nextFrame() {
	int result = frames--;
	std::cerr<<"WorkingThreadEngine::nextFrame result="<<result<<"\n";
	return result;
}
