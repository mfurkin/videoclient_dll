/*
 * CompletedStatusWriter.h
 *
 *  Created on: 29 но€б. 2017 г.
 *
 */

#ifndef COMPLETEDSTATUSWRITER_H_
#define COMPLETEDSTATUSWRITER_H_

#include "StatusWriter.h"

class CompletedStatusWriter: public StatusWriter {
public:
	CompletedStatusWriter();
	void outputStatus(unsigned short progress);
	virtual ~CompletedStatusWriter();
};

#endif /* COMPLETEDSTATUSWRITER_H_ */
