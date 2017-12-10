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

// #include "ClientCommon.h"
#include "server_names.h"
#include "AbortedStatusWriter.h"
#include "CompletedStatusWriter.h"
#include "PingThreadEngine.h"
// #include "ErrorReport.h"
#include "InProgressStatusWriter.h"
#include "ReceivedStatusWriter.h"

class ClientCommon;


class ClientRequest {
public:
	ClientRequest();
	ClientRequest(const ClientRequest& aRequest);
	ClientRequest(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight, unsigned short aType);
	virtual ~ClientRequest();
	int registerRequest(ClientCommon& commonClient);
	void unregisterRequest(ClientCommon& commonClient);
//	ClientRequest(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight, unsigned short aType, ErrorReport* req_ptr);
//	static VOID CALLBACK writeCompletedProc( DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
	static unsigned WINAPI processRequest(void* ptr);
	static std::string getTimeSt(time_t* time);
	virtual std::string toString();
	void currentErrorReport();
	void sendRequest();
protected:
	std::string getConvType();
	std::string getKey();
private:
	/*
	void initStatusWriters(std::map<int,StatusWriter*>& writers);
	void deinitStatusWriters(std::map<int,StatusWriter*>& writers);
	void checkThisWrite(unsigned long  errorCode, unsigned long bytesWritten);
	void receiveHeaderData(unsigned& frames, unsigned long& size);
	*/
//	static void deinitStatusWriter(std::pair<int,StatusWriter*> aPair);

//	HANDLE writeRequest();
	unsigned processThisRequest();
	/*
	int createEvent(long manual, long initial, std::string& name, HANDLE* ev_ptr);
	int openEvent(long rights, std::string name, HANDLE* ev_ptr);
	int openMutex(long rights, std::string name, HANDLE* ev_ptr);
	int openFileMapping(long rights, std::string name, HANDLE* fm_ptr);
	int createSharedMemory(std::string name, unsigned sizeLow, unsigned rights, unsigned MapRights, HANDLE* filemap_ptr, uint8_t** pptr);
	*/
//	void closeHandle(HANDLE aHandle);
	std::string generateName(std::string& first, std::string second);
//	void setCopied();
//	void init();
//	ErrorReport*  createErrorReport();
//	static std::string getTimeSt(time_t* time);
	unsigned short width,  height, type;
	std::string sourceName, destName;
	PingThreadEngine* engine_ptr;
	volatile int in_progress;
	/*
	static long count;

	ErrorReport*  req_ptr;
	std::string pingReqName,pingNotifyName,pingSharedMemoryName,writeCompletedName, writeEnabledName, dataSharedMemoryName;
	HANDLE pingReq, pingNotify,pingSharedMemory,writeCompleted,writeEnabled,dataSharedMemory;
	uint8_t* data_ptr,*ping_ptr;
	unsigned long bytesToWrite;
	LARGE_INTEGER offset;
	volatile int in_progress,copied,inited;
	*/
};

#endif /* CLIENTREQUEST_H_ */
