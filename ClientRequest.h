/*
 * ClientRequest.h
 *
 *  Created on: 25 но€б. 2017 г.
 *
 */

#ifndef CLIENTREQUEST_H_
#define CLIENTREQUEST_H_
#include <map>
#include <process.h>
#include <stdio.h>

#include <string>
#include <time.h>

#include "ClientCommon.h"
#include "RequestBuilder.h"
#include "server_names.h"
#include "AbortedStatusWriter.h"
#include "CompletedStatusWriter.h"
#include "InProgressStatusWriter.h"
#include "ReceivedStatusWriter.h"

class ClientCommon;

class RequestBuilder;

class ClientRequest {
public:
	ClientRequest(ClientRequest& aRequest);
	ClientRequest(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight, unsigned short aType);
	ClientRequest(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight, unsigned short aType, RequestBuilder* req_ptr);
	static VOID CALLBACK writeCompletedProc( DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
	static unsigned WINAPI processRequest(void* ptr);
	virtual ~ClientRequest();
	void sendRequest();
private:
	void initStatusWriters(std::map<int,StatusWriter*>& writers);
	void deinitStatusWriters(std::map<int,StatusWriter*>& writers);
	void checkThisWrite(unsigned long  errorCode, unsigned long bytesWritten);
	void receiveHeaderData(unsigned& frames, unsigned long& size);
	static void deinitStatusWriter(std::pair<int,StatusWriter*> aPair);
	void errorReport();
//	HANDLE writeRequest();
	unsigned processThisRequest();
	int createEvent(long manual, long initial, std::string& name, HANDLE* ev_ptr);
	int openEvent(long rights, std::string name, HANDLE* ev_ptr);
	int openMutex(long rights, std::string name, HANDLE* ev_ptr);
	int openFileMapping(long rights, std::string name, HANDLE* fm_ptr);
	int createSharedMemory(std::string name, unsigned sizeLow, unsigned rights, unsigned MapRights, HANDLE* filemap_ptr, uint8_t** pptr);
	void closeHandle(HANDLE aHandle);
	std::string generateName(std::string& first, std::string second);
	void setCopied();
	void init();
	RequestBuilder*  buildRequestBuilder();
	std::string getTimeSt(time_t* time);
	unsigned short width,  height, type;
	std::string sourceName, destName;
	static long count;
	RequestBuilder*  req_ptr;
	std::string pingReqName,pingNotifyName,pingSharedMemoryName,writeCompletedName, writeEnabledName, dataSharedMemoryName;
	HANDLE pingReq, pingNotify,pingSharedMemory,writeCompleted,writeEnabled,dataSharedMemory;
	uint8_t* data_ptr,*ping_ptr;
	unsigned long bytesToWrite;
	LARGE_INTEGER offset;
	volatile int in_progress,copied,inited;
};

#endif /* CLIENTREQUEST_H_ */
