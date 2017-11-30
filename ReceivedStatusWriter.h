/*
 * ReceivedStatusWriter.h
 *
 *  Created on: 29 ����. 2017 �.
 *
 */

#ifndef RECEIVEDSTATUSWRITER_H_
#define RECEIVEDSTATUSWRITER_H_

#include "StatusWriter.h"

class ReceivedStatusWriter: public StatusWriter {
public:
	ReceivedStatusWriter();
	void outputStatus(unsigned short progress);
	virtual ~ReceivedStatusWriter();
};

#endif /* RECEIVEDSTATUSWRITER_H_ */
