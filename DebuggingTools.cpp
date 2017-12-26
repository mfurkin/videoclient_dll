/*
 * DebuggingTools.cpp
 *
 *  Created on: 12 дек. 2017 г.
 *      Author: Алёна
 */

#include "DebuggingTools.h"

DebuggingTools::DebuggingTools() {
}

DebuggingTools::~DebuggingTools() {

}
/*
void DebuggingTools::logPtr(const char* msg, unsigned ptr) {
	char st[MAX_PATH];
	sprintf(st,"%s ptr:%0x\n",msg,ptr);
	std::cerr<<st;
}

int DebuggingTools::checkFileMapping(std::string name, std::string msg) {
	HANDLE fileMapping = OpenFileMapping(FILE_MAP_WRITE,FALSE,name.c_str());
	int result = (int)fileMapping;
	ClientCommon::getClientCommon().logString("DebuggingTools::checkFileMapping","checkFileMapping",msg);
	ClientCommon::getClientCommon().logPtr(msg,"DebuggingTools::checkFileMapping",result);
	std::cerr<<"checkFileMapping"<<msg<<" result="<<result<<"\n";
	return result;
}

int DebuggingTools::checkTimeouts(unsigned short& count) {
	std::cerr<<"checkTimeouts WAIT_TIMEOT has been got count="<<count--<<"\n";
	if (!(count))
		std::cerr<<"checkTimeouts: Count is finished\n";
	return count;
}
*/
