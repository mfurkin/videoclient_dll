/*
 * AbortdeStatusWriter.cpp
 *
 *  Created on: 29 но€б. 2017 г.
 *
 */

#include "AbortedStatusWriter.h"

AbortedStatusWriter::AbortedStatusWriter() {
}

void AbortedStatusWriter::outputStatus(unsigned short progress) {
	std::cerr<<"Request aborted\n";
}

AbortedStatusWriter::~AbortedStatusWriter() {
}

