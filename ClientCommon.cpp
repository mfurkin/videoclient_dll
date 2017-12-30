/*
 * Client.cpp
 *
 *  Created on: 22 но€б. 2017 г.
 *
 */
#include "ClientCommon.h"
std::string ClientCommon::CLIENT_LOG_NAME = "Client";
std::string ClientCommon::CLIENT_COMMON_TAG = "ClientCommon";
std::string ClientCommon::CLIENT_LOG_DIR_NAME="\\videoclient";
std::string ClientCommon::CLIENT_LOG_FILE_NAME="\\videoclient.log";
ClientCommon::ClientCommon():createLoggerProc(NULL),logProc(NULL),logPtrProc(NULL),deleteLoggerProc(NULL) {

	errorsMutex = CreateMutex(NULL,TRUE,NULL);
	inProgressMutex = CreateMutex(NULL,TRUE,NULL);
	init();
}

ClientCommon::~ClientCommon() {
	saveState();
	for_each(errors.begin(),errors.end(),ClientCommon::deleteError);
	for_each(inProgress.begin(),inProgress.end(),ClientCommon::deleteInProgress);
	CloseHandle(errorsMutex);
	CloseHandle(inProgressMutex);
}

std::string ClientCommon::getErrorsLogFname() {

	std::string result;
	if (!(inited))
		result = "";
	else {
		static char res[MAX_PATH];
		getLogFnameProc((char*)res,(char*)CLIENT_LOG_DIR_NAME.c_str(),(char*)CLIENT_LOG_FILE_NAME.c_str(),NULL);
		result = std::string(res);
	}
	return result;
}


int ClientCommon::makeSureFileExists(const char* fname, int isFile) {
	HANDLE findPath;
	WIN32_FIND_DATA findData;
	findPath = FindFirstFile(fname,&findData);
	int result = (findPath != INVALID_HANDLE_VALUE);
	if (result) {
		std::string fname_st(fname);
		logString(CLIENT_COMMON_TAG,"Search successful fname:",fname_st);
	} else {
		unsigned long errCode = GetLastError();
		logPtr(CLIENT_COMMON_TAG,"Error during log opening error code=",errCode);
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

std::string ClientCommon::getLogPath(LoggerEngine* logger_ptr) {
	std::string result;
	if (!(fileUtilsInited))
		result = "";
	else {
		static char res[MAX_PATH];
		getLogPathProc((char*)res,(char*)CLIENT_LOG_DIR_NAME.c_str(),logger_ptr);
		result = std::string(res);
	}
	return result;
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

	std::string logFname = getErrorsLogFname();
	std::filebuf fb;
	const char* buf;
	initConvTypeMap();
	initLogger();
	initFileUtils();
	inited = initFileUtils() && initLogger();
	if (inited) {
		logString(CLIENT_COMMON_TAG,"ClientCommon::init pt1 logFname=",logFname);
		if (makeSureFileExists(logFname.c_str(),TRUE)) {

			log(CLIENT_COMMON_TAG,"ClientCommon::init pt2");
			fb.open(logFname.c_str(),std::ios::in);
			std::istream readLogFile(&fb);

			log(CLIENT_COMMON_TAG,"ClientCommon::init pt3");
			for(;(!(readLogFile.eof()));) {
				std::string st;

				log(CLIENT_COMMON_TAG,"ClientCommon::init pt4");
				std::getline(readLogFile,st);

				logString(CLIENT_COMMON_TAG,"ClientCommon::init pt4.1 st=",st);
				if (st.empty())
					break;

				log(CLIENT_COMMON_TAG,"ClientCommon::init pt5");
				buf = st.c_str();

				logString(CLIENT_COMMON_TAG,"ClientCommon::init pt5.1 st=",st);
				addThisErrorFromChar(buf);

				log(CLIENT_COMMON_TAG,"ClientCommon::init pt6");
			}

			log(CLIENT_COMMON_TAG,"ClientCommon::init pt7");
			fb.close();
		}
	}
	log(CLIENT_COMMON_TAG,"ClientCommon::init exit");

}

int ClientCommon::isInProgress(std::string key) {
	int result;
	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
	result = (inProgress.find(key) != inProgress.end());
	ReleaseMutex(inProgressMutex);
	return result;
}

void ClientCommon::deleteError(	std::pair<std::string, ErrorReport*> aPair) {
	delete  aPair.second;
}

void ClientCommon::deleteInProgress(std::pair<std::string, ClientRequest*> aPair) {
	delete aPair.second;
}

ErrorReport ClientCommon::getInfo(const char* buf) {

	const char* delims=" ";
	char* type_st;
	char date_st[30],source_name[MAX_PATH],dest_name[MAX_PATH];
	std::string time_st;
	time_t date;
	unsigned short width,height,type;
	strcpy(date_st,strtok((char*)buf,delims));

	date = getInteger(delims);

	time_st = getString(delims);
	strcat(date_st,time_st.c_str());
	std::string date_st2(date_st);
	strcpy(source_name, getString(delims));

	width = getInteger(delims);

	height = getInteger(delims);

	type_st = getString(delims);

	type = convTypes[type_st];

	strcpy(dest_name, getString(delims));

	ClientRequest req(source_name,dest_name,width,height,type);
	return ErrorReport(req,date,date_st2);
}

int ClientCommon::registerRequest(std::string key, ClientRequest* req_ptr) {
	int result;;

	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
	result = !(isInProgress(key));
	if (result)
		inProgress[key] = req_ptr;
	ReleaseMutex(inProgressMutex);
	return result;
}

void ClientCommon::unregisterRequest(std::string key) {
	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);

	ClientRequest* ptr = inProgress[key];
	delete ptr;
	inProgress.erase(key);

	ReleaseMutex(inProgressMutex);

}

void ClientCommon::sendNewRequest(ClientRequest& req) {
	int result;
	if (inited) {
		WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
			result = req.registerRequest(*this);
		ReleaseMutex(inProgressMutex);
		if (result)
			req.sendRequest();
		req.unregisterRequest(*this);
	}
}

void ClientCommon::repeatRequest() {
	if (inited) {
		WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);
			std::string st = errors_deque.front();
			ErrorReport* report = errors[st];
			(*report).sendRequest();
			errors_deque.pop_front();
		ReleaseMutex(errorsMutex);
	}
}

