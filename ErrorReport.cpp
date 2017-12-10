/*
 * RequestBuilder.cpp
 *
 *  Created on: 25 но€б. 2017 г.
 *
 */

#include "ErrorReport.h"
#include "ClientCommon.h"
ErrorReport::ErrorReport():ClientRequest(),errorTime(0),dateSt(std::string("empty date")) {
}

ErrorReport::ErrorReport(const ErrorReport& aReport):ClientRequest(aReport),errorTime(aReport.errorTime),dateSt(aReport.dateSt) {
}

ErrorReport::ErrorReport(const ClientRequest& aRequest, time_t aTime, std::string& aDateSt):ClientRequest(aRequest),errorTime(aTime),dateSt(aDateSt) {
}


/*
ErrorReport::ErrorReport():width(0),height(0),type(0),time(0),sourceName("no source file"), destName("no dest file"),
								 dateSt("00.00.0000 00:00:00") {
}

ErrorReport::ErrorReport(const ErrorReport& aBuilder): width(aBuilder.width),height(aBuilder.height),type(aBuilder.type),
																time(aBuilder.time),sourceName(aBuilder.sourceName),destName(aBuilder.destName),
																dateSt(aBuilder.dateSt) {
}

ErrorReport::ErrorReport(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight,
							   unsigned short aType, std::string aDateSt, time_t aTime):width(aWidth),height(aHeight),type(aType),
							   time(aTime),sourceName(aSourceName), destName(aDestName),dateSt(aDateSt) {
}
*/
ErrorReport::~ErrorReport() {
}

void ErrorReport::pastErrorReport(ClientCommon& clientCommon) {
	clientCommon.addThisError(getKey(),this);
//	clientCommon.addError(request.getKey(),request);
}

std::string ErrorReport::toString() {
	char line[MAX_PATH];
//	std::cerr<<"ErrorReport::toString enter\n";
	std::string cl_req=ClientRequest::toString();
//	std::cerr<<"ErrorReport::toString pt1\n";
	sprintf(line,"%s %d %s",dateSt.c_str(),(unsigned)time,cl_req.c_str());
//	std::cerr<<"ErrorReport::toString exit\n";
	return std::string(line);
}
/*
void ErrorReport::errorReport() {
	errorReport(ClientCommon::getClientCommon());
}

ClientRequest* ErrorReport::build() {
	ClientRequest* req_ptr =  new ClientRequest(sourceName,destName,width,height,type,this);
	return req_ptr;
}

std::string ErrorReport::getKey() {
	return sourceName.append("->").append(destName);
}

int ErrorReport::isRun(ClientCommon clientCommon) {
	return clientCommon.isInProgress(destName);
}

// void ErrorReport::sendRequest(ClientCommon& clientCommon) {
void ErrorReport::sendRequest() {
	request.sendRequest();
}
*/
