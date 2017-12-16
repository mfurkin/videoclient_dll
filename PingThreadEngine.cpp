/*
 * ClientRequestEngine.cpp
 *
 *  Created on: 2 дек. 2017 г.
 *      Author: Алёна
 */

#include "PingThreadEngine.h"
volatile long PingThreadEngine::count = 0;
PingThreadEngine::PingThreadEngine(unsigned short aWidth, unsigned short aHeight, unsigned short aType):width(aWidth),height(aHeight),type(aType) {
//	init(size);
	char req_st[20];
	int result;
	inited = FALSE;
	long curCount = InterlockedIncrement(&count);
	sprintf(req_st,"req_%ld",curCount);
	std::string reqName = std::string(req_st);
//	std::string globalName = std::string("Global\\").append(reqName);
	pingReqName = generateName(reqName,"_pingReq");
	pingNotifyName = generateName(reqName,"_pingNotify");
	pingSharedMemoryName = generateName(reqName,"_pingSharedMem");
//	pingSharedMemoryName = generateName(globalName,"_pingSharedMem");

	destFileAccessName = generateName(reqName,"_destFileAccess");
	headerDataWrittenName = generateName(reqName,"_headerDataWritten");

//	dataSharedMemoryName = generateName(globalName,"_dataSharedMem");
	dataSharedMemoryName = generateName(reqName,"_dataSharedMem");
	result = 	EngineCreatingTools::createEvent(FALSE,FALSE,pingReqName,&pingReq) && EngineCreatingTools::createEvent(FALSE,FALSE,pingNotifyName,&pingNotify) &&
				EngineCreatingTools::createSharedMemory(pingSharedMemoryName,sizeof(RequestDataStruct),PAGE_READWRITE,FILE_MAP_WRITE,&pingSharedMemory,&ping_ptr);
	DebuggingTools::logPtr("PingThreadEngine ctor ping_ptr=",(unsigned)ping_ptr);
	if (result)
		std::cerr<<"PingThreadEngine have been inited successfully\n";
	else
		std::cerr<<"Errors during PingThreadEngine initialization: error code="<<GetLastError()<<"\n";
 	inited = result;
 	timeout_count = WAITING_PING_COUNT;
 	initPingProcessing();
}