void ClientCommon::addThisErrorFromChar(const char* buf) {

	ErrorReport* req = new ErrorReport(getInfo(buf));

	(*req).addThisError(*this);

}


void ClientCommon::addError(std::string key, ClientRequest& req, time_t curTime, std::string dateSt) {
	ErrorReport* report = new ErrorReport(req,curTime,dateSt);
	addThisError(key,report);
}
void ClientCommon::addThisError(std::string key,  ErrorReport*  report) {
	WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);

		errors[key] = report;

		errors_deque.push_back(key);

	ReleaseMutex(errorsMutex);
}

int ClientCommon::isErrorQueueEmpty() {
	int result;
	if (!(inited))
		result = TRUE;
	else {
		WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);
		result = errors_deque.empty();
		ReleaseMutex(errorsMutex);
	}
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
}

void __attribute__((dllexport)) repeatError() {
	ClientCommon::getClientCommon().repeatRequest();
}

int __attribute__((dllexport)) errorsExist() {
	return (!(ClientCommon::getClientCommon().isErrorQueueEmpty()));
}

void ClientCommon::saveState() {
	std::filebuf fb;

	fb.open(getErrorsLogFname().c_str(),std::ios::out);
	std::ostream writeLogFile(&fb);

	WaitForSingleObjectEx(inProgressMutex,INFINITE,TRUE);
		std::map<std::string,ClientRequest*>::iterator it_progress,end_progress = inProgress.end();
		std::string key;

		for(it_progress  = inProgress.begin();it_progress != end_progress;) {
			key = (*it_progress).first;
			ClientRequest* ptr = (*it_progress++).second;

			(*ptr).currentErrorReport();

		}

	ReleaseMutex(inProgressMutex);

	WaitForSingleObjectEx(errorsMutex,INFINITE,TRUE);

	std::deque<std::string>::iterator it_errors,end_errors;
	end_errors = errors_deque.end();
	std::string curKey;
	for (it_errors=errors_deque.begin();it_errors!=end_errors;) {
		curKey = *it_errors++;

		ErrorReport* ptr = errors[curKey];
		logPtr(CLIENT_COMMON_TAG,"ClientCommon::saveState ptr=",(unsigned)ptr);
		writeLogFile<<(*ptr).toString()<<"\n";
	}
	log(CLIENT_COMMON_TAG,"savestate pt10");

	ReleaseMutex(errorsMutex);
	fb.close();
}

void __attribute__((dllexport)) saveState() {
	ClientCommon::getClientCommon().saveState();
}

std::string ClientCommon::getConvTypeSt(int type) {
	std::string typesSt[5] = {YUV420toRGB24_st,  YUV422toRGB24_st, RGB24toYUV420_st,  RGB24toYUV422_st, YUV420toYUV422_st};
	return typesSt[type-1];
}

int ClientCommon::initLogger() {
	int result = TRUE;
	HMODULE hDll = LoadLibrary("liblogger.dll");
	if (!(hDll))
		result = 0;
	else {
		createLoggerProc = (CreateLoggerProc) GetProcAddress(hDll,"createLogger");
		logProc = (LogProc) GetProcAddress(hDll,"log");
		logPtrProc = (LogPtrProc) GetProcAddress(hDll,"logPtr");
		deleteLoggerProc = (DeleteLoggerProc) GetProcAddress(hDll,"deleteLogger");
		std::string fname = getLogPath(NULL);
		fname.append("\\client.log");
		createLogger(CLIENT_LOG_NAME,fname);
	}
	return result;
}

void ClientCommon::createLogger(std::string& name, std::string& fname) {
	if (createLoggerProc)
		createLoggerProc(name.c_str(),fname.c_str());
}

int ClientCommon::initFileUtils() {
	int result = TRUE;
	HMODULE hDll = LoadLibrary("fileutils.dll");
	if (!(hDll))
		result = FALSE;
	else {
		getLogPathProc  = (GetLogPathProc) GetProcAddress(hDll,"getLogPath");
		getLogFnameProc = (GetLogFnameProc) GetProcAddress(hDll,"getLogFname");
		result = getLogPathProc && getLogFnameProc;
	}
	fileUtilsInited = result;
	return result;
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
