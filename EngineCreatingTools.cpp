/*
 * EngineCreatingTools.cpp
 *
 *  Created on: 2 дек. 2017 г.
 *
 */

#include "EngineCreatingTools.h"


int EngineCreatingTools::createEvent(long manual, long initial, std::string& name, HANDLE* ev_ptr) {
	*ev_ptr = CreateEvent(NULL,manual,initial,name.c_str());
	return(*ev_ptr != NULL);
}

int EngineCreatingTools::createSharedMemory(std::string name, unsigned sizeLow, unsigned rights, unsigned mapRights, HANDLE* filemap_ptr,
											uint8_t** pptr) {
	int result;
	*filemap_ptr = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,rights,0,sizeLow,name.c_str());
	if (!(*filemap_ptr))
		result = FALSE;
	else{
//		std::cerr<<"createSharedMemory name= "<<name<<" length="<<name.length()<<" createFileMapping not null error code="<<GetLastError()<<"\n";
		*pptr = (uint8_t*)  MapViewOfFile(*filemap_ptr,mapRights,0,0,sizeLow);
		result = (int)*pptr;
	}
	return result;
}

int EngineCreatingTools::openEvent(long rights, std::string name, HANDLE* ev_ptr) {
	*ev_ptr = OpenEvent(rights,FALSE,name.c_str());
	return (int)*ev_ptr;
}

int EngineCreatingTools::openMutex(long rights, std::string name, HANDLE* mutex_ptr) {
	*mutex_ptr = OpenMutex(rights,FALSE,name.c_str());
	return (int)*mutex_ptr;
}

int EngineCreatingTools::openFileMapping(long rights, std::string name, HANDLE* fm_ptr) {
	*fm_ptr = OpenFileMapping(rights,FALSE,name.c_str());
	return (int)*fm_ptr;
}

int EngineCreatingTools::findServer(HANDLE* reqFlag, HANDLE* reqEnabledFlag, HANDLE* reqMutex, HANDLE* reqChannel) {
	return openEvent(EVENT_MODIFY_STATE,REQ_FLAG_NAME,reqFlag) && openEvent(SYNCHRONIZE,REQ_ENABLED_FLAG,reqEnabledFlag) &&
				 openMutex(SYNCHRONIZE,REQ_MUTEX_NAME,reqMutex) && openFileMapping(FILE_MAP_WRITE,REQ_FILE_NAME,reqChannel);
}


int EngineCreatingTools::createMutex(int ownerHere, std::string& name, HANDLE* mutex_ptr) {
	*mutex_ptr = CreateMutex(NULL,FALSE,name.c_str());
	return (mutex_ptr != NULL);
}

void EngineCreatingTools::closeHandle(HANDLE aHandle) {
	if (aHandle !=NULL)
		CloseHandle(aHandle);
}
