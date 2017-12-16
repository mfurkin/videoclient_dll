/*
 * ClientRequest.cpp
 *
 *  Created on: 25 нояб. 2017 г.
 *
 */

#include "ClientRequest.h"
#include "ClientCommon.h"
// long ClientRequest::count = 0;
ClientRequest::ClientRequest():	width(0),height(0),type(0), sourceName(std::string("empty sourceName")), destName(std::string("empty destName")),
								engine_ptr(NULL),in_progress(0) {
}

ClientRequest::ClientRequest(const ClientRequest& aRequest):width(aRequest.width),height(aRequest.height),type(aRequest.type),
															sourceName(aRequest.sourceName), destName(aRequest.destName),
															engine_ptr(aRequest.engine_ptr),in_progress(aRequest.in_progress)
{
}
/*
ClientRequest::ClientRequest(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight, unsigned short aType,
							ErrorReport* aReqPtr): width(aWidth),height(aHeight),type(aType),sourceName(aSourceName),destName(aDestName),
													  req_ptr(aReqPtr) {
	init();

}
*/
ClientRequest::ClientRequest(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight,
							 unsigned short aType): width(aWidth),height(aHeight),type(aType),sourceName(aSourceName),destName(aDestName),
									 	 	 	 	engine_ptr(NULL) {
//	init();
}
ClientRequest::~ClientRequest() {
	DebuggingTools::logPtr("ClientRequest dtor, this:",(unsigned)this);
/*
	if (!(copied))  {
		closeHandle(pingReq);
		closeHandle(pingNotify);
		closeHandle(writeEnabled);
		closeHandle(writeCompleted);
		closeHandle(pingSharedMemory);
		closeHandle(dataSharedMemory);
	}
	if (req_ptr != NULL)
		delete  req_ptr;
		*/
}


