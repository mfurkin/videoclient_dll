/*
 * RequestBuilder.cpp
 *
 *  Created on: 25 но€б. 2017 г.
 *
 */

#include "RequestBuilder.h"

RequestBuilder::RequestBuilder():width(0),height(0),type(0),time(0),sourceName("no source file"), destName("no dest file"),
								 dateSt("00.00.0000 00:00:00") {
}

RequestBuilder::RequestBuilder(const RequestBuilder& aBuilder): width(aBuilder.width),height(aBuilder.height),type(aBuilder.type),
																time(aBuilder.time),sourceName(aBuilder.sourceName),destName(aBuilder.destName),
																dateSt(aBuilder.dateSt) {
}

RequestBuilder::RequestBuilder(std::string aSourceName, std::string aDestName, unsigned short aWidth, unsigned short aHeight,
							   unsigned short aType, std::string aDateSt, time_t aTime):width(aWidth),height(aHeight),type(aType),
							   time(aTime),sourceName(aSourceName), destName(aDestName),dateSt(aDateSt) {
}

RequestBuilder::~RequestBuilder() {
}

void RequestBuilder::errorReport(ClientCommon& clientCommon) {
	clientCommon.addError(getKey(),this);
}

void RequestBuilder::errorReport() {
	errorReport(ClientCommon::getClientCommon());
}

ClientRequest* RequestBuilder::build() {
	ClientRequest* req_ptr =  new ClientRequest(sourceName,destName,width,height,type,this);
	return req_ptr;
}

std::string RequestBuilder::getKey() {
	return sourceName.append("->").append(destName);
}

int RequestBuilder::isRun(ClientCommon clientCommon) {
//	return clientCommon.isInProgress(getKey());
	return clientCommon.isInProgress(destName);
}

ClientRequest* RequestBuilder::startNewRequest(ClientCommon& clientCommon) {
	ClientRequest* req_ptr = build();
//	clientCommon.startNewRequest(getKey(),req_ptr);
	clientCommon.startNewRequest(sourceName,req_ptr);
	return req_ptr;
}
