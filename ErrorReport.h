/*
 * RequestBuilder.h
 *
 *  Created on: 25 но€б. 2017 г.
 *
 */

#ifndef ERRORREPORT_H_
#define ERRORREPORT_H_
// #include "ClientCommon.h"
#include "ClientRequest.h"
#include "server_names.h"

// class ClientCommon;

// class ClientRequest;

class ErrorReport : public ClientRequest {
public:
	ErrorReport();
	ErrorReport(const ErrorReport& aBuilder);
	ErrorReport(const ClientRequest&  aRequest, time_t aTime, std::string& aDateSt);
	/*
	ErrorReport(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight, unsigned short aType,
				   std::string aDateSt, time_t aTime);
*/
	virtual ~ErrorReport();
//	int isRun(ClientCommon clientCommon);
//	void sendRequest(ClientCommon& clientCommon);
//	void sendRequest();

	void pastErrorReport(ClientCommon& clientCommon);
/*
	void errorReport();
*/
private:
	/*
	std::string getKey();
	ClientRequest* build();
	*/
//	ClientRequest request;
//	unsigned short width,  height, type;
	time_t errorTime;
//	std::string sourceName, destName, dateSt;
	std::string dateSt;
};

#endif /* ERRORREPORT_H_ */
