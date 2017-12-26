/*
 * DebuggingTools.h
 *
 *  Created on: 12 дек. 2017 г.
 *      Author: Алёна
 */

#ifndef DEBUGGINGTOOLS_H_
#define DEBUGGINGTOOLS_H_
#include <stdio.h>
#include <iostream>
#include <wtypes.h>
#include <winbase.h>
#include <windef.h>
#include "ClientCommon.h"

class DebuggingTools {
public:
	DebuggingTools();
	virtual ~DebuggingTools();
//	static void logPtr(const char* msg, unsigned ptr);
//	static int checkFileMapping(std::string name, std::string msg);
//	static int checkTimeouts(unsigned short& count);
};

#endif /* DEBUGGINGTOOLS_H_ */
