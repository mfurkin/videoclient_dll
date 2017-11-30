/*
 * InProgressStatusWriter.h
 *
 *  Created on: 29 ����. 2017 �.
 *
 */

#ifndef INPROGRESSSTATUSWRITER_H_
#define INPROGRESSSTATUSWRITER_H_

#include "StatusWriter.h"

class InProgressStatusWriter: public StatusWriter {
public:
	InProgressStatusWriter();
	void outputStatus(unsigned short progress);
	virtual ~InProgressStatusWriter();
};

#endif /* INPROGRESSSTATUSWRITER_H_ */
