/*
File:   RGBClock.h
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

#ifndef IANS_GIANT_RGB_7SEG_H
#define IANS_GIANT_RGB_7SEG_H

#include <SPI.h>
#include <Drivers/TLC5947/TLC5947.h>


/*
This driver assumes a TLC5947 PWM driver chip with the following
  segment assignments:

segment(x)
  --7--
 |     |
 1     6
 |     |
  --0--
 |     |
 2     5
 |     |
  --3--  4

Each segment has three PWM channels of 12-bits each. This is a bit
  awkward with offsets. So it is the job of this class to interpret
  the TLC5947's buffer as colored digit segments for the application.

*/

enum class CharacterDefs {
  DIGIT_0  = 0b01110111,
  DIGIT_1  = 0b00000110,
  DIGIT_2  = 0b10110011,
  DIGIT_3  = 0b10010111,
  DIGIT_4  = 0b11000110,
  DIGIT_5  = 0b11010101,
  DIGIT_6  = 0b11110101,
  DIGIT_7  = 0b00000111,
  DIGIT_8  = 0b11110111,
  DIGIT_9  = 0b11010111,
  DIGIT_A  = 0b11100111,
  DIGIT_B  = 0b11110100,
  DIGIT_C  = 0b01110001,
  DIGIT_D  = 0b11110100,
  DIGIT_E  = 0b11110001,
  DIGIT_F  = 0b11100001,
  POINT    = 0b00001000
};


class RGBClock : public TLC5947 {
  public:
    RGBClock(SPIAdapter*, uint8_t cs, uint8_t oe);
    ~RGBClock();

    void     showDigit(int);
    uint16_t getSegColor(uint8_t seg, char channel);
    void     setSegColor(uint8_t seg, uint64_t nu);
    uint64_t getSegColor(uint8_t seg);
    void     setSegColor(uint8_t seg, char channel, uint16_t nu);
    void     clearDisplay(void);
    bool     segOn(uint8_t seg);

    void printDebug(StringBuilder*);


  private:
    uint8_t _seg_mask[4] = {0xFF, 0xFF, 0xFF, 0xFF};  // All segs of all digits off by default.

    void    _write_digit(uint8_t idx, uint8_t val, bool punctuation);

    static constexpr uint8_t CHANS_PER_SEG    = 3;
};


#endif
