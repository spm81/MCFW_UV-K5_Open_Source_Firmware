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



#include <assert.h>
#include "battery.h"
#include "driver/backlight.h"
#include "driver/st7565.h"
#include "functions.h"
#include "misc.h"
#include "settings.h"
#include "ui/battery.h"
#include "ui/menu.h"
#include "ui/ui.h"
#include "../helper/measurements.h"

uint16_t gBatteryCalibration[6];
uint16_t gBatteryCurrentVoltage;
uint16_t gBatteryCurrent;
uint16_t gBatteryVoltages[4];
uint16_t gBatteryVoltageAverage;
uint16_t          gBatteryVoltageAverage;
uint8_t           gBatteryDisplayLevel;
bool              gChargingWithTypeC;
bool              gLowBatteryBlink;
bool              gLowBattery;
bool              gLowBatteryConfirmed;
uint16_t          gBatteryCheckCounter;
volatile uint16_t gBatterySave;


typedef enum {
	BATTERY_LOW_INACTIVE,
	BATTERY_LOW_ACTIVE,
	BATTERY_LOW_CONFIRMED
} BatteryLow_t;


uint16_t          lowBatteryCountdown;
const uint16_t 	  lowBatteryPeriod = 30;

volatile uint16_t gPowerSave_10ms;

#ifdef ENABLE_STATUS_BATTERY_PERC

const uint16_t Voltage2PercentageTable[][7][2] = {
	[BATTERY_TYPE_1600_MAH] = {
		{828, 100},
		{814, 97 },
		{760, 25 },
		{729, 6  },
		{630, 0  },
		{0,   0  },
		{0,   0  },
	},

	[BATTERY_TYPE_2200_MAH] = {
		{832, 100},
		{813, 95 },
		{740, 60 },
		{707, 21 },
		{682, 5  },
		{630, 0  },
		{0,   0  },
	},
};

static_assert(ARRAY_SIZE(Voltage2PercentageTable[BATTERY_TYPE_1600_MAH]) ==
	ARRAY_SIZE(Voltage2PercentageTable[BATTERY_TYPE_2200_MAH]));

unsigned int BATTERY_VoltsToPercent(const unsigned int voltage_10mV)
{
	const uint16_t (*crv)[2] = Voltage2PercentageTable[gEeprom.BATTERY_TYPE];
	const int mulipl = 1000;
	for (unsigned int i = 1; i < ARRAY_SIZE(Voltage2PercentageTable[BATTERY_TYPE_2200_MAH]); i++) {
		if (voltage_10mV > crv[i][0]) {
			const int a = (crv[i - 1][1] - crv[i][1]) * mulipl / (crv[i - 1][0] - crv[i][0]);
			const int b = crv[i][1] - a * crv[i][0] / mulipl;
			const int p = a * voltage_10mV / mulipl + b;
			return MIN(p, 100);
		}
	}

	return 0;
}
#endif


