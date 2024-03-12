/* Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include "battery.h"
#include <string.h>
#if defined(ENABLE_FMRADIO)
#include "app/fm.h"
#endif
#ifdef ENABLE_MESSENGER
	#include "app/messenger.h"
#endif
#include "../bitmaps.h"
#include "../driver/keyboard.h"
#include "../driver/st7565.h"
#include "../external/printf/printf.h"
#include "../functions.h"
#include "../helper/battery.h"
#include "../misc.h"
#include "../settings.h"
#include "../ui/helper.h"
#include "status.h"
#ifdef ENABLE_DOCK
	#include "app/uart.h"
#endif
#if defined(ENABLE_MISSED_CALL_NOTIFICATION_AND_BLINKING_LED)
#include <string.h> 
#endif
#ifdef ENABLE_MESSENGER
	#ifdef ENABLE_STATUS_BATTERY_PERC
		const uint8_t MSG_ICO_POS = 85;
	#else
		const uint8_t MSG_ICO_POS = 105;
	#endif
  #endif
void UI_DisplayStatus(void) {



  // memset(gStatusLine, 64, sizeof(gStatusLine));
  memset(gStatusLine, 0, sizeof(gStatusLine));
  #ifdef ENABLE_DOCK
		uint8_t val1=0;
		uint8_t val2=0;	
		uint8_t val3=0;	
	#endif

  if (gBatteryDisplayLevel < 2) {
    if (gLowBatteryBlink == 1) {
      UI_DisplayBattery(1);
    }
  } else {
    UI_DisplayBattery(gBatteryDisplayLevel);
  }
#if defined(ENABLE_MISSED_CALL_NOTIFICATION_AND_BLINKING_LED)

const uint8_t BITMAP_MISSED = 65;

if (gMissedCalls > 0) { // Indicador de chamada perdida
    gStatusLine[BITMAP_MISSED + 0] |= 0b00000001;
    gStatusLine[BITMAP_MISSED + 1] |= 0b00000010;
    gStatusLine[BITMAP_MISSED + 2] |= 0b00000100;
    gStatusLine[BITMAP_MISSED + 3] |= 0b00001000;
    gStatusLine[BITMAP_MISSED + 4] |= 0b00010000;
    gStatusLine[BITMAP_MISSED + 5] |= 0b00100000;
    gStatusLine[BITMAP_MISSED + 6] |= 0b01000000;
    gStatusLine[BITMAP_MISSED + 7] |= 0b00100000;
    gStatusLine[BITMAP_MISSED + 8] |= 0b00010100;
    gStatusLine[BITMAP_MISSED + 9] |= 0b00001100;
    gStatusLine[BITMAP_MISSED + 10] |= 0b00001100;
    gStatusLine[BITMAP_MISSED + 11] |= 0b00001110;
}


#endif
#ifdef ENABLE_MESSENGER
	#ifdef ENABLE_STATUS_BATTERY_PERC
		const uint8_t MSG_ICO_POS = 85;
	#else
		const uint8_t MSG_ICO_POS = 105;
	#endif
	if (hasNewMessage) { // New Message indicator	
    gStatusLine[MSG_ICO_POS + 0] |= 0b00000000;
    gStatusLine[MSG_ICO_POS + 1] |= 0b00111111;
    gStatusLine[MSG_ICO_POS + 2] |= 0b00100011;
    gStatusLine[MSG_ICO_POS + 3] |= 0b00100101;
    gStatusLine[MSG_ICO_POS + 4] |= 0b00101001;
    gStatusLine[MSG_ICO_POS + 5] |= 0b00100101;
    gStatusLine[MSG_ICO_POS + 6] |= 0b00100011;
    gStatusLine[MSG_ICO_POS + 7] |= 0b00111111;
	}
#endif

  bool isPowerSave = gCurrentFunction == FUNCTION_POWER_SAVE;
  bool isKeyLock = gEeprom.KEY_LOCK;
  bool isFPressed = gWasFKeyPressed;
  bool isVox = gEeprom.VOX_SWITCH;
  bool isWx = gEeprom.CROSS_BAND_RX_TX != CROSS_BAND_OFF;
  bool isDw = gEeprom.DUAL_WATCH != DUAL_WATCH_OFF;
#if defined(ENABLE_FMRADIO)
  bool isFm = gFmRadioMode;
#else
  bool isFm = false;
#endif

 char String[32];
#ifdef ENABLE_STATUS_BATTERY_PERC
  sprintf(String, "%s %s %s %s %s %s %s      %u%%",
#else
  sprintf(String, "%s %s %s %s %s %s %s",
#endif
          isPowerSave ? "S" : " ", //
          isKeyLock ? "L" : " ",   //
          isFPressed ? "F" : " ",  //
          isVox ? "VOX" : "   ",   //
          isWx ? "WX" : "  ",      //
          isDw ? "DW" : "  ",      //
#ifdef ENABLE_STATUS_BATTERY_PERC
          isFm ? "FM" : "  ",       //
          BATTERY_VoltsToPercent(gBatteryVoltageAverage)
#else
          isFm ? "FM" : "  "       //
#endif		  
  );
  UI_PrintStringSmallest(String, 0, 0, true, true);

  ST7565_BlitStatusLine();

  #ifdef ENABLE_DOCK
    // -nicsure- bit 0 of val1 is TX
		//val1 |= 0b00000001;

    // -nicsure- bit 1 of val1 is RX
		//val1 |= 0b00000010;

    if(isPowerSave) {
      // -nicsure- bit 2 of val1 is PowerSave
		  val1 |= 0b00000100;
    }

    // -nicsure- bit 3 of val1 is NOAA
		//val1 |= 0b00001000;

    // -nicsure- bit 4 of val1 is DTMF
		//val1 |= 0b00010000;

    if(isFm) {
      // -nicsure- bit 5 of val1 is FM
		  val1 |= 0b00100000;
    }

    // -nicsure- val3 is the scan indicator
		//val3 = *s;

    // -nicsure- bit 6 of val1 is voice prompt
		//val1 |= 0b01000000;

    if(isDw) {
      // -nicsure- bit 7 of val1 is tdr1
		  val1 |= 0b10000000;
    } else {
      // -nicsure- bit 0 of val2 is tdr2
		  val2 |= 0b00000001;
    }

    if(isWx) {
      // -nicsure- bit 1 of val2 is xb
		  val2 |= 0b00000010;
    }

    if(isVox) {
      // -nicsure- bit 2 of val2 is vox
		  val2 |= 0b00000100;
    }

    if(isKeyLock) {
      // -nicsure- bit 3 of val2 is key lock
		  val2 |= 0b00001000;
    }

    if(isFPressed) {
      // -nicsure- bit 4 of val2 is F key mode
		  val2 |= 0b00010000;
    }
    
    // -nicsure- bit 5 of val2 is USB C Charging
		//val2 |= 0b00100000;
    
    UART_SendUiElement(5, 0, 0, 0, 0, NULL);
		UART_SendUiElement(6, val1, val2, val3, (gBatteryVoltageAverage>999?999:gBatteryVoltageAverage)>>2, NULL );
  #endif
}