void ClientRequest::sendRequest() {
	unsigned result;
//	RequestDataStruct* req_ptr;
//	HANDLE reqFlag,reqEnabledFlag,reqMutex,reqChannel;
	PingThreadEngine engine(width,height,type);
/*
	result = openEvent(EVENT_MODIFY_STATE,REQ_FLAG_NAME,&reqFlag) && openEvent(SYNCHRONIZE,REQ_ENABLED_FLAG,&reqEnabledFlag) &&
			 openMutex(SYNCHRONIZE,REQ_MUTEX_NAME,&reqMutex) && openFileMapping(FILE_MAP_WRITE,REQ_FILE_NAME,&reqChannel);
	if (!(result)) {
		std::cerr<<"Could not find some server handles. error code="<<GetLastError()<<"\n";
		errorReport();
	} else	{
		uint8_t* srv_ptr;
		srv_ptr = (uint8_t*) MapViewOfFile(reqChannel,FILE_MAP_WRITE,0,0,MAX_PATH);
		 if(!(srv_ptr)) {
			 std::cerr<<"Could not find server channel. error code="<<GetLastError()<<"\n";
			 errorReport();
		 } else {
		 */
/*
	if (!(engine.findServer())) {
		std::cerr<<"Could not find some server handles. error code="<<GetLastError()<<"\n";
		errorReport();
	} else
			if (!(engine.findServerChannel())) {
				std::cerr<<"Could not find server channel. error code="<<GetLastError()<<"\n";
				errorReport();
			} else {
			 HANDLE events[2];
			 int timeout_count;
			 std::map<int,StatusWriter*> statusWriters;
			 events[0] = reqEnabledFlag;
			 events[1] = reqMutex;
			 req_ptr = (RequestDataStruct*) ping_ptr;
			 (*req_ptr).width = width;
			 (*req_ptr).height = height;
			 (*req_ptr).conv_type = type;
			 strcpy((*req_ptr).source_name,sourceName.c_str());
			 strcpy((*req_ptr).pingReqName,pingReqName.c_str());
			 strcpy((*req_ptr).pingNotifyName,pingNotifyName.c_str());
			 strcpy((*req_ptr).writeCompletedName,writeCompletedName.c_str());
			 strcpy((*req_ptr).writeEnabledName,writeEnabledName.c_str());
			 result = WaitForMultipleObjectsEx(2,events,TRUE,INFINITE,TRUE);		// Поскольку нашли сервер, ждём бесконечно
			 strcpy((char*)req_ptr,pingSharedMemoryName.c_str());
			 SetEvent(reqFlag);
			 ReleaseMutex(reqMutex);
			 */
	if (!(engine.initServer(sourceName))) {
		std::cerr<<"ClientRequest::sendRequest before currentErrorReport call\n";
		currentErrorReport();
		std::cerr<<"ClientRequest::sendRequest after currentErrorReport call\n";
	} else {
			unsigned short answered;
			engine_ptr = &engine;
			in_progress = 1;
			engine.startProcessingThread(processRequest,this);
			std::cerr<<"ClientRequest::sendRequest before loop\n";
			answered = 0;
			for (;in_progress && (result = engine.getResponseMessage(&answered));engine.waitNext()) {
				if (answered) {
					engine.processMessage(&in_progress);
					engine.outputMessage();
					answered = 0;
				}
			}

//			 _beginthreadex(NULL,0,processRequest,this,0,NULL);
/*
			 SetEvent(pingReq);
			 timeout_count = 10;
			 initStatusWriters(statusWriters);
			 for (;in_progress && (result = WaitForSingleObjectEx(pingNotify,1000,TRUE));) {
				 if (result == WAIT_FAILED)
					 in_progress = 0;
				 else
					 if ((result == WAIT_TIMEOUT) && (!(--timeout_count)))
						 in_progress = 0;
					 else {
						ResponseDataStruct* rsp_ptr = (ResponseDataStruct*) ping_ptr;
						StatusWriter* writer_ptr = statusWriters[(*rsp_ptr).req_status];
						(*writer_ptr).outputStatus((*rsp_ptr).progress);
						timeout_count = 10;
						if ((*rsp_ptr).req_status == REQ_COMPLETED)
							in_progress = 0;
					 }
			 }
			 deinitStatusWriters(statusWriters);
			 */
		}
}

std::string ClientRequest::getTimeSt(time_t* time) {
	char st[25];
	struct tm* tm_ptr = localtime(time);
	unsigned short month, year;
	month = (*tm_ptr).tm_mon + 1;
	year = (*tm_ptr).tm_year + 1900;
	sprintf(st,"%2d.%2d.%4d %2d:%2d:%2d",(*tm_ptr).tm_mday,month,year,(*tm_ptr).tm_hour,(*tm_ptr).tm_min,(*tm_ptr).tm_sec);
	return std::string(st);
}

