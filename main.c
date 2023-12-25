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

#include "app/app.h"
#include "app/dtmf.h"
#ifdef ENABLE_MESSENGER
	#include "app/messenger.h"
#endif

#include "audio.h"
#if defined(ENABLE_AM_FIX)
#include "am_fix.h"
#endif
#include "board.h"
#include "bsp/dp32g030/gpio.h"
#include "bsp/dp32g030/syscon.h"
#include "driver/backlight.h"
#include "driver/bk4819.h"
#ifdef ENABLE_LCD_CONTRAST_OPTION
#include "driver/st7565.h"
#endif
#include "driver/gpio.h"
#include "driver/system.h"
#include "driver/systick.h"
#include <string.h>
#if defined(ENABLE_UART)
#include "driver/uart.h"
#endif
#include "helper/battery.h"
#include "helper/boot.h"
#include "misc.h"
#include "radio.h"
#include "settings.h"
#include "ui/lock.h"
#include "ui/menu.h"

#include "ui/welcome.h"
#include "version.h"

#include "driver/eeprom.h"

void _putchar(char c) {
#if defined(ENABLE_UART)
  UART_Send((uint8_t *)&c, 1);
#endif
}

void Main(void) {
  uint8_t i;

  // Enable clock gating of blocks we need.
  SYSCON_DEV_CLK_GATE = 0 | SYSCON_DEV_CLK_GATE_GPIOA_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_GPIOB_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_GPIOC_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_UART1_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_SPI0_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_SARADC_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_CRC_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_AES_BITS_ENABLE;

  SYSTICK_Init();
  BOARD_Init();

#if defined(ENABLE_UART)
  UART_Init();
  UART_Send(UART_Version, sizeof(UART_Version));
#endif

  // Not implementing authentic device checks

  memset(&gEeprom, 0, sizeof(gEeprom));
#ifdef ENABLE_DTMF_CALLING
  memset(gDTMF_String, '-', sizeof(gDTMF_String));
  gDTMF_String[14] = 0;
#endif
  BK4819_Init();
  BOARD_ADC_GetBatteryInfo(&gBatteryCurrentVoltage, &gBatteryCurrent);



  BOARD_EEPROM_Init();
  BOARD_EEPROM_LoadCalibration();

  #ifdef ENABLE_LCD_CONTRAST_OPTION
    ST7565_SetContrast(gEeprom.LCD_CONTRAST);
	#endif

  RADIO_ConfigureChannel(0, 2);
  RADIO_ConfigureChannel(1, 2);
  RADIO_SelectVfos();
  RADIO_SetupRegisters(true);

  for (i = 0; i < 4; i++) {
    BOARD_ADC_GetBatteryInfo(&gBatteryVoltages[i], &gBatteryCurrent);
  }

  BATTERY_GetReadings(false);
#ifdef ENABLE_MESSENGER
	MSG_Init();
#endif
#ifdef ENABLE_AM_FIX
  AM_fix_init();
#endif
  if (!gChargingWithTypeC && !gBatteryDisplayLevel) {
    FUNCTION_Select(FUNCTION_POWER_SAVE);
    GPIO_ClearBit(&GPIOB->DATA, GPIOB_PIN_BACKLIGHT);
    gReducedService = true;
  } else {
    BOOT_Mode_t BootMode;
	/*
    EEPROM_WriteBuffer(0x0E98, "\x4f");
    EEPROM_WriteBuffer(0x0E99, "\xa5");
    EEPROM_WriteBuffer(0x0E9A, "\x05");
  //EEPROM_WriteBuffer(0x0E9B, "\x00");
    SYSTEM_DelayMs(200);
	*/
	
	
    UI_DisplayWelcome();
    BACKLIGHT_TurnOn();
    SYSTEM_DelayMs(2500);
#if defined(ENABLE_DTMF_CALLING) && !defined(ENABLE_STATUS_BATTERY_PERC) && defined(ENABLE_ROGERBEEP)		
		gMenuListCount = 49;
#elif defined(ENABLE_DTMF_CALLING) && !defined(ENABLE_STATUS_BATTERY_PERC) && !defined(ENABLE_ROGERBEEP)		
		gMenuListCount = 48;		
#elif defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) && defined(ENABLE_ROGERBEEP)		
		gMenuListCount = 50;
#elif defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) && !defined(ENABLE_ROGERBEEP)		
		gMenuListCount = 49;		
#elif !defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) && defined(ENABLE_ROGERBEEP)
		gMenuListCount = 40;
#elif !defined(ENABLE_DTMF_CALLING) && defined(ENABLE_STATUS_BATTERY_PERC) && !defined(ENABLE_ROGERBEEP)
		gMenuListCount = 39;		
#else
		gMenuListCount = 38;
#endif	
#if defined(ENABLE_LCD_CONTRAST_OPTION)
  gMenuListCount += 1;
#endif

    BootMode = BOOT_GetMode();
    if (gEeprom.POWER_ON_PASSWORD < 1000000) {
      bIsInLockScreen = true;
      UI_DisplayLock();
      bIsInLockScreen = false;
    }

    BOOT_ProcessMode(BootMode);

    gUpdateStatus = true;
  }

  while (1) {
    APP_Update();
    if (gNextTimeslice) {
      APP_TimeSlice10ms();
      gNextTimeslice = false;
    }
    if (gNextTimeslice500ms) {
      APP_TimeSlice500ms();
      gNextTimeslice500ms = false;
    }
  }
}
