/*
 * Client.cpp
 *
 *  Created on: 22 но€б. 2017 г.
 *
 */
#include "ClientCommon.h"
std::string ClientCommon::CLIENT_LOG_NAME = "Client";
std::string ClientCommon::CLIENT_COMMON_TAG = "ClientCommon";
ClientCommon::ClientCommon():createLoggerProc(NULL),logProc(NULL),logPtrProc(NULL),deleteLoggerProc(NULL) {

	errorsMutex = CreateMutex(NULL,TRUE,NULL);
	inProgressMutex = CreateMutex(NULL,TRUE,NULL);
	init();
}

ClientCommon::~ClientCommon() {
//	std::cerr<<"ClientCommon dtor enter\n";
	saveState();
//	std::cerr<<"ClientCommon dtor pt1\n";
	for_each(errors.begin(),errors.end(),ClientCommon::deleteError);
//	std::cerr<<"ClientCommon dtor pt2\n";
	for_each(inProgress.begin(),inProgress.end(),ClientCommon::deleteInProgress);
//	std::cerr<<"ClientCommon dtor pt3\n";
	CloseHandle(errorsMutex);
	CloseHandle(inProgressMutex);
//	std::cerr<<"ClientCommon dtor exit\n";
}

std::string ClientCommon::getLogFname() {
	char path[MAX_PATH];
	strcpy(path,getLogPath().c_str());
	strcat(path,"\\videoclient.log");
	return path;
}


int ClientCommon::makeSureFileExists(const char* fname, int isFile) {
	HANDLE findPath;
	WIN32_FIND_DATA findData;
	findPath = FindFirstFile(fname,&findData);
	int result = (findPath != INVALID_HANDLE_VALUE);
	if (result) {
//		std::cerr<<"Search successful fname:"<<fname<<"\n";
		std::string fname_st(fname);
		logString(CLIENT_COMMON_TAG,"Search successful fname:",fname_st);
	} else {
		unsigned long errCode = GetLastError();
		logPtr(CLIENT_COMMON_TAG,"Error during log opening error code=",errCode);
//		std::cerr<<"Error during log opening error code="<<errCode<<"\n";
		if (!(isFile))
			CreateDirectory(fname,NULL);
		else {
			HANDLE file;
			file = CreateFile(fname,GENERIC_WRITE | GENERIC_READ,0,NULL,CREATE_NEW,0,NULL);
			if (file != INVALID_HANDLE_VALUE)
				CloseHandle(file);
			else {
				errCode = GetLastError();
				std::cerr<<"Log file creating error, error code="<<GetLastError()<<"\n";
			}
		}
	}
	FindClose(findPath);
	return result;
}

