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

#include "rssi.h"
#include "../driver/st7565.h"
#ifdef ENABLE_MIC_PLUS_GAIN_BAR_TX
	#include "../driver/st7565.h"
	#include "../driver/bk4819-regs.h"
	#include "../driver/bk4819.h"
#endif
#include "../external/printf/printf.h"
#include "../helper/measurements.h"
#include "../misc.h"
#include "../settings.h"
#include "../ui/helper.h"
#include <string.h>

void UI_DisplayRSSIBar(int16_t rssi) {
  char String[16];

  const uint8_t LINE = 3;
  const uint8_t BAR_LEFT_MARGIN = 24;

  int dBm = Rssi2DBm(rssi);
  uint8_t s = DBm2S(dBm);
  uint8_t *line = gFrameBuffer[LINE];

  memset(line, 0, 128);

  for (int i = BAR_LEFT_MARGIN, sv = 1; i < BAR_LEFT_MARGIN + s * 4;
       i += 4, sv++) {
    line[i] = line[i + 2] = 0b00111110;
    line[i + 1] = sv > 9 ? 0b00100010 : 0b00111110;
  }

  sprintf(String, "%d", dBm);
  UI_PrintStringSmallest(String, 110, 25, false, true);
  if (s < 10) {
    sprintf(String, "S%u", s);
  } else {
    sprintf(String, "S9+%u0", s - 9);
  }
  UI_PrintStringSmallest(String, 3, 25, false, true);
  ST7565_BlitFullScreen();
}



#ifdef ENABLE_MIC_PLUS_GAIN_BAR_TX
void UI_DisplayMICBar() {
 char String[16];
  const uint8_t LINE = 3;
 // const uint8_t BAR_LEFT_MARGIN = 24;
  uint8_t *line = gFrameBuffer[LINE];
  memset(line, 0, 128);

    uint8_t afDB = BK4819_ReadRegister(0x6F) & 0b1111111;
    uint8_t afPX = ConvertDomain(afDB, 26, 194, 0, 121);
    for (uint8_t i = 0, sv = 1; i < afPX*2; i+=4, sv++) {
       line[i] = line[i + 2] = 0b00111110;
       line[i + 1] = sv > 9 ? 0b00100010 : 0b00111110;
    }

  sprintf(String, "%d dB", afDB);
  UI_PrintStringSmallest(String, 109, 25, false, true);
 /* if (afDB < 10) {
    sprintf(String, "a%u", afDB);
  } else {
    sprintf(String, "aa%u0", afDB - 9);
  }
  UI_PrintStringSmallest(String, 3, 25, false, true);*/
  ST7565_BlitFullScreen();
}
#endif