/*
 * EngineCreatingTools.h
 *
 *  Created on: 2 дек. 2017 г.
 *      Author: Алёна
 */

#ifndef ENGINECREATINGTOOLS_H_
#define ENGINECREATINGTOOLS_H_
#include <iostream>
#include <stdint.h>
#include <string>
#include <wtypes.h>
#include <winbase.h>
#include "server_names.h"
class EngineCreatingTools {
public:
	static int createEvent(long manual, long initial, std::string& name, HANDLE* ev_ptr);
	static int createMutex(int ownerHere,std::string& name, HANDLE* mutex_ptr);
	static int createSharedMemory(std::string name, unsigned sizeLow, unsigned rights, unsigned mapRights, HANDLE* filemap_ptr, uint8_t** pptr);
	static int openEvent(long rights, std::string name, HANDLE* ev_ptr);
	static int openMutex(long rights, std::string name, HANDLE* mutex_ptr);
	static int openFileMapping(long rights, std::string name, HANDLE* fm_ptr);
	static int findServer(HANDLE* reqFlag, HANDLE* reqEnabledFlag, HANDLE* reqMutex, HANDLE* reqChannel);
	static void closeHandle(HANDLE aHandle);
};

#endif /* ENGINECREATINGTOOLS_H_ */