std::string ClientCommon::getLogPath() {
	char path[MAX_PATH];

	SHGetFolderPath(NULL,CSIDL_APPDATA,NULL,SHGFP_TYPE_CURRENT,path);
	makeSureFileExists(path,FALSE);
	strcat(path,"\\videoclient");
	makeSureFileExists(path,FALSE);
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
unsigned ClientCommon::getTypeId(std::string convSt) {
	return convTypes.count(convSt) ? convTypes[convSt] : 0;
}
unsigned short ClientCommon::getKeySize() {
	return MAX_PATH*2 + 3;
}

void ClientCommon::init() {
//	std::cerr<<"ClientCommon::init enter\n";
	std::string logFname = getLogFname();
	std::filebuf fb;
	const char* buf;
	initConvTypeMap();
	initLogger();
//	std::cerr<<"ClientCommon::init pt1 logFname="<<logFname<<"\n";
	logString(CLIENT_COMMON_TAG,"ClientCommon::init pt1 logFname=",logFname);
	if (makeSureFileExists(logFname.c_str(),TRUE)) {
//		std::cerr<<"ClientCommon::init pt2\n";
		log(CLIENT_COMMON_TAG,"ClientCommon::init pt2");
		fb.open(logFname.c_str(),std::ios::in);
		std::istream readLogFile(&fb);
//		std::cerr<<"ClientCommon::init pt3\n";
		log(CLIENT_COMMON_TAG,"ClientCommon::init pt3");
		for(;(!(readLogFile.eof()));) {
			std::string st;
//			std::cerr<<"ClientCommon::init pt4\n";
			log(CLIENT_COMMON_TAG,"ClientCommon::init pt4");
			std::getline(readLogFile,st);
//			std::cerr<<"ClientCommon::init pt4.1 st="<<st<<"\n";
			logString(CLIENT_COMMON_TAG,"ClientCommon::init pt4.1 st=",st);
			if (st.empty())
				break;
//			std::cerr<<"ClientCommon::init pt5\n";
			log(CLIENT_COMMON_TAG,"ClientCommon::init pt5");
			buf = st.c_str();
//			std::cerr<<"ClientCommon::init pt5.1 st="<<st<<"\n";
			logString(CLIENT_COMMON_TAG,"ClientCommon::init pt5.1 st=",st);
			addThisErrorFromChar(buf);
//			std::cerr<<"ClientCommon::init pt6\n";
			log(CLIENT_COMMON_TAG,"ClientCommon::init pt6");
		}
//		std::cerr<<"ClientCommon::init pt7\n";
		log(CLIENT_COMMON_TAG,"ClientCommon::init pt7");
		fb.close();
	}
	log(CLIENT_COMMON_TAG,"ClientCommon::init exit");
//	std::cerr<<"ClientCommon::init exit\n";
}

int ClientCommon::isInProgress(std::string key) {
	int result;
	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
//	result = (readers.find(key) != readers.end());
	result = (inProgress.find(key) != inProgress.end());
	ReleaseMutex(inProgressMutex);
	return result;
}

void ClientCommon::deleteError(	std::pair<std::string, ErrorReport*> aPair) {
//	logPtr((char*)"deleteError",(unsigned)aPair.second);
	delete  aPair.second;
}

void ClientCommon::deleteInProgress(std::pair<std::string, ClientRequest*> aPair) {
//	std::cerr<<"deleteInProgress enter\n";
//	logPtr("deleteInProgress",(unsigned)aPair.second);
	delete aPair.second;
//	std::cerr<<"deleteInProgress exit\n";
}

ErrorReport ClientCommon::getInfo(const char* buf) {

	const char* delims=" ";
	char* type_st;
	char date_st[30],source_name[MAX_PATH],dest_name[MAX_PATH];
	std::string time_st;
	time_t date;
	unsigned short width,height,type;
	strcpy(date_st,strtok((char*)buf,delims));
//	std::cerr<<"ClientCommon::getInfo date_st="<<date_st<<"\n";
	date = getInteger(delims);
//	std::cerr<<"ClientCommon::getInfo time_st="<<time_st<<"\n";
	time_st = getString(delims);
	strcat(date_st,time_st.c_str());
	std::string date_st2(date_st);
	strcpy(source_name, getString(delims));
//	std::cerr<<"ClientCommon::getInfo source_name="<<source_name<<"\n";
	width = getInteger(delims);
//	std::cerr<<"ClientCommon::getInfo width="<<width<<"\n";
	height = getInteger(delims);
//	std::cerr<<"ClientCommon::getInfo height="<<height<<"\n";
	type_st = getString(delims);
//	std::cerr<<"ClientCommon::getInfo type_st="<<type_st<<"\n";
	type = convTypes[type_st];
//	std::cerr<<"ClientCommon::getInfo type="<<type<<"\n";
	strcpy(dest_name, getString(delims));
//	std::cerr<<"ClientCommon::getInfo before ClientRequest ctor call"<<source_name<<"\n";
	ClientRequest req(source_name,dest_name,width,height,type);
	return ErrorReport(req,date,date_st2);
}

int ClientCommon::registerRequest(std::string key, ClientRequest* req_ptr) {
	int result;;
//	(*req_ptr).sendRequest();
	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
	result = !(isInProgress(key));
	if (result)
		inProgress[key] = req_ptr;
	ReleaseMutex(inProgressMutex);
	return result;
}

void ClientCommon::unregisterRequest(std::string key) {
	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
//	std::cerr<<"ClientCommon::unregisterRequest  key="<<key<<"\n";
	ClientRequest* ptr = inProgress[key];
	delete ptr;
//	std::cerr<<"ClientCommon::unregisterRequest  pt2\n";
//	std::cerr<<"ClientCommon::unregisterRequest  pt3\n";
	inProgress.erase(key);
//	std::cerr<<"ClientCommon::unregisterRequest  pt4\n";
	ReleaseMutex(inProgressMutex);
//	std::cerr<<"ClientCommon::unregisterRequest  exit\n";
}

void ClientCommon::sendNewRequest(ClientRequest& req) {
	int result;
	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
	result = req.registerRequest(*this);
	ReleaseMutex(inProgressMutex);
	if (result)
		req.sendRequest();
	req.unregisterRequest(*this);
}

void ClientCommon::repeatRequest() {
	WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);
	std::string st = errors_deque.front();
	ErrorReport* report = errors[st];
	(*report).sendRequest();
	errors_deque.pop_front();
//	errors_queue.pop();
	ReleaseMutex(errorsMutex);
}

