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

#include <Drivers/TLC5947/TLC5947.h>

#define RGBCLOCK_COLOR_STEP 0x0010

/*
This driver assumes a TLC5947 PWM driver chip with the following
  segment assignments:

segment(x)
 --31-       --23-       --15-       --7--
|     |     |     |     |     |     |     |
25   30     17   22     9    14     1     6
|     |     |     |     |     |     |     |
 --24-       --16-       --8--       --0--
|     |     |     |     |     |     |     |
26   21     18   13     10   13     2     5
|     |     |     |     |     |     |     |
 --27-  28   --19-  20   --11-  12   --3--  4

Within each segment, the channel order is B, R, G. Wiring constraints...

Each segment has three PWM channels of 12-bits each. This is a bit
  awkward with offsets. So it is the job of this class to...

  1) Interpret application input as strings and numbers, converting them
       to glyph bytes to show the value on 32 segments.
  2) Treat the TLC5947's buffer as 48-bit colored digit segments, and abstract
       this into a notion of color, to be presented to the application.
*/

enum class GlyphMap8 {
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
  DASH     = 0b10000000,
  USCORE   = 0b00010000,
  POINT    = 0b00001000
};


class RGBClock : public TLC5947 {
  public:
    RGBClock(SPIAdapter*, uint8_t cs, uint8_t oe);
    ~RGBClock();

    // TODO: Maybe HSB makes more sense in this context...
    //void setBrightness(uint brightness);

    void showString(char*);
    void clearDisplay();
    void setGlobalColor(uint16_t r, uint16_t g, uint16_t b);
    void printDebug(StringBuilder*);

    inline void incRed() {   setGlobalColor(_global_r + RGBCLOCK_COLOR_STEP, _global_g, _global_b);  };
    inline void decRed() {   setGlobalColor(_global_r - RGBCLOCK_COLOR_STEP, _global_g, _global_b);  };
    inline void incGreen() { setGlobalColor(_global_r, _global_g + RGBCLOCK_COLOR_STEP, _global_b);  };
    inline void decGreen() { setGlobalColor(_global_r, _global_g - RGBCLOCK_COLOR_STEP, _global_b);  };
    inline void incBlue() {  setGlobalColor(_global_r, _global_g, _global_b + RGBCLOCK_COLOR_STEP);  };
    inline void decBlue() {  setGlobalColor(_global_r, _global_g, _global_b - RGBCLOCK_COLOR_STEP);  };


  private:
    uint16_t _global_r    = 0x0800;
    uint16_t _global_g    = 0x0800;
    uint16_t _global_b    = 0x0800;

    void    _write_digit(uint8_t idx, uint8_t glyph);
    void    _relevel_buffer();
    bool    _seg_on(uint8_t seg);
    uint8_t _char_to_glyph(char);
};


#endif
