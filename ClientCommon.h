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
#include <map>
#include  <set>
#include <fstream>
#include <ios>
#include <iostream>
#include <shlobj.h>
#include <queue>
#include <string>
#include <wtypes.h>
#include <winbase.h>
#include <windef.h>
#include "RequestBuilder.h"
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
*/
const std::string YUV420toRGB24_st="YUV420toRGB24";
const std::string YUV422toRGB24_st="YUV422toRGB24";
const std::string RGB24toYUV420_st = "RGB24toYUV420";
const std::string RGB24toYUV422_st = "RGB24toYUV422";
const std::string YUV420toYUV422_st = "YUV420toYUV422";

class ClientRequest;

class RequestBuilder;

class ClientCommon{
public:
	static ClientCommon& getClientCommon();
	static void deleteError(std::pair<std::string,RequestBuilder*> aPair);
	static void deleteInProgress(std::pair<std::string, ClientRequest*> aPair);
	virtual ~ClientCommon();
	void addError(std::string key, RequestBuilder* req);			// сообщить об ошибке
	ClientRequest* startNewRequest(RequestBuilder& req);  		//послать новый запрос
	void startNewRequest(std::string key, ClientRequest* req);
	ClientRequest getRepeatRequest();									// повторить неудачный запрос
	void deleteHandle(std::string destName);
	int isInProgress(std::string key);
private:
	ClientCommon();
	void initConvTypeMap();
	std::string getLogFname();
	std::string getLogPath();
	unsigned short getKeySize();
	void init();
	void addThis(const char* buf);
	RequestBuilder getInfo(const char* buf);

	HANDLE errorsMutex,inProgressMutex;
//	std::map<std::string,ClientRequest*> readers;
	std::map<std::string,ClientRequest*> inProgress;
	std::map<std::string,RequestBuilder*> errors;
	std::map<std::string,unsigned> convTypes;
	std::queue<std::string> errors_queue;
};

#endif /* CLIENT_H_ */
