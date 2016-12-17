/*
File:   main.cpp
Author: J. Ian Lindsay
Date:   2014.05.19

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

____ _  _ ____ ____ _  _ ____ ____ _  _ ____ ____ _  _ ____
|    |__| |__/ |  | |\ | |  | |    |__| |__/ |  | |\/| |__|
|___ |  | |  \ |__| | \| |__| |___ |  | |  \ |__| |  | |  |


Supported build targets: Teensy3 and Raspi.
*/

#include <Platform/Platform.h>

#include <Transports/ManuvrSerial/ManuvrSerial.h>
#include <XenoSession/Console/ManuvrConsole.h>

#include "ChronoChroma/ChronoChroma.h"

#define HOST_BAUD_RATE  115200



// These are only here until they are migrated to each receiver that deals with them.
const MessageTypeDef message_defs_cc[] = {
  {  MANUVR_MSG_DIRTY_FRAME_BUF      , 0x0000,  "DIRTY_FRAME_BUF",   ManuvrMsg::MSG_ARGS_NONE }, // Something changed the framebuffer and we need to redraw.
};


Kernel*             kernel        = NULL;


/****************************************************************************************************
* Entry-point for teensy3...                                                                        *
****************************************************************************************************/

uint8_t analog_write_val = 0;
int8_t direction = 1;   // Zero for hold, 1 for brighten, -1 for darken.

void logo_fade() {
  if (direction < 0) analog_write_val--;
  else if (direction > 0) analog_write_val++;
  else return;

  if (0 == analog_write_val) {
    direction = direction * -1;
  }
  else if (200 == analog_write_val) {
    direction = direction * -1;
  }
  analogWrite(4, analog_write_val);
}



void setup() {
  // One of the first things we need to do is populate the EventManager with all of the
  // message codes that come with this firmware.
  int mes_count = sizeof(message_defs_cc) / sizeof(MessageTypeDef);
  ManuvrMsg::registerMessages(message_defs_cc, mes_count);

  platform.platformPreInit();
  kernel = platform.kernel();

  analogWriteResolution(12);   // Setup the DAC.

  gpioDefine(13, OUTPUT);
  gpioDefine(11, OUTPUT);
  gpioDefine(4,  OUTPUT);
  gpioDefine(2,  INPUT_PULLUP);
  gpioDefine(3,  INPUT_PULLUP);
  gpioDefine(14, INPUT);

  kernel->subscribe((EventReceiver*) new ChronoChroma());

  platform.bootstrap();

  ManuvrMsg* fade_sched = kernel->createSchedule(40, -1, false, logo_fade);
  fade_sched->enableSchedule(true);
}


void loop() {
  ManuvrSerial  _console_xport("U", HOST_BAUD_RATE);  // Indicate USB.
  kernel->subscribe((EventReceiver*) &_console_xport);
  ManuvrConsole _console((BufferPipe*) &_console_xport);
  kernel->subscribe((EventReceiver*) &_console);

  while (1) {
    kernel->procIdleFlags();
  }
}