void ClientRequest::currentErrorReport() {
//	std::cerr<<"currentErrorReport enter\n";
	time_t curTime = time(NULL);
//	std::cerr<<"currentErrorReport pt1\n";
	std::string dateSt = getTimeSt(&curTime);
	std::string key = getKey();
//	std::cerr<<"currentErrorReport pt2"<<key<<"\n";
	DebuggingTools::logPtr("currentErrorReport this:",(unsigned)this);
	ClientCommon::getClientCommon().addError(key,*this,curTime,dateSt);
//	ClientCommon::getClientCommon().addError(getKey(),*this,curTime,dateSt);
//	std::cerr<<"currentErrorReport exit\n";
}
/*
ErrorReport* ClientRequest::createErrorReport() {

	time_t curTime = time(NULL);
	std::string timeSt = getTimeSt(&curTime);
	ErrorReport* req_ptr = new ErrorReport(*this,timeSt,curTime);

	return req_ptr;
}
*/
std::string ClientRequest::generateName(std::string& first,	std::string second) {

	return first.append(second);
}
/*
void ClientRequest::init() {
	char req_st[20];
	int result;
	inited = FALSE;
	copied = 0;
	in_progress = 0;
	long curCount = InterlockedIncrement(&count);
	sprintf(req_st,"req_%ld",curCount);
	std::string reqName = std::string(req_st);
	pingReqName = generateName(reqName,"_pingReq");
	pingNotifyName = generateName(reqName,"_pingNotify");
	pingSharedMemoryName = generateName(reqName,"_pingSharedMem");
	writeCompletedName = generateName(reqName,"_writeCompleted");
	writeEnabledName = generateName(reqName,"_writeEnabled");
	unsigned size = width*height*COLORS_NUM;
	result = createEvent(FALSE,FALSE,pingReqName,&pingReq) && createEvent(FALSE,FALSE,pingNotifyName,&pingNotify) &&
			 createEvent(FALSE,FALSE,writeCompletedName,&writeCompleted) && createEvent(FALSE,FALSE,writeEnabledName,&writeEnabled) &&
			 createSharedMemory(pingSharedMemoryName.c_str(),sizeof(RequestDataStruct),PAGE_READWRITE,PAGE_READWRITE,&pingSharedMemory,&ping_ptr) &&
			 createSharedMemory(dataSharedMemoryName.c_str(),size,PAGE_READWRITE,PAGE_READONLY,&dataSharedMemory,&data_ptr);
	if (result)
		std::cerr<<"ClientRequest have been inited successfully";
	else
		std::cerr<<"Errors during ClientRequest initialization: error code="<<GetLastError()<<"\n";
 	inited = result;
}
*/
unsigned __attribute__((__stdcall__)) ClientRequest::processRequest(void* p) {
	ClientRequest* ptr = (ClientRequest*) p;
	return (*ptr).processThisRequest();
}

unsigned ClientRequest::processThisRequest() {
	std::cerr<<"ClientRequest::processThisRequest enter\n";
	HANDLE fileDest = CreateFile(destName.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_FLAG_OVERLAPPED,NULL);
	DebuggingTools::logPtr("ClientRequest::processThisRequest pt1",(unsigned)fileDest);
	WorkingThreadEngine engine = (*engine_ptr).getWorkingThreadEngine(&in_progress);
	if (fileDest == INVALID_HANDLE_VALUE)
		in_progress = 0;
	else  {
		unsigned long result;
		unsigned frames = 1;
		for (engine.receiveHeaderData();in_progress && (frames--) && (result = engine.waitData());
				result = WaitForSingleObjectEx(fileDest,INFINITE,TRUE)) {
			if (result) {
				engine.writeToFile(fileDest);
				engine.releaseAccess();
/*
  				ResetEvent(writeEnabled);
				ovl.Offset = offset.LowPart;
				ovl.OffsetHigh = offset.HighPart;
				ovl.hEvent = &engine;
				WriteFileEx(fileDest,data_ptr,bytesToWrite,&ovl,ClientRequest::writeCompletedProc);
				*/
			}
			std::cerr<<"ClientRequest::processThisRequest pt4\n";
		}
		std::cerr<<"ClientRequest::processThisRequest pt5\n";
		CloseHandle(fileDest);
	}
	std::cerr<<"ClientRequest::processThisRequest exit\n";
	return 0;
}

/*
int ClientRequest::createEvent(long manual, long initial, std::string& name, HANDLE* ev_ptr) {
	*ev_ptr = CreateEvent(NULL,manual,initial,name.c_str());
	return(*ev_ptr != NULL);
}

void ClientRequest::setCopied() {
	copied = TRUE;
}

int ClientRequest::createSharedMemory(std::string name, unsigned sizeLow, unsigned rights, unsigned mapRights, HANDLE* filemap_ptr, uint8_t** pptr) {
	int result;
	*filemap_ptr = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,rights,0,sizeLow,name.c_str());
	if (!(*filemap_ptr))
		result = FALSE;
	else{
		*pptr = (uint8_t*)  MapViewOfFile(*filemap_ptr,mapRights,0,0,sizeLow);
		result = (int)*pptr;
	}
	return result;
}
*/


