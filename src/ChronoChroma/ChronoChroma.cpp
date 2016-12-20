/*
File:   ChronoChroma.cpp
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


#include "ChronoChroma.h"
#include <Platform/Platform.h>
#include "../RGBClock/RGBClock.h"

#include <Platform/Peripherals/SPI/SPIAdapter.h>

/*******************************************************************************
*      _______.___________.    ___   .___________. __    ______     _______.
*     /       |           |   /   \  |           ||  |  /      |   /       |
*    |   (----`---|  |----`  /  ^  \ `---|  |----`|  | |  ,----'  |   (----`
*     \   \       |  |      /  /_\  \    |  |     |  | |  |        \   \
* .----)   |      |  |     /  _____  \   |  |     |  | |  `----.----)   |
* |_______/       |__|    /__/     \__\  |__|     |__|  \______|_______/
*
* Static members and initializers should be located here.
*******************************************************************************/

RGBClock* rgb_clock = nullptr;

/*******************************************************************************
*   ___ _              ___      _ _              _      _
*  / __| |__ _ ______ | _ ) ___(_) |___ _ _ _ __| |__ _| |_ ___
* | (__| / _` (_-<_-< | _ \/ _ \ | / -_) '_| '_ \ / _` |  _/ -_)
*  \___|_\__,_/__/__/ |___/\___/_|_\___|_| | .__/_\__,_|\__\___|
*                                          |_|
* Constructors/destructors, class initialization functions and so-forth...
*******************************************************************************/
/**
* Vanilla constructor.
*/
ChronoChroma::ChronoChroma() : EventReceiver() {
  setReceiverName("ChronoChroma");
  SPIAdapter* _spi = new SPIAdapter(0, 12, 11, 13);
  platform.kernel()->subscribe((EventReceiver*) _spi);
  rgb_clock = new RGBClock(_spi, 10, 9);
}


/**
* Constructor. Takes arguments.
*
* @param   Argument* root_config
*/
ChronoChroma::ChronoChroma(Argument* root_config) : ChronoChroma() {
  erConfigure(root_config);
}


/**
* Unlike many of the other EventReceivers, THIS one needs to be able to be torn down.
*/
ChronoChroma::~ChronoChroma() {
  platform.kernel()->removeSchedule(&event_color_shift);
  platform.kernel()->removeSchedule(&event_display_refresh);
}




/*******************************************************************************
* ######## ##     ## ######## ##    ## ########  ######
* ##       ##     ## ##       ###   ##    ##    ##    ##
* ##       ##     ## ##       ####  ##    ##    ##
* ######   ##     ## ######   ## ## ##    ##     ######
* ##        ##   ##  ##       ##  ####    ##          ##
* ##         ## ##   ##       ##   ###    ##    ##    ##
* ########    ###    ######## ##    ##    ##     ######
*
* These are overrides from EventReceiver interface...
*******************************************************************************/

/**
* This is called when the kernel attaches the module.
* This is the first time the class can be expected to have kernel access.
*
* @return 0 on no action, 1 on action, -1 on failure.
*/
int8_t ChronoChroma::attached() {
  if (EventReceiver::attached()) {
    event_color_shift.repurpose(CHRONOCHROMA_MSG_COLOR_SHF, (EventReceiver*) this);
    event_color_shift.incRefs();
    event_color_shift.specific_target = (EventReceiver*) this;
    event_color_shift.alterSchedulePeriod(30);
    event_color_shift.autoClear(false);
    event_color_shift.alterScheduleRecurrence(-1);
    platform.kernel()->addSchedule(&event_color_shift);

    event_display_refresh.repurpose(CHRONOCHROMA_MSG_DISP_UPDATE, (EventReceiver*) this);
    event_display_refresh.incRefs();
    event_display_refresh.specific_target = (EventReceiver*) this;
    event_display_refresh.alterSchedulePeriod(1000);
    event_display_refresh.autoClear(false);
    event_display_refresh.alterScheduleRecurrence(-1);
    platform.kernel()->addSchedule(&event_display_refresh);
    return 1;
  }
  return 0;
}


