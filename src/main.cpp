/*
File:   main.cpp
Author: J. Ian Lindsay
Date:   2016.12.10

Copyright 2016 Manuvr, Inc

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

____ _  _ ____ ____ _  _ ____ ____ _  _ ____ ____ _  _ ____
|    |__| |__/ |  | |\ | |  | |    |__| |__/ |  | |\/| |__|
|___ |  | |  \ |__| | \| |__| |___ |  | |  \ |__| |  | |  |


Supported build targets: Teensy3 and (TODO) Raspi.
*/

#include <Platform/Platform.h>

#include <Transports/ManuvrSerial/ManuvrSerial.h>
#include <XenoSession/Console/ManuvrConsole.h>

#include "ChronoChroma/ChronoChroma.h"

#define HOST_BAUD_RATE  115200


Kernel* kernel = nullptr;

// These are only here until they are migrated to each receiver that deals with them.
const MessageTypeDef message_defs_cc[] = {
  {  MANUVR_MSG_DIRTY_FRAME_BUF      , 0x0000,  "DIRTY_FRAME_BUF",   ManuvrMsg::MSG_ARGS_NONE }, // Something changed the framebuffer and we need to redraw.
};


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
