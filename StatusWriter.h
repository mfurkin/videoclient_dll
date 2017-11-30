/*
 * StatusWriter.h
 *
 *  Created on: 29 но€б. 2017 г.
 *
 */

#ifndef STATUSWRITER_H_
#define STATUSWRITER_H_
#include <iostream>
class StatusWriter {
public:
	StatusWriter();
	virtual void outputStatus(unsigned short progress) = 0;
	virtual ~StatusWriter();
};

#endif /* STATUSWRITER_H_ */