void BATTERY_GetReadings(bool bDisplayBatteryLevel)
{
    uint16_t Voltage = (gBatteryVoltages[0] + gBatteryVoltages[1] + gBatteryVoltages[2] + gBatteryVoltages[3]) / 4;
    uint8_t PreviousBatteryLevel = gBatteryDisplayLevel;

    gBatteryDisplayLevel = (Voltage >= gBatteryCalibration[5]) ? 6 :
                           (Voltage >= gBatteryCalibration[4]) ? 5 :
                           (Voltage >= gBatteryCalibration[3]) ? 4 :
                           (Voltage >= gBatteryCalibration[2]) ? 3 :
                           (Voltage >= gBatteryCalibration[1]) ? 2 :
                           (Voltage >= gBatteryCalibration[0]) ? 1 : 0;

    gBatteryVoltageAverage = (Voltage * 760) / gBatteryCalibration[3];

    if (gScreenToDisplay == DISPLAY_MENU && gMenuCursor == MENU_VOL)
        gUpdateDisplay = true;

    if ((gBatteryCurrent < 501) != gChargingWithTypeC) {
        gUpdateStatus = true;
        gChargingWithTypeC = (gBatteryCurrent >= 501);
        if (!gChargingWithTypeC)
            BACKLIGHT_TurnOn();
    }

    if (PreviousBatteryLevel != gBatteryDisplayLevel) {
        gLowBattery = (gBatteryDisplayLevel < 2);
        gLowBatteryCountdown = 0;
        if (!gLowBattery && bDisplayBatteryLevel)
            UI_DisplayBattery(gBatteryDisplayLevel);
    }
}
//////2
/*
void BATTERY_GetReadings(bool bDisplayBatteryLevel)
{
	uint16_t Voltage = (gBatteryVoltages[0] + gBatteryVoltages[1] + gBatteryVoltages[2] + gBatteryVoltages[3]) / 4;
	uint8_t PreviousBatteryLevel = gBatteryDisplayLevel;

	if (Voltage >= gBatteryCalibration[5]) gBatteryDisplayLevel = 6;
	else if (Voltage >= gBatteryCalibration[4]) gBatteryDisplayLevel = 5;
	else if (Voltage >= gBatteryCalibration[3]) gBatteryDisplayLevel = 4;
	else if (Voltage >= gBatteryCalibration[2]) gBatteryDisplayLevel = 3;
	else if (Voltage >= gBatteryCalibration[1]) gBatteryDisplayLevel = 2;
	else if (Voltage >= gBatteryCalibration[0]) gBatteryDisplayLevel = 1;
	else gBatteryDisplayLevel = 0;

	gBatteryVoltageAverage = (Voltage * 760) / gBatteryCalibration[3];

	if (gScreenToDisplay == DISPLAY_MENU && gMenuCursor == MENU_VOL) gUpdateDisplay = true;

	if (gBatteryCurrent < 501) {
		if (gChargingWithTypeC) {
			gUpdateStatus = true;
			gChargingWithTypeC = 0;
		}
	} else {
		if (!gChargingWithTypeC) {
			gUpdateStatus = true;
			BACKLIGHT_TurnOn();
			gChargingWithTypeC = 1;
		}
	}

	if (PreviousBatteryLevel != gBatteryDisplayLevel) {
		gLowBattery = (gBatteryDisplayLevel < 2);
		gLowBatteryCountdown = 0;

		if (!gLowBattery && bDisplayBatteryLevel) {
			UI_DisplayBattery(gBatteryDisplayLevel);
		}
	}
}
*/
////2
/*
void BATTERY_GetReadings(bool bDisplayBatteryLevel)
{
	uint16_t Voltage;
	uint8_t PreviousBatteryLevel;

	PreviousBatteryLevel = gBatteryDisplayLevel;

	Voltage = (gBatteryVoltages[0] + gBatteryVoltages[1] + gBatteryVoltages[2] + gBatteryVoltages[3]) / 4;

	if (gBatteryCalibration[5] < Voltage) {
		gBatteryDisplayLevel = 6;
	} else if (gBatteryCalibration[4] < Voltage) {
		gBatteryDisplayLevel = 5;
	} else if (gBatteryCalibration[3] < Voltage) {
		gBatteryDisplayLevel = 4;
	} else if (gBatteryCalibration[2] < Voltage) {
		gBatteryDisplayLevel = 3;
	} else if (gBatteryCalibration[1] < Voltage) {
		gBatteryDisplayLevel = 2;
	} else if (gBatteryCalibration[0] < Voltage) {
		gBatteryDisplayLevel = 1;
	} else {
		gBatteryDisplayLevel = 0;
	}

	gBatteryVoltageAverage = (Voltage * 760) / gBatteryCalibration[3];

	if ((gScreenToDisplay == DISPLAY_MENU) && gMenuCursor == MENU_VOL) {
		gUpdateDisplay = true;
	}
	if (gBatteryCurrent < 501) {
		if (gChargingWithTypeC) {
			gUpdateStatus = true;
		}
		gChargingWithTypeC = 0;
	} else {
		if (!gChargingWithTypeC) {
			gUpdateStatus = true;
			BACKLIGHT_TurnOn();
		}
		gChargingWithTypeC = 1;
	}

	if (PreviousBatteryLevel != gBatteryDisplayLevel) {
		if (gBatteryDisplayLevel < 2) {
			gLowBattery = true;
		} else {
			gLowBattery = false;
			if (bDisplayBatteryLevel) {
				UI_DisplayBattery(gBatteryDisplayLevel);
			}
		}
		gLowBatteryCountdown = 0;
	}
}
*/
////1
/*
void BATTERY_GetReadings(bool bDisplayBatteryLevel) {
  uint8_t PreviousBatteryLevel = gBatteryDisplayLevel;
  uint16_t Voltage = Mid(gBatteryVoltages, ARRAY_SIZE(gBatteryVoltages));
  //const uint16_t Voltage = (gBatteryVoltages[0] + gBatteryVoltages[1] + gBatteryVoltages[2] + gBatteryVoltages[3]) / 4;
  
  gBatteryDisplayLevel = 0;
  for (int i = ARRAY_SIZE(gBatteryCalibration) - 1; i >= 0; --i) {
    if (Voltage > gBatteryCalibration[i]) {
  // old config gBatteryDisplayLevel = i + 1;		
      gBatteryDisplayLevel = i + 1;
      break;
    }
  }
  gBatteryVoltageAverage = (Voltage * 760) / gBatteryCalibration[3];
  if ((gScreenToDisplay == DISPLAY_MENU) && gMenuCursor == MENU_VOL) {
    gUpdateDisplay = true;
  }
  if (gBatteryCurrent < 501) {
    if (gChargingWithTypeC) {
      gUpdateStatus = true;
    }
    gChargingWithTypeC = false;
  } else {
    if (!gChargingWithTypeC) {
      gUpdateStatus = true;
      BACKLIGHT_TurnOn();
    }
    gChargingWithTypeC = true;
  }
  if (PreviousBatteryLevel != gBatteryDisplayLevel) {
    if (gBatteryDisplayLevel < 2) {
      gLowBattery = true;
    } else {
      gLowBattery = false;
      if (bDisplayBatteryLevel) {
        UI_DisplayBattery(gBatteryDisplayLevel);
      }
    }
    gLowBatteryCountdown = 0;
  }
}
*/

