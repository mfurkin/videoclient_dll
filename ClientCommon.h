/*
 * Client.h
 *
 *  Created on: 22 нояб. 2017 г.
 *      Author: Михаил
 */

#ifndef SRC_CLIENT_H_
#define SRC_CLIENT_H_

#define _WIN32_IE 0x0500
#include <algorithm>
#include <ctime>
#include <map>
#include <set>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <shlobj.h>
#include <queue>
#include <string>
#include <wtypes.h>
#include <winbase.h>
#include <windef.h>
#include <windows.h>
// #include "DebuggingTools.h"
#include "FileUtils.h"
#include "ErrorReport.h"
#include "LoggerTypes.h"
#include "LoggerEngine.h"
#include "server_names.h"

class ClientRequest;

class ErrorReport;

class ClientCommon : public LoggerEngine {
public:
	static ClientCommon& getClientCommon();
	static void deleteError(std::pair<std::string,ErrorReport*> aPair);
	static void deleteInProgress(std::pair<std::string, ClientRequest*> aPair);
	unsigned getTypeId(std::string convSt);
	virtual ~ClientCommon();
	void addError(std::string key, ClientRequest& request, time_t time, std::string date_st);			// сообщить об ошибке
	void addThisError(std::string key,  ErrorReport*  report);
	int isErrorQueueEmpty();
	void sendNewRequest(ClientRequest& req);  		//послать новый запрос
	void repeatRequest();									// повторить неудачный запрос
	int registerRequest(std::string key, ClientRequest* req);
	void unregisterRequest(std::string key);
	int isInProgress(std::string key);
	void saveState();
	std::string getConvTypeSt(int type);
	void log(std::string& tag,  std::string msg);
	void logPtr(std::string& tag, std::string msg, unsigned ptr);
	void logString(std::string& tag, std::string msg, std::string& msg2);
private:
	static std::string CLIENT_LOG_NAME,CLIENT_COMMON_TAG,CLIENT_LOG_DIR_NAME,CLIENT_LOG_FILE_NAME;
	ClientCommon();
	int initLogger();
	void initConvTypeMap();
	int initFileUtils();
	int makeSureFileExists(const char* fname, int isFile);
	std::string getErrorsLogFname();
	std::string getLogPath(LoggerEngine* logger_ptr);
	unsigned short getKeySize();
	void init();
	void addThisErrorFromChar(const char* buf);
	void createLogger(std::string& name, std::string& fname);
	void deleteLogger(std::string& name);

	ErrorReport getInfo(const char* buf);

	HANDLE errorsMutex,inProgressMutex;
	std::map<std::string,ClientRequest*> inProgress;
	std::map<std::string,ErrorReport*> errors;
	std::map<std::string,unsigned> convTypes;
	std::deque<std::string> errors_deque;
	static std::ostream* writeLog_ptr;
	int inited,fileUtilsInited;
	CreateLoggerProc createLoggerProc;
	LogProc logProc;
	LogPtrProc logPtrProc;
	DeleteLoggerProc deleteLoggerProc;
	GetLogPathProc getLogPathProc;
	GetLogFnameProc getLogFnameProc;
};
ClientRequest* buildClientRequest(char* params[]);
enum { SOURCE_NAME_NUMBER=1, WIDTH_NUMBER, HEIGHT_NUMBER, TYPE_NUMBER, DEST_FILE_NUMBER };
//<source file> <width> <height> <transform_function> <output file>
extern "C" __declspec(dllexport) void sendRequest(char* params[]);
extern "C" __declspec(dllexport) void repeatError();
extern "C" __declspec(dllexport) int errorsExist();
extern "C" __declspec(dllexport) void saveState();
#endif /* CLIENT_H_ */
