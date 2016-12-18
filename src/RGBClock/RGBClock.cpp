/*
File:   RGBClock.cpp
Author: J. Ian Lindsay
Date:   2016.12.15

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include "RGBClock.h"


RGBClock::RGBClock(SPIAdapter* b, uint8_t cs, uint8_t oe) : TLC5947(4, b, cs, oe) {
}


RGBClock::~RGBClock() {
}


void RGBClock::clearDisplay() {
	zeroAll();
}


void RGBClock::showDigit(int dig) {
	uint8_t* buf = buffer();
	size_t len = bufLen();
	random_fill(buf, len);
}


void RGBClock::printDebug(StringBuilder* output) {
	output->concat("---< RGBClock >------------------------------------- \n");
	output->concatf("--\t %d digits chained for a total of %u bytes of buffer\n.", _chain_len, bufLen());
	output->concatf("--\t Using CS pin %u, OE pin %u\n", _cs_pin, _oe_pin);
	output->concatf("--\t Buffer is %s dirty.\n--\n", (_buf_dirty) ? "":"not");
}
