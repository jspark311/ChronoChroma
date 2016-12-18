/*
File:   ChronoChroma.h
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

*/


#ifndef __CHRONO_CHROMA_MODULE_H__
#define __CHRONO_CHROMA_MODULE_H__

#include <Platform/Platform.h>

#define CHRONOCHROMA_MSG_COLOR_SHF    0x7f21
#define CHRONOCHROMA_MSG_DISP_UPDATE  0x7f22

/*
* These state flags are hosted by the EventReceiver. This may change in the future.
* Might be too much convention surrounding their assignment across inherritence.
*/
#define CHRONOCHRMOA_FLAG_SHOWING_TIME 0x01  //
#define CHRONOCHRMOA_FLAG_RESERVED_6   0x02  //
#define CHRONOCHRMOA_FLAG_RESERVED_5   0x04  //
#define CHRONOCHRMOA_FLAG_RESERVED_4   0x08  //
#define CHRONOCHRMOA_FLAG_RESERVED_3   0x10  //
#define CHRONOCHRMOA_FLAG_RESERVED_2   0x20  //
#define CHRONOCHRMOA_FLAG_RESERVED_1   0x40  //
#define CHRONOCHRMOA_FLAG_RESERVED_0   0x80  //


class ChronoChroma : public EventReceiver {
  public:
    ChronoChroma();
    ChronoChroma(Argument*);
    ~ChronoChroma();

    /* Overrides from EventReceiver */
    void procDirectDebugInstruction(StringBuilder*);
    void printDebug(StringBuilder*);
    int8_t notify(ManuvrMsg*);
    int8_t callback_proc(ManuvrMsg*);
    int8_t erConfigure(Argument*);


  protected:
    int8_t attached();


  private:
    ManuvrMsg event_color_shift;
    ManuvrMsg event_display_refresh;
};

#endif   // __CHRONO_CHROMA_MODULE_H__
