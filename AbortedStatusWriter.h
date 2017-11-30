/*
 * AbortdeStatusWriter.h
 *
 *  Created on: 29 но€б. 2017 г.
 *
 */

#ifndef ABORTEDSTATUSWRITER_H_
#define ABORTEDSTATUSWRITER_H_

#include "StatusWriter.h"

class AbortedStatusWriter: public StatusWriter {
public:
	AbortedStatusWriter();
	void outputStatus(unsigned short progress);
	virtual ~AbortedStatusWriter();
};

#endif /* ABORTEDSTATUSWRITER_H_ */
