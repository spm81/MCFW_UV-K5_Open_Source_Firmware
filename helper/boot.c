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

#include "app/aircopy.h"
#include "bsp/dp32g030/gpio.h"
#include "driver/bk4819.h"
//////////////////////////////////// Erase Passwords
#include "../driver/st7565.h"
#include "ARMCM0.h"
#include "ui/helper.h"
////////////////////////////////////
#include "driver/gpio.h"
#include "driver/keyboard.h"
#include "driver/system.h"
#include "helper/boot.h"
#include "misc.h"
#include "radio.h"
#include "settings.h"
#include "ui/menu.h"
#include "ui/ui.h"
#include "ui/welcome.h"
#include "../driver/eeprom.h"
#include <stdio.h>

BOOT_Mode_t BOOT_GetMode(void) {
  KEY_Code_t Keys[2];
  uint8_t i;

  for (i = 0; i < 2; i++) {
    if (GPIO_CheckBit(&GPIOC->DATA, GPIOC_PIN_PTT)) {
      return BOOT_MODE_NORMAL;
    }
    Keys[i] = KEYBOARD_Poll();
    SYSTEM_DelayMs(20);
  }
  if (Keys[0] == Keys[1]) {
    gKeyReading0 = Keys[0];
    gKeyReading1 = Keys[0];
    gDebounceCounter = 2;
    if (Keys[0] == KEY_SIDE1) {
      return BOOT_MODE_F_LOCK;
    }
#if defined(ENABLE_AIRCOPY)
    if (Keys[0] == KEY_SIDE2) {
      return BOOT_MODE_AIRCOPY;
    }

#endif
    /*
    ////Erase with FF 512 EEPROM after offset 0x2000
                    if (Keys[0] == KEY_MENU) {

                    uint8_t data[16];
    memset(data, 0xff, sizeof(data));

    for (int i = 0x2000; i <= 0x10000; i += 16) {
        EEPROM_WriteBuffer(i, data);
    }
    return BOOT_MODE_NORMAL;
    }
    */


    
      ////Erase Password Lock & Messenger
    if (Keys[0] == KEY_MENU) {
      char String[16];
      memset(String, 0, sizeof(String));
      UI_DisplayClear();
      UI_PrintStringSmallBold("ERASING PASSWORDS", 0, 127, 0);
      UI_PrintStringSmallBold("OF LOCK &", 0, 127, 2);
      UI_PrintStringSmallBold("MESSENGER", 0, 127, 4);
      ST7565_BlitFullScreen();
      i = 0;
      while (i < 50) // 500ms
      {
        i = (GPIO_CheckBit(&GPIOC->DATA, GPIOC_PIN_PTT) &&
             KEYBOARD_Poll() == KEY_INVALID)
                ? i + 1
                : 0;
        SYSTEM_DelayMs(10);
      }

      uint8_t data[16];
      memset(data, 0xff, sizeof(data));

      // for (int i = 0x2000; i <= 0x10000; i += 16) {
      for (int i = 0x1D20; i <= 0x1D30; i += 16) {
        EEPROM_WriteBuffer(i, data);
      }
      EEPROM_WriteBuffer(0x0E98, "\xff\xff\xff\xff");

      NVIC_SystemReset();
    }
      }
  return BOOT_MODE_NORMAL;
}

