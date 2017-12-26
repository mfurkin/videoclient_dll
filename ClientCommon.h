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
#include "ErrorReport.h"
#include "LoggerTypes.h"
#include "LoggerEngine.h"
#include "server_names.h"
/*
typedef struct {
	unsigned short width,height;
	unsigned char type;
	char source_name[MAX_PATH],dest_name[MAX_PATH];
} RequestClientDataStruct;
typedef struct {
	RequestClientDataStruct req;
	char date_st[15];
	time_t date;
} LoggingDataStruct;

const std::string YUV420toRGB24_st="YUV420toRGB24";
const std::string YUV422toRGB24_st="YUV422toRGB24";
const std::string RGB24toYUV420_st = "RGB24toYUV420";
const std::string RGB24toYUV422_st = "RGB24toYUV422";
const std::string YUV420toYUV422_st = "YUV420toYUV422";

*/
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
	static std::string CLIENT_LOG_NAME,CLIENT_COMMON_TAG;
	ClientCommon();
	void initLogger();
	void initConvTypeMap();
	int makeSureFileExists(const char* fname, int isFile);
	std::string getLogFname();
	std::string getLogPath();
	unsigned short getKeySize();
	void init();
	void addThisErrorFromChar(const char* buf);
	void createLogger(std::string& name, std::string& fname);
	void deleteLogger(std::string& name);

//	std::string getTimeSt(time_t* time);
	ErrorReport getInfo(const char* buf);

	HANDLE errorsMutex,inProgressMutex;
//	std::map<std::string,ClientRequest*> readers;
	std::map<std::string,ClientRequest*> inProgress;
	std::map<std::string,ErrorReport*> errors;
	std::map<std::string,unsigned> convTypes;
//	std::queue<std::string> errors_queue;
	std::deque<std::string> errors_deque;
	static std::ostream* writeLog_ptr;
	CreateLoggerProc createLoggerProc;
	LogProc logProc;
	LogPtrProc logPtrProc;
	DeleteLoggerProc deleteLoggerProc;
};
ClientRequest* buildClientRequest(char* params[]);
enum { SOURCE_NAME_NUMBER=1, WIDTH_NUMBER, HEIGHT_NUMBER, TYPE_NUMBER, DEST_FILE_NUMBER };
//<source file> <width> <height> <transform_function> <output file>
extern "C" __declspec(dllexport) void sendRequest(char* params[]);
extern "C" __declspec(dllexport) void repeatError();
extern "C" __declspec(dllexport) int errorsExist();
extern "C" __declspec(dllexport) void saveState();
#endif /* CLIENT_H_ */
