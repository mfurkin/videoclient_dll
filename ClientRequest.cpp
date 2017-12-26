/*
 * ClientRequest.cpp
 *
 *  Created on: 25 но€б. 2017 г.
 *
 */

#include "ClientRequest.h"
#include "ClientCommon.h"
std::string ClientRequest::PING_THREAD_TAG = "Ping_thread";
std::string ClientRequest::WORKING_THREAD_TAG = "Working_thread";
std::string ClientRequest::GETKEY_TAG="ClientRequest::getKey";
std::string ClientRequest::DTOR_TAG="ClientRequest::desctructor";
std::string ClientRequest::CURRENT_ERROR_REPORT_TAG="ClientRequest::currentErrorReport";
ClientCommon* ClientRequest::clientCommon_ptr = NULL;
// ClientCommon& ClientRequest::clCommon = ClientCommon::getClientCommon();
ClientRequest::ClientRequest():	width(0),height(0),type(0), sourceName(std::string("empty sourceName")), destName(std::string("empty destName")),
								engine_ptr(NULL),in_progress(0) {
}

ClientRequest::ClientRequest(const ClientRequest& aRequest):width(aRequest.width),height(aRequest.height),type(aRequest.type),
															sourceName(aRequest.sourceName), destName(aRequest.destName),
															engine_ptr(aRequest.engine_ptr),in_progress(aRequest.in_progress)
{
}

ClientRequest::ClientRequest(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight,
							 unsigned short aType): width(aWidth),height(aHeight),type(aType),sourceName(aSourceName),destName(aDestName),
									 	 	 	 	engine_ptr(NULL) {
}
ClientRequest::~ClientRequest() {
	logPtr(DTOR_TAG, "ClientRequest dtor, this:",(unsigned)this);
//	DebuggingTools::logPtr("ClientRequest dtor, this:",(unsigned)this);
}


void ClientRequest::sendRequest() {
	unsigned result;
	engine_ptr = new PingThreadEngine(width,height,type,clientCommon_ptr);
	if (!((*engine_ptr).initServer(sourceName))) {
		log(PING_THREAD_TAG,"ClientRequest::sendRequest before currentErrorReport call");
		currentErrorReport();
		log(PING_THREAD_TAG,"ClientRequest::sendRequest after currentErrorReport call");
	} else {
		unsigned i = 0;
			unsigned short answered;
			in_progress = 1;
			(*engine_ptr).startProcessingThread(processRequest,this);
			log(PING_THREAD_TAG,"ClientRequest::sendRequest before loop");
			answered = 0;
			switch(i) {
				for (;in_progress && (result = (*engine_ptr).getResponseMessage(&answered));) {
					if (answered) {
						(*engine_ptr).processMessage(&in_progress);
						(*engine_ptr).outputMessage();
						answered = 0;
					}
				case 0:
					(*engine_ptr).waitNext();
				}
			}
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
//	DebuggingTools::logPtr("currentErrorReport this:",(unsigned)this);
	logPtr(CURRENT_ERROR_REPORT_TAG,"currentErrorReport this:",(unsigned)this);
	(*clientCommon_ptr).addError(key,*this,curTime,dateSt);
//	ClientCommon::getClientCommon().addError(key,*this,curTime,dateSt);
//	ClientCommon::getClientCommon().addError(getKey(),*this,curTime,dateSt);
//	std::cerr<<"currentErrorReport exit\n";
}
/*
std::string ClientRequest::generateName(std::string& first,	std::string second) {

	return first.append(second);
}
*/

unsigned __attribute__((__stdcall__)) ClientRequest::processRequest(void* p) {
	ClientRequest* ptr = (ClientRequest*) p;
	return (*ptr).processThisRequest();
}

unsigned ClientRequest::processThisRequest() {
//	std::cerr<<"ClientRequest::processThisRequest enter\n";
	log(WORKING_THREAD_TAG,"ClientRequest::processThisRequest enter");
	WorkingThreadEngine engine = (*engine_ptr).getWorkingThreadEngine(&in_progress,destName);
	engine.writeConvertedFile(in_progress);
	log(WORKING_THREAD_TAG,"ClientRequest::processThisRequest exit");
//	std::cerr<<"ClientRequest::processThisRequest exit\n";
	return 0;
}

std::string ClientRequest::getKey() {
//	std::cerr<<"ClientRequest::getKey enter sourceName="<<sourceName<<" destname="<<destName<<"\n";
	logString(GETKEY_TAG,"ClientRequest::getKey enter sourceName=",sourceName);
	logString(GETKEY_TAG,"ClientRequest::getKey enter destname=",destName);
	logPtr(GETKEY_TAG,"ClientRequest::getKey",(unsigned)this);
//S	DebuggingTools::logPtr("ClientRequest::getKey",(unsigned)this);
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
	return (*clientCommon_ptr).getConvTypeSt(type);
}

std::string ClientRequest::toString() {
	char st[MAX_PATH];
//	std::cerr<<"ClientRequest::toString enter\n";
	sprintf(st,"%s %d %d %s %s",sourceName.c_str(),width,height,getConvType().c_str(),destName.c_str());
//	std::cerr<<"ClientRequest::toString exit\n";
	return std::string(st);
}

void ClientRequest::log(std::string& tag, std::string msg) {
	(*clientCommon_ptr).log(tag,msg);
}

void ClientRequest::logPtr(std::string& tag, std::string msg, unsigned ptr) {
	(*clientCommon_ptr).logPtr(tag,msg,ptr);
}

void ClientRequest::logString(std::string& tag,	std::string msg, std::string& msg2) {
	(*clientCommon_ptr).logString(tag,msg,msg2);
}

void ClientRequest::setClientCommon(ClientCommon* aClientCommon) {
	clientCommon_ptr = aClientCommon;
}