void BOOT_ProcessMode(BOOT_Mode_t Mode) {
  if (Mode == BOOT_MODE_F_LOCK) {
    gMenuCursor = MENU_350TX;
    gSubMenuSelection = gSetting_350TX;
    GUI_SelectNextDisplay(DISPLAY_MENU);

    /*
    #if defined(ENABLE_DTMF_CALLING) && !defined(ENABLE_STATUS_BATTERY_PERC) &&
    defined(ENABLE_ROGERBEEP) && !defined(ENABLE_LCD_CONTRAST_OPTION)
    //#ifdef ENABLE_DTMF_CALLING
                    gMenuListCount = 55;
    #elif defined(ENABLE_DTMF_CALLING) && !defined(ENABLE_STATUS_BATTERY_PERC)
    && defined(ENABLE_ROGERBEEP) && defined(ENABLE_LCD_CONTRAST_OPTION)
    //#ifdef ENABLE_DTMF_CALLING
                    gMenuListCount = 56;
    #elif defined(ENABLE_DTMF_CALLING) && !defined(ENABLE_STATUS_BATTERY_PERC)
    && !defined(ENABLE_ROGERBEEP) && !defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 54;
    #elif defined(ENABLE_DTMF_CALLING) && !defined(ENABLE_STATUS_BATTERY_PERC)
    && !defined(ENABLE_ROGERBEEP) && defined(ENABLE_MDC) &&
    !defined(ENABLE_LCD_CONTRAST_OPTION) gMenuListCount = 55; #elif
    defined(ENABLE_DTMF_CALLING) && !defined(ENABLE_STATUS_BATTERY_PERC) &&
    !defined(ENABLE_ROGERBEEP) && defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 55;
    #elif defined(ENABLE_DTMF_CALLING) && !defined(ENABLE_STATUS_BATTERY_PERC)
    && !defined(ENABLE_ROGERBEEP) && defined(ENABLE_MDC) &&
    defined(ENABLE_LCD_CONTRAST_OPTION) gMenuListCount = 56; #elif
    defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) &&
    defined(ENABLE_ROGERBEEP) && !defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 56;
    #elif defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) &&
    defined(ENABLE_ROGERBEEP) && defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 57;
    #elif defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) &&
    !defined(ENABLE_ROGERBEEP) && !defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 55;
    #elif defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) &&
    !defined(ENABLE_ROGERBEEP) && defined(ENABLE_MDC) &&
    !defined(ENABLE_LCD_CONTRAST_OPTION) gMenuListCount = 56; #elif
    defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) &&
    !defined(ENABLE_ROGERBEEP) && defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 56;
    #elif defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) &&
    !defined(ENABLE_ROGERBEEP) && defined(ENABLE_MDC) &&
    defined(ENABLE_LCD_CONTRAST_OPTION) gMenuListCount = 57; #elif
    !defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) &&
    defined(ENABLE_ROGERBEEP) && !defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 46;
    #elif !defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC)
    && defined(ENABLE_ROGERBEEP) && defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 47;
    #elif !defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC)
    && !defined(ENABLE_ROGERBEEP) && !defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 45;
    #elif !defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC)
    && !defined(ENABLE_ROGERBEEP) && defined(ENABLE_MDC) &&
    !defined(ENABLE_LCD_CONTRAST_OPTION) gMenuListCount = 46; #elif
    !defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) &&
    !defined(ENABLE_ROGERBEEP) && defined(ENABLE_LCD_CONTRAST_OPTION)
                    gMenuListCount = 46;
    #elif !defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC)
    && !defined(ENABLE_ROGERBEEP) && defined(ENABLE_MDC) &&
    defined(ENABLE_LCD_CONTRAST_OPTION) gMenuListCount = 47; #else
                    gMenuListCount = 44;
    #endif	*/

#if defined(ENABLE_DTMF_CALLING)
#if defined(ENABLE_STATUS_BATTERY_PERC)
#if defined(ENABLE_ROGER_DEFAULT) || defined(ENABLE_ROGER_MOTOTRBO) ||         \
    defined(ENABLE_ROGER_TPT) || defined(ENABLE_ROGER_MOTOTRBOT40) ||          \
    defined(ENABLE_ROGER_MOTOTRBOTLKRT80) ||                                   \
    defined(ENABLE_ROGER_ROGERCOBRAAM845) ||                                   \
    defined(ENABLE_ROGER_POLICE_ITA) || defined(ENABLE_ROGER_UV5RC) ||         \
    defined(ENABLE_ROGER_MARIO) || defined(ENABLE_MDC)
    gMenuListCount = 55;
#elif defined(ENABLE_LCD_CONTRAST_OPTION)
    gMenuListCount++;
#else
    gMenuListCount = 53;
#endif
#else
#if defined(ENABLE_ROGER_DEFAULT) || defined(ENABLE_ROGER_MOTOTRBO) ||         \
    defined(ENABLE_ROGER_TPT) || defined(ENABLE_ROGER_MOTOTRBOT40) ||          \
    defined(ENABLE_ROGER_MOTOTRBOTLKRT80) ||                                   \
    defined(ENABLE_ROGER_ROGERCOBRAAM845) ||                                   \
    defined(ENABLE_ROGER_POLICE_ITA) || defined(ENABLE_ROGER_UV5RC) ||         \
    defined(ENABLE_ROGER_MARIO) || defined(ENABLE_MDC)
    gMenuListCount = 54;
#elif defined(ENABLE_LCD_CONTRAST_OPTION)
    gMenuListCount++;
#else
    gMenuListCount = 52;
#endif
#endif
#elif defined(ENABLE_STATUS_BATTERY_PERC)
#if defined(ENABLE_ROGER_DEFAULT) || defined(ENABLE_ROGER_MOTOTRBO) ||         \
    defined(ENABLE_ROGER_TPT) || defined(ENABLE_ROGER_MOTOTRBOT40) ||          \
    defined(ENABLE_ROGER_MOTOTRBOTLKRT80) ||                                   \
    defined(ENABLE_ROGER_ROGERCOBRAAM845) ||                                   \
    defined(ENABLE_ROGER_POLICE_ITA) || defined(ENABLE_ROGER_UV5RC) ||         \
    defined(ENABLE_ROGER_MARIO) || defined(ENABLE_MDC)
    gMenuListCount = 46;
#elif defined(ENABLE_LCD_CONTRAST_OPTION)
    gMenuListCount++;
#endif
#else
    gMenuListCount = 44;
#endif

#ifdef ENABLE_LIVESEEK_MHZ_KEYPAD
#endif
#ifdef ENABLE_DOCK
    gMenuListCount++;
#endif
#if defined(ENABLE_LCD_INVERT_OPTION)
    gMenuListCount++;
#endif
#if defined(ENABLE_SATCOM_AMP)
    gMenuListCount++;
#endif
#if defined(ENABLE_PTT_HOLD)
    gMenuListCount++;
#endif
#if defined(ENABLE_MISSED_CALL_NOTIFICATION_AND_BLINKING_LED)
    gMenuListCount++;
#endif
#ifdef ENABLE_MESSENGER
    gMenuListCount += 2;
#ifdef ENABLE_MESSENGER_ROGERBEEP_NOTIFICATION
    gMenuListCount++;
#endif
#ifdef ENABLE_MESSENGER_ENCRYPTION
    gMenuListCount += 2;
#endif
#endif

    // gMenuListCount = MENU_ITEMS_COUNT;
    gF_LOCK = true;
#if defined(ENABLE_AIRCOPY)
  } else if (Mode == BOOT_MODE_AIRCOPY) {
    gEeprom.DUAL_WATCH = DUAL_WATCH_OFF;
    gEeprom.BATTERY_SAVE = 0;
    gEeprom.VOX_SWITCH = false;
    gEeprom.CROSS_BAND_RX_TX = CROSS_BAND_OFF;
    gEeprom.AUTO_KEYPAD_LOCK = false;
    gEeprom.KEY_1_SHORT_PRESS_ACTION = 0;
    gEeprom.KEY_1_LONG_PRESS_ACTION = 0;
    gEeprom.KEY_2_SHORT_PRESS_ACTION = 0;
    gEeprom.KEY_2_LONG_PRESS_ACTION = 0;

    RADIO_InitInfo(gRxVfo, 205, 5, 41002500);
    gRxVfo->CHANNEL_BANDWIDTH = BANDWIDTH_NARROW;
    gRxVfo->OUTPUT_POWER = 0;
    RADIO_ConfigureSquelchAndOutputPower(gRxVfo);
    gCurrentVfo = gRxVfo;
    RADIO_SetupRegisters(true);
    BK4819_SetupAircopy();
    BK4819_ResetFSK();
    gAircopyState = AIRCOPY_READY;
    GUI_SelectNextDisplay(DISPLAY_AIRCOPY);
#endif
  } else {
    GUI_SelectNextDisplay(DISPLAY_MAIN);
  }
}
