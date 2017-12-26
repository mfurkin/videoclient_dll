/*
 * WorkingThreadEngine.cpp
 *
 *  Created on: 3 дек. 2017 г.
 *      Author: Алёна
 */

#include "WorkingThreadEngine.h"
std::string WorkingThreadEngine::WORKING_THREAD_TAG = "Working_thread";
WorkingThreadEngine::WorkingThreadEngine(std::string& aWriteCompletedName, std::string& aWriteEnabledName, std::string& aDataSharedMemoryName,
										 unsigned aSize, std::string& aDestName, volatile int* aFinishedPtr, LoggerEngine* aLoggerPtr): size(aSize), finished_ptr(aFinishedPtr),
										 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 logger_ptr(aLoggerPtr) {
	int result = 	EngineCreatingTools::createEvent(FALSE,FALSE,aWriteCompletedName,&writeCompleted) &&
					EngineCreatingTools::createEvent(FALSE,FALSE,aWriteEnabledName,&writeEnabled) &&
				 	EngineCreatingTools::createSharedMemory(aDataSharedMemoryName,size,PAGE_READWRITE,PAGE_READONLY,&dataSharedMemory,&data_ptr);// &&
	fileDest = CreateFile(aDestName.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_FLAG_OVERLAPPED,NULL);
	inited = result;
	bytesToWrite = 0;
	frames = 0;
	offset.QuadPart = 0;
}

WorkingThreadEngine::~WorkingThreadEngine() {
	log("WorkingThreadEngine dtor enter");
	UnmapViewOfFile(data_ptr);
	CloseHandle(writeCompleted);
	CloseHandle(writeEnabled);
	CloseHandle(dataSharedMemory);
	CloseHandle(fileDest);
}

void WorkingThreadEngine::receiveHeaderData(volatile int& in_progress) {
	logPtr("WorkingThreadEngine::receiveHeaderData enter inited=",inited);
	if (inited) {
		log("WorkingThreadEngine::receiveHeaderData pt1");
		SetEvent(writeEnabled);
		unsigned result = WaitForSingleObjectEx(writeCompleted,INFINITE,TRUE);
		if (result != WAIT_OBJECT_0) {
			in_progress = 0;
			logPtr("WorkingThreadEngine::receiveHeaderData result=",result);
		} else {
			log("WorkingThreadEngine::receiveHeaderData pt2");
			HeaderDataStruct* header_ptr = (HeaderDataStruct*)  data_ptr;
			frames = (*header_ptr).frames_qty;
			bytesToWrite = (*header_ptr).frame_size;
			logPtr("WorkingThreadEngine::receiveHeaderData pt5 frames=",frames);
		}
		log("WorkingThreadEngine::receiveHeaderData pt6");
	}
	log("WorkingThreadEngine::receiveHeaderData exit");
}

int WorkingThreadEngine::waitData() {
	unsigned result = WaitForSingleObjectEx(writeCompleted,INFINITE,TRUE);
	logPtr("WorkingThreadEngine::waitData result=",result);
	if (result == WAIT_FAILED)
		logPtr("WorkingThreadEngine::waitData wait Failed error code=",GetLastError());
	return (result == WAIT_OBJECT_0);
}

void WorkingThreadEngine::releaseAccess() {
	SetEvent(writeEnabled);
}

void WorkingThreadEngine::checkThisWrite(unsigned long errorCode,	unsigned long bytesWritten) {
	logPtr("WorkingThreadEngine::checkThisWrite enter errorCode=",errorCode);
	logPtr("WorkingThreadEngine::checkThisWrite bytesWritten=",bytesWritten);
	logPtr("WorkingThreadEngine::checkThisWrite  bytesToWrite =",bytesToWrite);
	if ((errorCode == 0) &&  (bytesWritten == bytesToWrite))
		offset.QuadPart +=  bytesWritten;
	else {
		logPtr("Error during file write error code=",errorCode);
		*finished_ptr = 0;
	}
	log("WorkingThreadEngine::checkThisWrite exit");
}

void WorkingThreadEngine::writeConvertedFile(volatile int& in_progress) {
	int result,res = fileCreated();
	logPtr("WorkingThreadEngine:writeConvertedFile enter res=",res);
	if ((!(inited)) || (!(res)))
		in_progress = 0;
	else {
		logPtr("WorkingThreadEngine:writeConvertedFile pt1 in_progress=",in_progress);
		for (receiveHeaderData(in_progress);in_progress && (nextFrame()) && (result = waitData());) {
			logPtr("WorkingThreadEngine:writeConvertedFile pt2 result=",result);
			if (result) {
				writeToFile();
				waitWriteComplete();
				releaseAccess();
			}
			log("WorkingThreadEngine:writeConvertedFile pt3");
		}
		log("WorkingThreadEngine:writeConvertedFile pt4");
	}
	log("WorkingThreadEngine:writeConvertedFile exits");
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
	logPtr("WorkingThreadEngine::nextFrame result=",result);
	return result;
}

void WorkingThreadEngine::log(std::string msg) {
	if (logger_ptr)
		(*logger_ptr).log(WORKING_THREAD_TAG,msg);
}

void WorkingThreadEngine::logPtr(std::string msg,	unsigned ptr) {
	if  (logger_ptr)
		(*logger_ptr).logPtr(WORKING_THREAD_TAG,msg,ptr);
}

void WorkingThreadEngine::logString(std::string msg,	std::string& msg2) {
	if (logger_ptr)
		(*logger_ptr).logString(WORKING_THREAD_TAG,msg,msg2);
}
