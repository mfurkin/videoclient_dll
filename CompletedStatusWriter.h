/*
 * CompletedStatusWriter.h
 *
 *  Created on: 29 ����. 2017 �.
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
