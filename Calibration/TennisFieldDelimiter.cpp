/*
 * TennisFieldDelimiter.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: sled
 */

#include "TennisFieldDelimiter.h"

TennisFieldDelimiter::TennisFieldDelimiter() {
	// TODO Auto-generated constructor stub

}

TennisFieldDelimiter::~TennisFieldDelimiter() {
	// TODO Auto-generated destructor stub
}

void TennisFieldDelimiter::printDebug()
{
	cout << "(DEBUG) Tennis Field Delimiter:" << endl;
	cout << "\tBL: " << bottomLeft << endl;
	cout << "\tBR: " << bottomRight << endl;
	cout << "\tTR: " << topRight << endl;
	cout << "\tTL: " << topLeft << endl;
	cout << "(DEBUG) ---" << endl;
}
