/*
 * InProgressStatusWriter.cpp
 *
 *  Created on: 29 ����. 2017 �.
 *
 */

#include "InProgressStatusWriter.h"

InProgressStatusWriter::InProgressStatusWriter() {
}

void InProgressStatusWriter::outputStatus(unsigned short progress) {
	std::cerr<<"Request in progress "<<progress<<" percents have done\n";
}

InProgressStatusWriter::~InProgressStatusWriter() {
}