void ClientCommon::addThisErrorFromChar(const char* buf) {
//	std::cerr<<"ClientCommon::addThisErrorFromChar enter\n";
	ErrorReport* req = new ErrorReport(getInfo(buf));
//	std::cerr<<"ClientCommon::addThisErrorFromChar pt1\n";
	(*req).addThisError(*this);
//	std::cerr<<"ClientCommon::addThisErrorFromChar exit\n";
}


void ClientCommon::addError(std::string key, ClientRequest& req, time_t curTime, std::string dateSt) {
	ErrorReport* report = new ErrorReport(req,curTime,dateSt);
	addThisError(key,report);
}
void ClientCommon::addThisError(std::string key,  ErrorReport*  report) {
	WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);
//		std::cerr<<"ClientCommon::addThisError enter\n";
		errors[key] = report;
//		std::cerr<<"ClientCommon::addThisError pt1\n";
		errors_deque.push_back(key);
//		std::cerr<<"ClientCommon::addThisError exit\n";
	ReleaseMutex(errorsMutex);
}

int ClientCommon::isErrorQueueEmpty() {
	WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);
		int result = errors_deque.empty();
	ReleaseMutex(errorsMutex);
	return result;

}
char* getFileName(char* buf) {
	unsigned long result;
	static char fullPathName[MAX_PATH];
	char* fileName;
	result = GetFullPathName(buf,MAX_PATH,fullPathName,&fileName);
	return (!(result)) ? NULL : fullPathName;
}

unsigned short getNumber(char* buf) {
	return atoi(buf);
}
//<source file> <width> <height> <transform_function> <output file>
ClientRequest* buildClientRequest(char* params[], ClientCommon& commonClient) {
	ClientRequest* ptr;
	char* sourceFNamePtr = getFileName(params[SOURCE_NAME_NUMBER]);
	if (sourceFNamePtr == NULL)
		ptr = NULL;
	else {
		std::string sourceName = std::string(sourceFNamePtr);
		unsigned short width = getNumber(params[WIDTH_NUMBER]),
		height = getNumber(params[HEIGHT_NUMBER]);
		if ((!(width)) || (!(height)))
			ptr = NULL;
		else{
			unsigned short type = commonClient.getTypeId(std::string(params[TYPE_NUMBER]));
			if (!(type))
				ptr = NULL;
			else {
				char destFileCh[MAX_PATH];
				char* fileName;
				GetFullPathName(params[DEST_FILE_NUMBER],MAX_PATH,destFileCh,&fileName);
				std::string destFile = std::string(destFileCh);
//				std::cerr<<"ClientCommon::buildClientRequest before ClientRequest ctor sourceName="<<sourceName<<"\n";
				ptr = new ClientRequest(sourceName,destFile,width,height,type);
			}
		}
	}
	return ptr;
}

void __attribute__((dllexport)) sendRequest(char* params[]) {
	ClientCommon commonClient = ClientCommon::getClientCommon();
	ClientRequest* req_ptr = buildClientRequest(params,commonClient);
	commonClient.sendNewRequest(*req_ptr);

//	delete req_ptr;
}

void __attribute__((dllexport)) repeatError() {
	ClientCommon::getClientCommon().repeatRequest();
}

