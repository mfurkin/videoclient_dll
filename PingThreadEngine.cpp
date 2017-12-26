/*
 * ClientRequestEngine.cpp
 *
 *  Created on: 2 дек. 2017 г.
 *      Author: Алёна
 */

#include "PingThreadEngine.h"
volatile long PingThreadEngine::count = 0;
std::string PingThreadEngine::PING_THREAD_ENGINE_CTOR_TAG = "PingThreadEngine::ctor";
std::string PingThreadEngine::PING_THREAD_ENGINE_DTOR_TAG = "PingThreadEngine::dtor";
std::string PingThreadEngine::PING_THREAD_TAG = "Ping_thread";
PingThreadEngine::PingThreadEngine(unsigned short aWidth, unsigned short aHeight, unsigned short aType, LoggerEngine* aLoggerPtr):width(aWidth),height(aHeight),type(aType),logger_ptr(aLoggerPtr) {
	char req_st[20];
	int result;
	inited = FALSE;
	long curCount = InterlockedIncrement(&count);
	sprintf(req_st,"req_%ld",curCount);
	std::string reqName = std::string(req_st);
	pingReqName = generateName(reqName,"_pingReq");
	pingNotifyName = generateName(reqName,"_pingNotify");
	pingSharedMemoryName = generateName(reqName,"_pingSharedMem");
	writeEnabledName = generateName(reqName,"_writeEnabled");
	writeCompletedName = generateName(reqName,"_writeCompleted");

	dataSharedMemoryName = generateName(reqName,"_dataSharedMem");
	result = 	EngineCreatingTools::createEvent(FALSE,FALSE,pingReqName,&pingReq) && EngineCreatingTools::createEvent(FALSE,FALSE,pingNotifyName,&pingNotify) &&
				EngineCreatingTools::createSharedMemory(pingSharedMemoryName,sizeof(RequestDataStruct),PAGE_READWRITE,FILE_MAP_WRITE,&pingSharedMemory,&ping_ptr);
	if (result)
		log(PING_THREAD_ENGINE_CTOR_TAG,"PingThreadEngine have been inited successfully");
	else
		logPtr(PING_THREAD_ENGINE_CTOR_TAG,"Errors during PingThreadEngine initialization: error code=",GetLastError());
 	inited = result;
 	timeout_count = WAITING_PING_COUNT;
 	initPingProcessing();
}

PingThreadEngine::~PingThreadEngine() {
	log(PING_THREAD_ENGINE_DTOR_TAG,"PingThreadEngine enter");
	deinitPingProcessing();
	UnmapViewOfFile(ping_ptr);
	EngineCreatingTools::closeHandle(pingReq);
	EngineCreatingTools::closeHandle(pingNotify);
	EngineCreatingTools::closeHandle(pingSharedMemory);
}

std::string PingThreadEngine::generateName(std::string first,	std::string second) {

	return first.append(second);
}


int PingThreadEngine::initServer(std::string& sourceName) {
	int result;
	HANDLE reqFlag,reqEnabledFlag,reqMutex,reqChannel;
	log(PING_THREAD_TAG,"PingThreadEngine::initServer enter");
	result = inited && EngineCreatingTools::findServer(&reqFlag,&reqEnabledFlag,&reqMutex,&reqChannel);
	logPtr(PING_THREAD_TAG,"PingThreadEngine::initServer pt1 result =",result);
	if (!(result)) {
		logPtr(PING_THREAD_TAG,"Could not find some server handles. error code=",GetLastError());
	} else	{
		uint8_t* srv_ptr;
		log(PING_THREAD_TAG,"PingThreadEngine::initServer pt2");
		srv_ptr = (uint8_t*) MapViewOfFile(reqChannel,FILE_MAP_WRITE,0,0,MAX_PATH);
		log(PING_THREAD_TAG,"PingThreadEngine::initServer pt3");
		result = (int) srv_ptr;
		 if(!(srv_ptr)) {
			 logPtr(PING_THREAD_TAG,"Could not find server channel. error code=",GetLastError());
		 } else {
			 	 HANDLE events[2];
			 	 std::map<int,StatusWriter*> statusWriters;
			 	 RequestDataStruct* req_ptr;
			 	log(PING_THREAD_TAG,"PingThreadEngine::initServer pt4");
			 	 events[0] = reqEnabledFlag;
			 	 events[1] = reqMutex;
			 	logPtr(PING_THREAD_TAG,"PingThreadEngine::initServer ping_ptr=",(unsigned)ping_ptr);
			 	req_ptr = (RequestDataStruct*) ping_ptr;
				(*req_ptr).width = width;
			 	(*req_ptr).height = height;
			 	(*req_ptr).conv_type = type;
			 	strcpy((*req_ptr).source_name,sourceName.c_str());
			 	strcpy((*req_ptr).pingReqName,pingReqName.c_str());
			 	strcpy((*req_ptr).pingNotifyName,pingNotifyName.c_str());
			 	strcpy((*req_ptr).dataName,dataSharedMemoryName.c_str());
			 	strcpy((*req_ptr).writeEnabledName,writeEnabledName.c_str());
			 	strcpy((*req_ptr).writeCompletedName,writeCompletedName.c_str());
			 	log(PING_THREAD_TAG,"PingThreadEngine::initServer pt10");
				WaitForMultipleObjectsEx(2,events,TRUE,INFINITE,TRUE);		// Поскольку нашли сервер, ждём бесконечно
				logString(PING_THREAD_TAG,"PingThreadEngine::initServer pt11-1 pingName=",pingSharedMemoryName);
			 	strcpy((char*)srv_ptr,pingSharedMemoryName.c_str());
			 	log(PING_THREAD_TAG,"PingThreadEngine::initServer pt12");
			 	SetEvent(reqFlag);
			 	log(PING_THREAD_TAG,"PingThreadEngine::initServer pt13");
			 	ReleaseMutex(reqMutex);
			 	log(PING_THREAD_TAG,"PingThreadEngine::initServer pt14");
				result = TRUE;
				UnmapViewOfFile(srv_ptr);
		 }
	}
	return result;
}

