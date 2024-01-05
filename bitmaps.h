#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>

extern const uint8_t BITMAP_CurrentIndicator[8];

extern const uint8_t BITMAP_VFO_Default[8];
extern const uint8_t BITMAP_VFO_NotDefault[8];

#ifdef ENABLE_BATTERY_CHARGING
extern const uint8_t BITMAP_SettingsBattCharging[];
#endif
#endif
