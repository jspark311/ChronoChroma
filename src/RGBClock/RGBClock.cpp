#include "RGB7Seg.h"


RGB7Seg::RGB7Seg(BusAdapter<SPIBusOp>* b, uint8_t cs, uint8_t oe) : TLC5947(4, b, cs, oe) {
}


RGB7Seg::~RGB7Seg() {
}


void RGB7Seg::clearDisplay() {
	zeroAll();
}


void RGB7Seg::addSegment(uint8_t seg) {
  for (int i = 0; i < 36; i++) {   working_set[i] |= segs[seg][i];   }
}

void RGB7Seg::remSegment(uint8_t seg) {
  for (int i = 0; i < 36; i++) {   working_set[i] &= ~segs[seg][i];   }
}


/*
* Returns true if the segment is on (any color channel in non-zero).
*/
bool RGB7Seg::segOn(uint8_t seg) {
  uint8_t result = 0;
  for (int i = 0; i < 36; i++) {   result |= (working_set[i] & segs[seg][i]);   }
  return (result != 0x00);
}


void RGB7Seg::showDigit(int digit) {
  if (digit >= 10) {
  	  // Don't try to read from an array that is too small.
  	  return;
  }
  for (int x = 0; x < 8; x++) {
    if ((RGB7Seg::seg_digit_map[digit] >> (7-x)) & 0x01) {
      // Digit should be on.
      if (!segOn(x)) { // And it isn't.
        addSegment(x);
        needs_refresh = true;
      }
    }
    else {
      if (segOn(x)) {
        remSegment(x);
        needs_refresh = true;
      }
    }
  }
}



uint16_t RGB7Seg::getSegColor(uint8_t seg, char channel) {
  uint8_t c_int = 0;
  uint16_t return_value = 0;
  switch (channel) {
    case 'r':
      c_int = 12;
      break;
    case 'g':
      c_int = 24;
      break;
    case 'b':
      c_int = 0;
      break;
    default:
      return 0;
  }
  uint16_t bits_to_shift = c_int + (3*12*seg);
  uint8_t bit_offset  = bits_to_shift % 8;
  uint8_t byte_offset = (bits_to_shift - bit_offset) / 8;

  if (bit_offset == 0) {
    return_value  = ((uint16_t) ((working_set[byte_offset] * 256) + (working_set[byte_offset+1] & 0xF0))) >> 4;
  }
  else {
    return_value  = ((uint16_t) ((working_set[byte_offset] & 0x0F) * 256) + working_set[byte_offset+1]);
  }
  return return_value;
}


uint64_t RGB7Seg::getSegColor(uint8_t seg) {
  uint64_t return_value = 0;
  return return_value;
}
