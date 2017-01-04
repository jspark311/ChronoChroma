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
#include <Drivers/Sensors/TMP006/TMP006.h>
#include <Drivers/Sensors/INA219/INA219.h>
#include <Drivers/Sensors/TSL2561/TSL2561.h>
#include <Drivers/Sensors/Si7021/Si7021.h>
#include <Drivers/Sensors/VEML6070/VEML6070.h>

#include "ChronoChroma/ChronoChroma.h"

#define HOST_BAUD_RATE  115200


Kernel* kernel = nullptr;

// These are only here until they are migrated to each receiver that deals with them.
const MessageTypeDef message_defs_cc[] = {
  {  CHRONOCHROMA_MSG_COLOR_SHF      , 0x0000,  "COLOR_SHIFT",       ManuvrMsg::MSG_ARGS_NONE }, //
  {  CHRONOCHROMA_MSG_DISP_UPDATE    , 0x0000,  "DISP_POLL",         ManuvrMsg::MSG_ARGS_NONE }, //
  {  MANUVR_MSG_DIRTY_FRAME_BUF      , 0x0000,  "DIRTY_FRAME_BUF",   ManuvrMsg::MSG_ARGS_NONE }, // Something changed the framebuffer and we need to redraw.
};

const I2CAdapterOptions i2c_opts(
  0,    // Device number
  18,   // sda
  19    // scl
);


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

  gpioDefine(4,  OUTPUT);

  I2CAdapter* i2c = new I2CAdapter(&i2c_opts);
  kernel->subscribe(i2c);

  INA219*  ina219  = new INA219(0x4A);
  TMP006*  tmp006  = new TMP006(0x41, 7);    // DRDY is pin 7.
  TSL2561* tsl2561 = new TSL2561(0x39, 6);   // IRQ is pin 6.
  Si7021*  si7021  = new Si7021();
  VEML6070* veml6070 = new VEML6070(5);      // ACK in on pin 5.
  i2c->addSlaveDevice(ina219);
  i2c->addSlaveDevice(tmp006);
  i2c->addSlaveDevice(tsl2561);
  i2c->addSlaveDevice(si7021);
  i2c->addSlaveDevice(veml6070);

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
