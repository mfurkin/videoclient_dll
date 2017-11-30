/*
 * RequestBuilder.h
 *
 *  Created on: 25 но€б. 2017 г.
 *
 */

#ifndef REQUESTBUILDER_H_
#define REQUESTBUILDER_H_
#include "ClientCommon.h"
#include "ClientRequest.h"
#include "server_names.h"
class ClientCommon;

class ClientRequest;

class RequestBuilder {
public:
	RequestBuilder();
	RequestBuilder(const RequestBuilder& aBuilder);
	RequestBuilder(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight, unsigned short aType,
				   std::string aDateSt, time_t aTime);

	virtual ~RequestBuilder();
	int isRun(ClientCommon clientCommon);
	ClientRequest* startNewRequest(ClientCommon& clientCommon);
	void errorReport(ClientCommon& clientCommon);
	void errorReport();
private:
	std::string getKey();
	ClientRequest* build();
	unsigned short width,  height, type;
	time_t time;
	std::string sourceName, destName, dateSt;
};

#endif /* REQUESTBUILDER_H_ */