int __attribute__((dllexport)) errorsExist() {
	return (!(ClientCommon::getClientCommon().isErrorQueueEmpty()));
}
/*
void ClientCommon::saveThisError(std::string key) {
	ErrorReport* ptr = errors[key];
	std::string  error_st = (*ptr).toString();

}
*/
void ClientCommon::saveState() {
	std::filebuf fb;
//	std::cerr<<"savestate enter\n";
	fb.open(getLogFname().c_str(),std::ios::out);
	std::ostream writeLogFile(&fb);
//	std::cerr<<"savestate pt1\n";
	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
		std::map<std::string,ClientRequest*>::iterator it_progress,end_progress = inProgress.end();
		std::string key;

		for(it_progress  = inProgress.begin();it_progress != end_progress;) {
			key = (*it_progress).first;
			ClientRequest* ptr = (*it_progress++).second;
//			std::cerr<<"savestate pt2\n";
			(*ptr).currentErrorReport();
//			std::cerr<<"savestate pt3\n";
		}
//	std::cerr<<"savestate pt4\n";
	ReleaseMutex(inProgressMutex);
//	std::cerr<<"savestate pt5\n";
	WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);
//	std::cerr<<"savestate pt6\n";
	std::deque<std::string>::iterator it_errors,end_errors;
	end_errors = errors_deque.end();
	std::string curKey;
	for (it_errors=errors_deque.begin();it_errors!=end_errors;) {
		curKey = *it_errors++;
//		std::cerr<<"savestate pt7 curKey="<<curKey<<"\n";
		ErrorReport* ptr = errors[curKey];
//		ErrorReport* ptr = errors[*it_errors++];
//		std::cerr<<"savestate pt8\n";
//		DebuggingTools::logPtr("ClientCommon::saveState ptr=",(unsigned)ptr);
		logPtr(CLIENT_COMMON_TAG,"ClientCommon::saveState ptr=",(unsigned)ptr);
//		std::cerr<<"savestate pt8.1\n";
		writeLogFile<<(*ptr).toString()<<"\n";
//		std::cerr<<"savestate pt9\n";
	}
	log(CLIENT_COMMON_TAG,"savestate pt10");
//	std::cerr<<"savestate pt10\n";
	ReleaseMutex(errorsMutex);
//	std::cerr<<"savestate pt11\n";
	fb.close();
//	std::cerr<<"savestate exit\n";
}

void __attribute__((dllexport)) saveState() {
	ClientCommon::getClientCommon().saveState();
}

std::string ClientCommon::getConvTypeSt(int type) {
//	std::cerr<<"ClientCommon::getConvTypeSt type="<<type<<"\n";
	std::string typesSt[5] = {YUV420toRGB24_st,  YUV422toRGB24_st, RGB24toYUV420_st,  RGB24toYUV422_st, YUV420toYUV422_st};
	return typesSt[type-1];
}

void ClientCommon::initLogger() {
	HMODULE hDll = LoadLibrary("liblogger.dll");
	createLoggerProc = (CreateLoggerProc) GetProcAddress(hDll,"createLogger");
	logProc = (LogProc) GetProcAddress(hDll,"log");
	logPtrProc = (LogPtrProc) GetProcAddress(hDll,"logPtr");
	deleteLoggerProc = (DeleteLoggerProc) GetProcAddress(hDll,"deleteLogger");
	std::string fname = getLogPath();
	fname.append("\\client.log");
	createLogger(CLIENT_LOG_NAME,fname);
}

void ClientCommon::createLogger(std::string& name, std::string& fname) {
	if (createLoggerProc)
		createLoggerProc(name.c_str(),fname.c_str());
}

void ClientCommon::deleteLogger(std::string& name) {
	if (deleteLoggerProc)
		deleteLoggerProc(name.c_str());
}

void ClientCommon::log(std::string& tag, std::string msg) {
	if (logProc)
		logProc(CLIENT_LOG_NAME.c_str(),tag.c_str(),msg.c_str());
}

void ClientCommon::logPtr(std::string& tag, std::string msg, unsigned ptr) {
	if  (logPtrProc)
		logPtrProc(CLIENT_LOG_NAME.c_str(),tag.c_str(),msg.c_str(),ptr);
}

void ClientCommon::logString( std::string& tag, std::string msg, std::string& msg2) {
	std::string msgAll = msg.append(msg2);
	log(tag,msgAll);
}