PingThreadEngine::~PingThreadEngine() {
	std::cerr<<"PingThreadEngine dtor enter\n";
	deinitPingProcessing();
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
	std::cerr<<"PingThreadEngine::initServer enter\n";
	result = inited && EngineCreatingTools::findServer(&reqFlag,&reqEnabledFlag,&reqMutex,&reqChannel);
	std::cerr<<"PingThreadEngine::initServer pt1 result ="<<result<<"\n";
	if (!(result)) {
		std::cerr<<"Could not find some server handles. error code="<<GetLastError()<<"\n";
	} else	{
		uint8_t* srv_ptr;
		std::cerr<<"PingThreadEngine::initServer pt2\n";
		srv_ptr = (uint8_t*) MapViewOfFile(reqChannel,FILE_MAP_WRITE,0,0,MAX_PATH);
		std::cerr<<"PingThreadEngine::initServer pt3\n";
		result = (int) srv_ptr;
		 if(!(srv_ptr)) {
			 std::cerr<<"Could not find server channel. error code="<<GetLastError()<<"\n";
		 } else {
			 	 HANDLE events[2];
//		 	 unsigned long res1,res2;
//			 	 int res4;
//			 	 unsigned res2;
//			 	MEMORY_BASIC_INFORMATION meminfo;
			 	 std::map<int,StatusWriter*> statusWriters;
			 	 RequestDataStruct* req_ptr;
					std::cerr<<"PingThreadEngine::initServer pt4\n";

			 	 events[0] = reqEnabledFlag;
			 	 events[1] = reqMutex;
			 	 DebuggingTools::checkFileMapping(pingSharedMemoryName,"PingThreadEngine::initServer");
					DebuggingTools::logPtr("PingThreadEngine::initServer ping_ptr=",(unsigned)ping_ptr);
			 	 req_ptr = (RequestDataStruct*) ping_ptr;
//			 	 VirtualQuery(ping_ptr,&meminfo,sizeof(MEMORY_BASIC_INFORMATION));
				 	 (*req_ptr).width = width;

			 	 (*req_ptr).height = height;

			 	 (*req_ptr).conv_type = type;
//					std::cerr<<"PingThreadEngine::initServer pt5\n";
			 	 strcpy((*req_ptr).source_name,sourceName.c_str());
//					std::cerr<<"PingThreadEngine::initServer pt6\n";
			 	 strcpy((*req_ptr).pingReqName,pingReqName.c_str());
//					std::cerr<<"PingThreadEngine::initServer pt7\n";
			 	 strcpy((*req_ptr).pingNotifyName,pingNotifyName.c_str());
//					std::cerr<<"PingThreadEngine::initServer pt8\n";
			 	 strcpy((*req_ptr).dataName,dataSharedMemoryName.c_str());
//					std::cerr<<"PingThreadEngine::initServer pt9\n";
			 	 strcpy((*req_ptr).destFileAccessName,destFileAccessName.c_str());
					std::cerr<<"PingThreadEngine::initServer pt10\n";
//					res1 = WaitForSingleObjectEx(reqEnabledFlag,INFINITE,TRUE);
				strcpy((*req_ptr).headerDataWrittenName,headerDataWrittenName.c_str());
//			 	 res2 =
			 			 WaitForMultipleObjectsEx(2,events,TRUE,INFINITE,TRUE);		// Поскольку нашли сервер, ждём бесконечно
/*
					std::cerr<<"PingThreadEngine::initServer pt11\n";
					res2 = WaitForSingleObjectEx(reqMutex,INFINITE,TRUE);
*/
					std::cerr<<"PingThreadEngine::initServer pt11-1 pingName="<<pingSharedMemoryName<<"\n";
			 	 strcpy((char*)srv_ptr,pingSharedMemoryName.c_str());
					std::cerr<<"PingThreadEngine::initServer pt12\n";
			 	 SetEvent(reqFlag);
					std::cerr<<"PingThreadEngine::initServer pt13\n";
			 	 ReleaseMutex(reqMutex);
					std::cerr<<"PingThreadEngine::initServer pt14\n";
					result = TRUE;
		 }
		 /*
		 CloseHandle(reqFlag);
		 CloseHandle(reqEnabledFlag);
		 CloseHandle(reqMutex);
		 CloseHandle(reqChannel);
		 */
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
	std::cerr<<"PingThreadEngine::getResponse message enter inited="<<inited<<"\n";
	if (!(inited))
		result = 0;
	else {
		result = TRUE;
		*answered_ptr = TRUE;
		res = WaitForSingleObjectEx(pingNotify,1000,TRUE);
		if (res == WAIT_FAILED) {
			result = 0;
			*answered_ptr = 0;
			std::cerr<<"getResponseMessage: res=WAIT_FAILED error code="<<GetLastError()<<"\n";
		} else
			if (res == WAIT_TIMEOUT) {
				*answered_ptr = 0;
				std::cerr<<"PingThreadEngine::getResponseMessage WAIT_TIMEOUT  have been got\n";
				if (!(DebuggingTools::checkTimeouts(timeout_count)))
					result = FALSE;
			}
//			if ((result == WAIT_TIMEOUT) && (!(--timeout_count)))
/*			if ((res == WAIT_TIMEOUT) && (!(DebuggingTools::checkTimeouts(timeout_count))))
				result = FALSE;
				*/
	}
	std::cerr<<"PingThreadEngine::getResponseMessage exit result="<<result<<" res="<<res<<"\n";
	return result;
}

void PingThreadEngine::processMessage(volatile int* finished_ptr) {
	std::cerr<<"PingThreadEngine::processMessage  enter inited="<<inited<<"\n";
	if (!(inited))
		*finished_ptr = 0;
	else {
		ResponseDataStruct* rsp_ptr = (ResponseDataStruct*) ping_ptr;
		timeout_count = WAITING_PING_COUNT;
		if ((*rsp_ptr).req_status == REQ_COMPLETED)
			*finished_ptr = 0;
	}
	std::cerr<<"PingThreadEngine::processMessage exit\n";
}

void PingThreadEngine::outputMessage() {
	if (inited) {
		ResponseDataStruct* rsp_ptr = (ResponseDataStruct*) ping_ptr;
		StatusWriter* writer_ptr = writers[(*rsp_ptr).req_status];
		(*writer_ptr).outputStatus((*rsp_ptr).progress);
	}
}

WorkingThreadEngine PingThreadEngine::getWorkingThreadEngine(volatile int* aFinishedPtr) {
//	return WorkingThreadEngine(writeCompletedName,writeEnabledName,dataSharedMemoryName,width*height*COLORS_NUM);
	return WorkingThreadEngine(destFileAccessName,dataSharedMemoryName,headerDataWrittenName, width*height*COLORS_NUM,aFinishedPtr);
}

void PingThreadEngine::waitNext() {
	SleepEx(PING_PERIOD,TRUE);
	SetEvent(pingReq);
}
/*
void PingThreadEngine::init() {
	char req_st[20];
	int result;
	inited = FALSE;
	long curCount = InterlockedIncrement(&count);
	sprintf(req_st,"req_%ld",curCount);
	std::string reqName = std::string(req_st);
	pingReqName = generateName(reqName,"_pingReq");
	pingNotifyName = generateName(reqName,"_pingNotify");
	pingSharedMemoryName = generateName(reqName,"_pingSharedMem");

	writeCompletedName = generateName(reqName,"_writeCompleted");
	writeEnabledName = generateName(reqName,"_writeEnabled");

	result = 	EngineCreatingTools::createEvent(FALSE,FALSE,pingReqName,&pingReq) && EngineCreatingTools::createEvent(FALSE,FALSE,pingNotifyName,&pingNotify) &&
//			 createEvent(FALSE,FALSE,writeCompletedName,&writeCompleted) && createEvent(FALSE,FALSE,writeEnabledName,&writeEnabled) &&
				EngineCreatingTools::createSharedMemory(pingSharedMemoryName,sizeof(RequestDataStruct),PAGE_READWRITE,PAGE_READWRITE,&pingSharedMemory,&ping_ptr);
//			 &&
//			 createSharedMemory(dataSharedMemoryName.c_str(),size,PAGE_READWRITE,PAGE_READONLY,&dataSharedMemory,&data_ptr);
	if (result)
		std::cerr<<"ClientRequest have been inited successfully";
	else
		std::cerr<<"Errors during ClientRequest initialization: error code="<<GetLastError()<<"\n";
 	inited = result;
}
*/