/**
* We are being configured.
*
* @return 0 on no action, 1 on action, -1 on failure.
*/
int8_t ChronoChroma::erConfigure(Argument* opts) {
  int8_t return_value = 0;
  if (opts->retrieveArgByKey("somekey")) {
  }
  return return_value;
}


/**
* If we find ourselves in this fxn, it means an event that this class built (the argument)
*   has been serviced and we are now getting the chance to see the results. The argument
*   to this fxn will never be NULL.
*
* Depending on class implementations, we might choose to handle the completed Event differently. We
*   might add values to event's Argument chain and return RECYCLE. We may also free() the event
*   ourselves and return DROP. By default, we will return REAP to instruct the Kernel
*   to either free() the event or return it to it's preallocate queue, as appropriate. If the event
*   was crafted to not be in the heap in its own allocation, we will return DROP instead.
*
* @param  event  The event for which service has been completed.
* @return A callback return code.
*/
int8_t ChronoChroma::callback_proc(ManuvrMsg* event) {
  /* Setup the default return code. If the event was marked as mem_managed, we return a DROP code.
     Otherwise, we will return a REAP code. Downstream of this assignment, we might choose differently. */
  int8_t return_value = (0 == event->refCount()) ? EVENT_CALLBACK_RETURN_REAP : EVENT_CALLBACK_RETURN_DROP;

  /* Some class-specific set of conditionals below this line. */
  switch (event->eventCode()) {
    default:
      break;
  }

  return return_value;
}


int8_t ChronoChroma::notify(ManuvrMsg* active_event) {
  int8_t return_value = 0;

  switch (active_event->eventCode()) {
    case CHRONOCHROMA_MSG_COLOR_SHF:
      {
        uint32_t permute = randomInt();
        switch (permute % 3) {
          case 0:
            if (0x04 & permute) rgb_clock->incRed();
            else rgb_clock->decRed();
            break;
          case 1:
            if (0x04 & permute) rgb_clock->incGreen();
            else rgb_clock->decGreen();
            break;
          case 2:
            if (0x04 & permute) rgb_clock->incBlue();
            else rgb_clock->decBlue();
            break;
        }
      }
      rgb_clock->refresh();
      return_value++;
      break;
    case CHRONOCHROMA_MSG_DISP_UPDATE:
      return_value++;
      break;
    default:
      return_value += EventReceiver::notify(active_event);
      break;
  }

  flushLocalLog();
  return return_value;
}


/**
* Debug support method. This fxn is only present in debug builds.
*
* @param   StringBuilder* The buffer into which this fxn should write its output.
*/
void ChronoChroma::printDebug(StringBuilder *output) {
  EventReceiver::printDebug(output);
  rgb_clock->printDebug(output);
}


#if defined(MANUVR_CONSOLE_SUPPORT)
void ChronoChroma::procDirectDebugInstruction(StringBuilder *input) {
  char* str = input->position(0);
  char c    = *(str);

  switch (c) {
    case 'p':
      random_fill(rgb_clock->buffer(), rgb_clock->bufLen());
      rgb_clock->refresh();
      break;
    case 'w':
      rgb_clock->setGlobalColor(0x0FFF, 0x0FFF, 0x0FFF);
      break;
    case 'r':   // Display red.
      rgb_clock->setGlobalColor(0x0FFF, 0, 0);
      break;
    case 'g':   // Display green.
      rgb_clock->setGlobalColor(0, 0x0FFF, 0);
      break;
    case 'h':   // Display blue.
      rgb_clock->setGlobalColor(0, 0, 0x0FFF);
      break;
    case 'O':   // Blank or unblank the display.
    case 'o':
      rgb_clock->blank('o' == c);
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case '.':
    case '-':
    case '_':
      rgb_clock->showString(str);
      break;
    case 'u':
      rgb_clock->refresh();
      break;
    case 'K':
    case 'k':
      event_color_shift.enableSchedule('K' == c);
      break;
    case 'T':
    case 't':
      event_display_refresh.enableSchedule('T' == c);
      break;
    case 'z':
      rgb_clock->clearDisplay();
      break;
    default:
      EventReceiver::procDirectDebugInstruction(input);
      break;
  }

  flushLocalLog();
}
#endif  // MANUVR_CONSOLE_SUPPORT
