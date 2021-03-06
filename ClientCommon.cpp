/*
 * Client.cpp
 *
 *  Created on: 22 ����. 2017 �.
 *
 */
#include "ClientCommon.h"

ClientCommon::ClientCommon() {

	errorsMutex = CreateMutex(NULL,TRUE,NULL);
	inProgressMutex = CreateMutex(NULL,TRUE,NULL);
	init();
}

ClientCommon::~ClientCommon() {
	for_each(errors.begin(),errors.end(),ClientCommon::deleteError);
	for_each(inProgress.begin(),inProgress.end(),ClientCommon::deleteInProgress);
	CloseHandle(errorsMutex);
	CloseHandle(inProgressMutex);
}

std::string ClientCommon::getLogFname() {
	char path[MAX_PATH];
	strcpy(path,getLogPath().c_str());
	strcat(path,"\videoclient.log");
	return path;
}

std::string ClientCommon::getLogPath() {
	char path[MAX_PATH];
	HANDLE file;
	SHGetFolderPath(NULL,CSIDL_APPDATA,NULL,SHGFP_TYPE_CURRENT,path);
	strcat(path,"\videoclient");
	file = CreateFile(path,GENERIC_WRITE | GENERIC_READ,0,NULL,OPEN_ALWAYS,0,NULL);
	CloseHandle(file);
	return path;
}

char* getString(const char* delims) {
	return strtok(NULL,delims);
}
unsigned getInteger(const char* delims) {
	char* ptr = strtok(NULL,delims);
	return (ptr == NULL) ? 0 : atoi(ptr);
}

ClientCommon& ClientCommon::getClientCommon() {
	static ClientCommon client_common;

	return client_common;
}

void ClientCommon::initConvTypeMap() {
	convTypes[YUV420toRGB24_st] = YUV420toRGB24;
	convTypes[YUV422toRGB24_st] = YUV422toRGB24;
	convTypes[RGB24toYUV420_st] = RGB24toYUV420;
	convTypes[RGB24toYUV422_st] = RGB24toYUV422;
	convTypes[YUV420toYUV422_st] = YUV420toYUV422;
}

unsigned short ClientCommon::getKeySize() {
	return MAX_PATH*2 + 3;
}

void ClientCommon::init() {
	std::string logFname = getLogFname();
	std::filebuf fb;
	const char* buf;
	initConvTypeMap();
	fb.open(logFname.c_str(),std::ios::in);
	std::istream readLogFile(&fb);
//	char source_file[MAX_PATH], dest_file[MAX_PATH];
	for(;(!(readLogFile.eof()));) {
		std::string st;
		std::getline(readLogFile,st);
		buf = st.c_str();
		addThis(buf);
	}


}

int ClientCommon::isInProgress(std::string key) {
	int result;
	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
//	result = (readers.find(key) != readers.end());
	result = (inProgress.find(key) != inProgress.end());
	ReleaseMutex(inProgressMutex);
	return result;
}

void ClientCommon::deleteError(	std::pair<std::string, RequestBuilder*> aPair) {
	delete  aPair.second;
}

void ClientCommon::deleteInProgress(std::pair<std::string, ClientRequest*> aPair) {
	delete aPair.second;
}

RequestBuilder ClientCommon::getInfo(const char* buf) {

	const char* delims=" ";
	char* type_st;
	char date_st[15],source_name[MAX_PATH],dest_name[MAX_PATH];
	time_t date;
	unsigned short width,height,type;
	strcpy(date_st,strtok((char*)buf,delims));
	date = getInteger(delims);
	strcpy(source_name, getString(delims));
	width = getInteger(delims);
	height = getInteger(delims);
	type_st = getString(delims);
	type = convTypes[type_st];
	strcpy(dest_name, getString(delims));

	return RequestBuilder(source_name,dest_name,width,height,type,date_st,date);
}
void ClientCommon::startNewRequest(std::string key, ClientRequest* req_ptr) {
	(*req_ptr).sendRequest();
	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
	inProgress[key] = req_ptr;
	ReleaseMutex(inProgressMutex);
}
ClientRequest* ClientCommon::startNewRequest(RequestBuilder& req) {
	ClientRequest* req_ptr = NULL;
	if (!(req.isRun(*this)))
		req_ptr = req.startNewRequest(*this);
	return req_ptr;
}

ClientRequest ClientCommon::getRepeatRequest() {
	WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);
	std::string st = errors_queue.front();
	RequestBuilder* builder = errors[st];
	ClientRequest* req_ptr = (*builder).startNewRequest(*this);
	errors_queue.pop();
	ReleaseMutex(errorsMutex);
	return *req_ptr;
}

void ClientCommon::addThis(const char* buf) {
	RequestBuilder req = getInfo(buf);
	req.errorReport(*this);
}

void ClientCommon::addError(std::string key, RequestBuilder* req) {
	WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);
	errors[key] = req;
	errors_queue.push(key);
	ReleaseMutex(errorsMutex);
}

//<source file> <width> <height> <transform_function> <output file>
