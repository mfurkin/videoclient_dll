/*
 * CompletedStatusWriter.cpp
 *
 *  Created on: 29 ����. 2017 �.
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

