/*
 * LoggerEngine.h
 *
 *  Created on: 26 дек. 2017 г.
 *      Author: Алёна
 */

#ifndef LOGGERENGINE_H_
#define LOGGERENGINE_H_

#include <string>

class LoggerEngine {
public:
	LoggerEngine();
	virtual ~LoggerEngine();
	virtual void log(std::string& tag,  std::string msg) = 0;
	virtual void logPtr(std::string& tag, std::string msg, unsigned ptr) = 0;
	virtual void logString(std::string& tag, std::string msg, std::string& msg2) = 0;
};


#endif /* LOGGERENGINE_H_ */
