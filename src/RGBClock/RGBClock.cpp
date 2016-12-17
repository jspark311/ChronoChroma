#include "RGBClock.h"


RGBClock::RGBClock(BusAdapter<SPIBusOp>* b, uint8_t cs, uint8_t oe) : TLC5947(4, b, cs, oe) {
}


RGBClock::~RGBClock() {
}


void RGBClock::clearDisplay() {
	zeroAll();
}