/*
int ClientRequest::openEvent(long rights, std::string name, HANDLE* ev_ptr) {
	*ev_ptr = OpenEvent(rights,FALSE,name.c_str());
	return (int)*ev_ptr;
}

int ClientRequest::openMutex(long rights, std::string name, HANDLE* mutex_ptr) {
	*mutex_ptr = OpenMutex(rights,FALSE,name.c_str());
	return (int)*mutex_ptr;
}

int ClientRequest::openFileMapping(long rights, std::string name, HANDLE* fm_ptr) {
	*fm_ptr = OpenFileMapping(rights,FALSE,name.c_str());
	return (int)*fm_ptr;
}


void ClientRequest::initStatusWriters(std::map<int, StatusWriter*>& writers) {
	writers[REQ_RECEIVED] = new ReceivedStatusWriter();
	writers[REQ_CONV_IN_PROGRESS]= new InProgressStatusWriter();
	writers[REQ_COMPLETED]= new CompletedStatusWriter();
	writers[REQ_ABORTED] = new AbortedStatusWriter();
}

void ClientRequest::deinitStatusWriters(std::map<int,StatusWriter*>& writers) {
	for_each(writers.begin(),writers.end(),ClientRequest::deinitStatusWriter);
}
*/

/*
void __attribute__((__stdcall__)) ClientRequest::writeCompletedProc(DWORD dwErrorCode, DWORD dwNumberOfBytesTransferred,	LPOVERLAPPED lpOverlapped) {
	ClientRequest *ptr = (ClientRequest*) (*lpOverlapped).hEvent;
	(*ptr).checkThisWrite(dwErrorCode,dwNumberOfBytesTransferred);
}

void ClientRequest::checkThisWrite(unsigned long errorCode,	unsigned long bytesWritten) {
	if ((errorCode == 0) &&  (bytesWritten == bytesToWrite)) {
		offset.QuadPart +=  bytesWritten;
	}
	else {
		std::cerr<<"Error during file write error code="<<GetLastError()<<"\n";
		in_progress = 0;
	}
}

void ClientRequest::receiveHeaderData(unsigned& frames, unsigned long& size) {
	unsigned result = WaitForSingleObjectEx(writeCompleted,INFINITE,TRUE);
	if (result == WAIT_OBJECT_0) {
		HeaderDataStruct* header_ptr = (HeaderDataStruct*)  data_ptr;
		frames = (*header_ptr).frames_qty;
		size = (*header_ptr).frame_size;
	}
}
*/

std::string ClientRequest::getKey() {
	std::cerr<<"ClientRequest::getKey enter sourceName="<<sourceName<<" \ndestname="<<destName<<"\n";
	DebuggingTools::logPtr("ClientRequest::getKey",(unsigned)this);
	std::string result = sourceName;
//	return sourceName.append("->").append(destName);
	return result.append("->").append(destName);
}

int ClientRequest::registerRequest(ClientCommon& commonClient) {
	return commonClient.registerRequest(getKey(),this);
}

void ClientRequest::unregisterRequest(ClientCommon& commonClient) {
	commonClient.unregisterRequest(getKey());
//	std::cerr<<" ClientRequest::unregisterRequest exit\n";
}

std::string ClientRequest::getConvType() {
	return ClientCommon::getClientCommon().getConvTypeSt(type);
}

std::string ClientRequest::toString() {
	char st[MAX_PATH];
//	std::cerr<<"ClientRequest::toString enter\n";
	sprintf(st,"%s %d %d %s %s",sourceName.c_str(),width,height,getConvType().c_str(),destName.c_str());
//	std::cerr<<"ClientRequest::toString exit\n";
	return std::string(st);
}