void PingThreadEngine::startProcessingThread(ProcType proc, void* p) {
	if (inited)
		_beginthreadex(NULL,0,proc,p,0,NULL);
}

void PingThreadEngine::initPingProcessing() {
	writers[REQ_RECEIVED] = new ReceivedStatusWriter();
	writers[REQ_CONV_IN_PROGRESS]= new InProgressStatusWriter();
	writers[REQ_COMPLETED]= new CompletedStatusWriter();
	writers[REQ_ABORTED] = new AbortedStatusWriter();
}

void PingThreadEngine::deinitPingProcessing() {
	for_each(writers.begin(),writers.end(),deinitStatusWriter);
}

void PingThreadEngine::deinitStatusWriter(std::pair<int, StatusWriter*> aPair) {
	delete aPair.second;
}

int PingThreadEngine::getResponseMessage(unsigned short* answered_ptr) {
	unsigned long res;
	int result;
	logPtr(PING_THREAD_TAG,"PingThreadEngine::getResponse message enter inited=",inited);
	if (!(inited))
		result = 0;
	else {
		result = TRUE;
		*answered_ptr = TRUE;
		res = WaitForSingleObjectEx(pingNotify,1000,TRUE);
		if (res == WAIT_FAILED) {
			result = 0;
			*answered_ptr = 0;
			logPtr(PING_THREAD_TAG,"getResponseMessage: res=WAIT_FAILED error code=",GetLastError());
		} else
			if (res == WAIT_TIMEOUT) {
				*answered_ptr = 0;
				log(PING_THREAD_TAG,"PingThreadEngine::getResponseMessage WAIT_TIMEOUT  have been got");
				if (!(checkTimeouts(timeout_count)))
					result = FALSE;
			}
	}
	logPtr(PING_THREAD_TAG,"PingThreadEngine::getResponseMessage exit result=",result);
	logPtr(PING_THREAD_TAG,"PingThreadEngine::getResponseMessage exit res=",res);
	return result;
}

void PingThreadEngine::processMessage(volatile int* finished_ptr) {
	logPtr(PING_THREAD_TAG,"PingThreadEngine::processMessage  enter inited=",inited);
	if (!(inited))
		*finished_ptr = 0;
	else {
		ResponseDataStruct* rsp_ptr = (ResponseDataStruct*) ping_ptr;
		timeout_count = WAITING_PING_COUNT;
		if ((*rsp_ptr).req_status == REQ_COMPLETED)
			*finished_ptr = 0;
	}
	log(PING_THREAD_TAG,"PingThreadEngine::processMessage exit");
}

void PingThreadEngine::outputMessage() {
	logPtr(PING_THREAD_TAG,"PingThreadEngine::outputMessage enter inited=",inited);
	if (inited) {
		ResponseDataStruct* rsp_ptr = (ResponseDataStruct*) ping_ptr;
		logPtr(PING_THREAD_TAG,"PingThreadEngine::outputMessage pt1 status=",(*rsp_ptr).req_status);
		StatusWriter* writer_ptr = writers[(*rsp_ptr).req_status];
		(*writer_ptr).outputStatus((*rsp_ptr).progress);
	}
	log(PING_THREAD_TAG,"PingThreadEngine::outputMessage exit");
}

WorkingThreadEngine PingThreadEngine::getWorkingThreadEngine(volatile int* aFinishedPtr, std::string& destName) {
	return WorkingThreadEngine(writeCompletedName,writeEnabledName,dataSharedMemoryName, width*height*COLORS_NUM,
							   destName,aFinishedPtr,logger_ptr);

}

void PingThreadEngine::waitNext() {
	SetEvent(pingReq);
	SleepEx(PING_PERIOD,TRUE);
}

void PingThreadEngine::logString(std::string& tag, std::string msg,	std::string& msg2) {
	if (logger_ptr)
		(*logger_ptr).logString(tag,msg,msg2);
}

int PingThreadEngine::checkTimeouts(unsigned short & count) {
	logPtr(PING_THREAD_TAG,"checkTimeouts WAIT_TIMEOT has been got count=",count--);
	if (!(count))
		log(PING_THREAD_TAG,"checkTimeouts: Count is finished");
	return count;
}

void PingThreadEngine::log(std::string& tag, std::string msg) {
	if (logger_ptr)
		(*logger_ptr).log(tag,msg);
}

void PingThreadEngine::logPtr(std::string& tag, std::string msg, unsigned ptr) {
	if (logger_ptr)
		(*logger_ptr).logPtr(tag,msg,ptr);
}

