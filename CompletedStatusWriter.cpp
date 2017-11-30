/*
 * CompletedStatusWriter.cpp
 *
 *  Created on: 29 но€б. 2017 г.
 *
 */

#include "CompletedStatusWriter.h"

CompletedStatusWriter::CompletedStatusWriter() {
}

void CompletedStatusWriter::outputStatus(unsigned short progress) {
	std::cerr<<"Request completed\n";
}

CompletedStatusWriter::~CompletedStatusWriter() {
}

