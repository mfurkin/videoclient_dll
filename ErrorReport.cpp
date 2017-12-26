/*
 * RequestBuilder.cpp
 *
 *  Created on: 25 но€б. 2017 г.
 *
 */

#include "ErrorReport.h"
std::string ErrorReport::ERROR_REPORT_TOSTRING_TAG="ErrorReport::toString";
ErrorReport::ErrorReport():ClientRequest(),errorTime(0),dateSt(std::string("empty date")) {
}

ErrorReport::ErrorReport(const ErrorReport& aReport):ClientRequest(aReport),errorTime(aReport.errorTime),dateSt(aReport.dateSt) {
}

ErrorReport::ErrorReport(const ClientRequest& aRequest, time_t aTime, std::string& aDateSt):ClientRequest(aRequest),errorTime(aTime),dateSt(aDateSt) {
}

ErrorReport::~ErrorReport() {
}

void ErrorReport::addThisError(ClientCommon& clientCommon) {
	clientCommon.addThisError(getKey(),this);
}

std::string ErrorReport::toString() {
	char line[MAX_PATH];
//	std::cerr<<"ErrorReport::toString enter\n";
	log(ERROR_REPORT_TOSTRING_TAG,"ErrorReport::enter");
	std::string cl_req=ClientRequest::toString();
	log(ERROR_REPORT_TOSTRING_TAG,"ErrorReport::toString pt1");
//	std::cerr<<"ErrorReport::toString pt1\n";
	sprintf(line,"%s %d %s",dateSt.c_str(),(unsigned)time,cl_req.c_str());
//	std::cerr<<"ErrorReport::toString exit\n";
	log(ERROR_REPORT_TOSTRING_TAG,"ErrorReport::toString exit");
	return std::string(line);
}

