/*
 * RequestBuilder.h
 *
 *  Created on: 25 но€б. 2017 г.
 *
 */

#ifndef ERRORREPORT_H_
#define ERRORREPORT_H_
#include "ClientCommon.h"
#include "ClientRequest.h"


class ErrorReport : public ClientRequest {

public:
	ErrorReport();
	ErrorReport(const ErrorReport& aReport);
	ErrorReport(const ClientRequest&  aRequest, time_t aTime, std::string& aDateSt);
	virtual ~ErrorReport();
	std::string toString();
	void addThisError(ClientCommon& clientCommon);
private:
	static std::string ERROR_REPORT_TOSTRING_TAG;
	time_t errorTime;
	std::string dateSt;
};

#endif /* ERRORREPORT_H_ */
