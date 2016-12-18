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


/*
* Zeros the buffer, but does not blank the display.
*/
void RGBClock::clearDisplay() {
	zeroAll();
	refresh();
}


void RGBClock::showString(char* str) {
	if (str) {
		if (*str) { _write_digit(0, _char_to_glyph(*(str++)));  }
		if (*str) { _write_digit(1, _char_to_glyph(*(str++)));  }
		if (*str) { _write_digit(2, _char_to_glyph(*(str++)));  }
		if (*str) { _write_digit(3, _char_to_glyph(*(str++)));  }
		refresh();
	}
}


void RGBClock::printDebug(StringBuilder* output) {
	output->concat("---< RGBClock >------------------------------------- \n");
	output->concatf("--\t %d digits chained (%u bytes of buffer)\n", _chain_len, bufLen());
	output->concatf("--\t CS pin: %u, OE pin: %u\n", _cs_pin, _oe_pin);
	output->concatf("--\t R: 0x%04x, G: 0x%04x, B: 0x%04x\n", _global_r, _global_g, _global_b);
	output->concatf("--\t Buffer is %s.\n--\n", (_buf_dirty) ? "dirty":"clean");
	StringBuilder::printBuffer(output, buffer(), bufLen(), "--\t ");
}


/*
* Writes a glyph to the buffer representing one of the four digits after
*   considering color and brightness.
*/
void RGBClock::_write_digit(uint8_t idx, uint8_t glyph) {
	for (int i = 0; i < 8; i++) {
		// We shift the glyph byte one place at a time, and if that segment is to
		//   be lit, we apply the color/brightness setting for the display and
		//   write it to the appropriate offsets in the buffer.
		uint base = (idx*24) + (i*3);
		setChannel(base + 0, (glyph & 0x80) ? _global_r : 0);
		setChannel(base + 1, (glyph & 0x80) ? _global_g : 0);
		setChannel(base + 2, (glyph & 0x80) ? _global_b : 0);
		glyph = glyph << 1;
	}
}


/*
* Applies the present color settings to the buffer without altering the glyphs.
*/
void RGBClock::_relevel_buffer() {
	for (uint seg = 0; seg < 32; seg++) {
		if (_seg_on(seg)) {
			setChannel((seg*3) + 0, _global_b);
			setChannel((seg*3) + 1, _global_r);
			setChannel((seg*3) + 2, _global_g);
		}
		else {
			setChannel((seg*3) + 0, 0);
			setChannel((seg*3) + 1, 0);
			setChannel((seg*3) + 2, 0);
		}
	}
	refresh();
}


void RGBClock::setGlobalColor(uint16_t r, uint16_t g, uint16_t b) {
	_global_r = strict_min(r, 0x0FFF);
	_global_g = strict_min(g, 0x0FFF);
	_global_b = strict_min(b, 0x0FFF);
	_relevel_buffer();
}


/*
* Given a supported character, return a glyph to represent it as best as
*   possible on a 7-seg display.
*/
uint8_t RGBClock::_char_to_glyph(char c) {
	if (c >= 97) {
		c -= 32;	// We don't have the luxury of caring about case-distinguishment.
	}
	switch (c) {
		case '0':  return (uint8_t) GlyphMap8::DIGIT_0;
		case '1':  return (uint8_t) GlyphMap8::DIGIT_1;
		case '2':  return (uint8_t) GlyphMap8::DIGIT_2;
		case '3':  return (uint8_t) GlyphMap8::DIGIT_3;
		case '4':  return (uint8_t) GlyphMap8::DIGIT_4;
		case '5':  return (uint8_t) GlyphMap8::DIGIT_5;
		case '6':  return (uint8_t) GlyphMap8::DIGIT_6;
		case '7':  return (uint8_t) GlyphMap8::DIGIT_7;
		case '8':  return (uint8_t) GlyphMap8::DIGIT_8;
		case '9':  return (uint8_t) GlyphMap8::DIGIT_9;
		case 'A':  return (uint8_t) GlyphMap8::DIGIT_A;
		case 'B':  return (uint8_t) GlyphMap8::DIGIT_B;
		case 'C':  return (uint8_t) GlyphMap8::DIGIT_C;
		case 'D':  return (uint8_t) GlyphMap8::DIGIT_D;
		case 'E':  return (uint8_t) GlyphMap8::DIGIT_E;
		case 'F':  return (uint8_t) GlyphMap8::DIGIT_F;
		case '-':  return (uint8_t) GlyphMap8::DASH;
		case '_':  return (uint8_t) GlyphMap8::USCORE;
		case '.':  return (uint8_t) GlyphMap8::POINT;
		default:   break;
	}
	return 0;
}


/*
* Probes the buffer to see if a segment is on or off (no matter the color).
* NOTE: Doesn't consider if the buffer is dirty. Will return the state of the
*   buffer and not necessarily the display.
*/
bool RGBClock::_seg_on(uint8_t seg) {
	if (0 == getChannel((seg*3) + 0)) {
		if (0 == getChannel((seg*3) + 1)) {
			if (0 == getChannel((seg*3) + 2)) {
				return false;
			}
		}
	}
	return true;
}
