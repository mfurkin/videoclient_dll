/*
 * AbortdeStatusWriter.h
 *
 *  Created on: 29 ����. 2017 �.
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
