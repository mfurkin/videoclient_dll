/*
 * AbortdeStatusWriter.cpp
 *
 *  Created on: 29 ����. 2017 �.
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

