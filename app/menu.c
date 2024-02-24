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

#include "finput.h"
#include <string.h>
#if !defined(ENABLE_OVERLAY)
#include "ARMCM0.h"
#endif
#ifdef ENABLE_DTMF_CALLING
#include "app/dtmf.h"
#endif
#include "app/generic.h"
#include "app/menu.h"
#include "app/scanner.h"
#include "audio.h"
#include "board.h"
#include "bsp/dp32g030/gpio.h"
#include "driver/backlight.h"
#ifdef ENABLE_LCD_CONTRAST_OPTION
#include "driver/st7565.h"
#endif
#include "driver/gpio.h"
#include "driver/keyboard.h"
#include "frequencies.h"
#include "misc.h"
#include "settings.h"
#if defined(ENABLE_OVERLAY)
#include "sram-overlay.h"
#endif
#include "ui/inputbox.h"
#include "ui/menu.h"
#include "ui/ui.h"
#ifdef ENABLE_LIVESEEK_MHZ_KEYPAD
#include "app/ceccommon.h"
#endif

/*#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif
*/
void MENU_StartCssScan(int8_t Direction) {
  gCssScanMode = CSS_SCAN_MODE_SCANNING;
  gMenuScrollDirection = Direction;
  RADIO_SelectVfos();
  MENU_SelectNextCode();
  ScanPauseDelayIn10msec = 50;
  gScheduleScanListen = false;
}

void MENU_StopCssScan(void) {
  gCssScanMode = CSS_SCAN_MODE_OFF;
  RADIO_SetupRegisters(true);
}

int MENU_GetLimits(uint8_t Cursor, uint8_t *pMin, uint8_t *pMax) {
	*pMin = 0;	
  switch (Cursor) {
#ifdef ENABLE_LIVESEEK_MHZ_KEYPAD
//============= KD8CEC WORK ==================
  case MENU_LIVESEEK:
  //*pMin = 0;
  *pMax = ARRAY_SIZE(gSubMenu_LIVESEEK) - 1;
  break;
//============= END OF KD8CEC WORK ===========
#endif		
  case MENU_SQL:
    //*pMin = 0;
    *pMax = 9;
    break;
  case MENU_STEP:
    //*pMin = 0;
    *pMax = 18;
    break;
  case MENU_ABR:
    //*pMin = 0;
    *pMax = 6;
    break;
  case MENU_F_LOCK:
    //*pMin = 0;
    *pMax = 4;
    break;
#ifdef ENABLE_LCD_CONTRAST_OPTION
  case MENU_CONTRAST:
		*pMin = 1;
		*pMax = 63;
		break;	
#endif

  case MENU_MDF:
    //*pMin = 0;
    *pMax = 3;
    break;
  case MENU_TXP:
  case MENU_SFT_D:
  case MENU_TDR:
  case MENU_WX:
 // case MENU_VOICE:
  case MENU_SC_REV:
  case MENU_PONMSG:
  case MENU_ALL_TX:
    //*pMin = 0;
    *pMax = 2;
    break;
#if defined (ENABLE_ROGERBEEP) || defined (ENABLE_MDC)	
  case MENU_ROGER:
    //*pMin = 0;
	*pMax = ARRAY_SIZE(gSubMenu_ROGER) - 1;
    break;
#endif
	
  case MENU_UPCONVERTER:
    //*pMin = 0;
    *pMax = 6;
    break;
  case MENU_R_DCS:
  case MENU_T_DCS:
    //*pMin = 0;
    *pMax = 208;
    break;
  case MENU_R_CTCS:
  case MENU_T_CTCS:
    //*pMin = 0;
    *pMax = 50;
    break;
#ifdef ENABLE_DOCK
	case MENU_REMOTE_UI:
#endif    
  case MENU_BCL:
  case MENU_BEEP:
  case MENU_AUTOLK:
  case MENU_S_ADD1:
  case MENU_S_ADD2:
  case MENU_STE:
#ifdef ENABLE_DTMF_CALLING
  case MENU_D_ST:
  case MENU_D_DCD:
#endif  
  case MENU_RESET:
  case MENU_350TX:
  case MENU_200TX:
  case MENU_500TX:
  case MENU_SCREN:
#ifdef ENABLE_MESSENGER_ENCRYPTION
		case MENU_MSG_ENC:
#endif
#if defined (ENABLE_MESSENGER) && defined(ENABLE_MESSENGER_ROGERBEEP_NOTIFICATION)
		case MENU_MSG_NOTIFICATION:
#endif	
#ifdef ENABLE_MESSENGER
		case MENU_MSG_ACK:
#endif
	
    //*pMin = 0;
    *pMax = 1;
    break;
#ifdef ENABLE_MESSENGER
		case MENU_MSG_MODULATION:
			*pMin = 0;
			*pMax = ARRAY_SIZE(gSubMenu_MSG_MODULATION) - 1;
			break;
#endif    
  case MENU_W_N:
    //*pMin = 0;
    *pMax = ARRAY_SIZE(bwNames) - 1;
    break;
#ifdef ENABLE_STATUS_BATTERY_PERC	
  case MENU_BATTYP:
	//*pMin = 0;
	*pMax = 1;
	break;
#endif	
  case MENU_AM:
    //*pMin = 0;
    *pMax = ARRAY_SIZE(modulationTypeOptions) - 1;
    break;
  case MENU_SCR:
  case MENU_VOX:
  case MENU_TOT:
  case MENU_RP_STE:
    //*pMin = 0;
    *pMax = 10;
    break;
  case MENU_MEM_CH:
  case MENU_1_CALL:
  case MENU_SLIST1:
  case MENU_SLIST2:
  /*  *pMin = -1;
    *pMax = FREQ_CHANNEL_LAST;
	*/
	//*pMin = 0;
    *pMax = 199;
    break;  
  case MENU_DEL_CH:
    //*pMin = 0;
    *pMax = 199;
    break;
  case MENU_SAVE:
  case MENU_MIC:
    //*pMin = 0;
    *pMax = 4;
    break;
  case MENU_S_LIST:
    //*pMin = 0;
    *pMax = 2;
    break;
#ifdef ENABLE_DTMF_CALLING
  case MENU_D_RSP:
  case MENU_PTT_ID:
    //*pMin = 0;
    *pMax = 3;
    break;
  case MENU_D_HOLD:
    *pMin = 5;
    *pMax = 60;
    break;
  case MENU_D_PRE:
    *pMin = 3;
    *pMax = 99;
    break;
  case MENU_D_LIST:
    *pMin = 1;
    *pMax = 16;
    break;
#endif
  default:
    return -1;
  }

  return 0;
}

void MENU_AcceptSetting(void) {
  uint8_t Min, Max;
  uint8_t Code;
  FREQ_Config_t *pConfig = &gTxVfo->ConfigRX;

  if (!MENU_GetLimits(gMenuCursor, &Min, &Max)) {
    if (gSubMenuSelection < Min) {
      gSubMenuSelection = Min;
    } else if (gSubMenuSelection > Max) {
      gSubMenuSelection = Max;
    }
  }

  switch (gMenuCursor) {
  case MENU_SQL:
    gEeprom.SQUELCH_LEVEL = gSubMenuSelection;
    gRequestSaveSettings = true;
    gVfoConfigureMode = VFO_CONFIGURE_1;
    return;

  case MENU_STEP:
    if (IS_FREQ_CHANNEL(gTxVfo->CHANNEL_SAVE)) {
      gTxVfo->STEP_SETTING = gSubMenuSelection;
      gRequestSaveChannel = 1;
      return;
    }
    gSubMenuSelection = gTxVfo->STEP_SETTING;
    return;

  case MENU_TXP:
    gTxVfo->OUTPUT_POWER = gSubMenuSelection;
    gRequestSaveChannel = 1;
    return;

  case MENU_T_DCS:
    pConfig = &gTxVfo->ConfigTX;
    // Fallthrough
  case MENU_R_DCS:
    if (gSubMenuSelection == 0) {
      if (pConfig->CodeType != CODE_TYPE_DIGITAL &&
          pConfig->CodeType != CODE_TYPE_REVERSE_DIGITAL) {
        gRequestSaveChannel = 1;
        return;
      }
      Code = 0;
      pConfig->CodeType = CODE_TYPE_OFF;
    } else if (gSubMenuSelection < 105) {
      pConfig->CodeType = CODE_TYPE_DIGITAL;
      Code = gSubMenuSelection - 1;
    } else {
      pConfig->CodeType = CODE_TYPE_REVERSE_DIGITAL;
      Code = gSubMenuSelection - 105;
    }
    pConfig->Code = Code;
    gRequestSaveChannel = 1;
    return;

  case MENU_T_CTCS:
    pConfig = &gTxVfo->ConfigTX;
    // Fallthrough
  case MENU_R_CTCS:
    if (gSubMenuSelection == 0) {
      if (pConfig->CodeType != CODE_TYPE_CONTINUOUS_TONE) {
        gRequestSaveChannel = 1;
        return;
      }
      Code = 0;
      pConfig->CodeType = CODE_TYPE_OFF;
    } else {
      pConfig->CodeType = CODE_TYPE_CONTINUOUS_TONE;
      Code = gSubMenuSelection - 1;
    }
    pConfig->Code = Code;
    gRequestSaveChannel = 1;
    return;

  case MENU_SFT_D:
    gTxVfo->FREQUENCY_DEVIATION_SETTING = gSubMenuSelection;
    gRequestSaveChannel = 1;
    return;

  case MENU_OFFSET:
    gTxVfo->FREQUENCY_OF_DEVIATION = gSubMenuSelection;
    gRequestSaveChannel = 1;
    return;

  case MENU_W_N:
    gTxVfo->CHANNEL_BANDWIDTH = gSubMenuSelection;
    gRequestSaveChannel = 1;
    return;

  case MENU_SCR:
    gTxVfo->SCRAMBLING_TYPE = gSubMenuSelection;
    gRequestSaveChannel = 1;
    return;

  case MENU_BCL:
    gTxVfo->BUSY_CHANNEL_LOCK = gSubMenuSelection;
    gRequestSaveChannel = 1;
    return;

  case MENU_MEM_CH:
    gTxVfo->CHANNEL_SAVE = gSubMenuSelection;
    gRequestSaveChannel = 2;
#ifndef ENABLE_KEEPNAMEONSAVE
    gEeprom.MrChannel[0] = gSubMenuSelection;
#else
    gEeprom.MrChannel[gEeprom.TX_CHANNEL] = gSubMenuSelection;
#endif
    return;

  case MENU_SAVE:
    gEeprom.BATTERY_SAVE = gSubMenuSelection;
    break;

  case MENU_VOX:
    gEeprom.VOX_SWITCH = gSubMenuSelection != 0;
    if (gEeprom.VOX_SWITCH) {
      gEeprom.VOX_LEVEL = gSubMenuSelection - 1;
    }
    BOARD_EEPROM_LoadCalibration();
    gFlagReconfigureVfos = true;
    gRequestSaveSettings = true;
    gUpdateStatus = true;
    return;

  case MENU_ABR:
    gEeprom.BACKLIGHT = gSubMenuSelection;
    if (gSubMenuSelection == 0) {
      GPIO_ClearBit(&GPIOB->DATA, GPIOB_PIN_BACKLIGHT);
    } else {
      BACKLIGHT_TurnOn();
    }
    break;

  case MENU_TDR:
    gEeprom.DUAL_WATCH = gSubMenuSelection;
    gFlagReconfigureVfos = true;
    gRequestSaveSettings = true;
    gUpdateStatus = true;
    return;

  case MENU_WX:
    gEeprom.CROSS_BAND_RX_TX = gSubMenuSelection;
    gFlagReconfigureVfos = true;
    gRequestSaveSettings = true;
    gUpdateStatus = true;
    return;

#ifdef ENABLE_DOCK
		case MENU_REMOTE_UI:
			gSetting_Remote_UI = gSubMenuSelection;
			break;
#endif

  case MENU_BEEP:
    gEeprom.BEEP_CONTROL = gSubMenuSelection;
    break;

  case MENU_TOT:
    gEeprom.TX_TIMEOUT_TIMER = gSubMenuSelection;
    break;
/*
  case MENU_VOICE:
    gEeprom.VOICE_PROMPT = gSubMenuSelection;
    gRequestSaveSettings = true;
    gUpdateStatus = true;
    return;
*/
  case MENU_SC_REV:
    gEeprom.SCAN_RESUME_MODE = gSubMenuSelection;
    break;

  case MENU_MDF:
    gEeprom.CHANNEL_DISPLAY_MODE = gSubMenuSelection;
    break;

#ifdef ENABLE_LIVESEEK_MHZ_KEYPAD
	case MENU_LIVESEEK:
	CEC_LiveSeekMode = gSubMenuSelection;
	break;
#endif			

  case MENU_AUTOLK:
    gEeprom.AUTO_KEYPAD_LOCK = gSubMenuSelection;
    gKeyLockCountdown = 0x1e;
    break;

  case MENU_S_ADD1:
    gTxVfo->SCANLIST1_PARTICIPATION = gSubMenuSelection;
    SETTINGS_UpdateChannel(gTxVfo->CHANNEL_SAVE, gTxVfo, true);
    gVfoConfigureMode = VFO_CONFIGURE_1;
    gFlagResetVfos = true;
    return;

  case MENU_S_ADD2:
    gTxVfo->SCANLIST2_PARTICIPATION = gSubMenuSelection;
    SETTINGS_UpdateChannel(gTxVfo->CHANNEL_SAVE, gTxVfo, true);
    gVfoConfigureMode = VFO_CONFIGURE_1;
    gFlagResetVfos = true;
    return;

  case MENU_STE:
    gEeprom.TAIL_NOTE_ELIMINATION = gSubMenuSelection;
    break;

  case MENU_RP_STE:
    gEeprom.REPEATER_TAIL_TONE_ELIMINATION = gSubMenuSelection;
    break;

  case MENU_MIC:
    gEeprom.MIC_SENSITIVITY = gSubMenuSelection;
    BOARD_EEPROM_LoadCalibration();
    gRequestSaveSettings = true;
    gFlagReconfigureVfos = true;
    return;

  case MENU_1_CALL:
    gEeprom.CHAN_1_CALL = gSubMenuSelection;
    break;

  case MENU_S_LIST:
  //  gEeprom.SCAN_LIST_DEFAULT = gSubMenuSelection - 1;
    gEeprom.SCAN_LIST_DEFAULT = gSubMenuSelection;
    break;
#ifdef ENABLE_DTMF_CALLING
  case MENU_D_ST:
    gEeprom.DTMF_SIDE_TONE = gSubMenuSelection;
    break;

  case MENU_D_RSP:
    gEeprom.DTMF_DECODE_RESPONSE = gSubMenuSelection;
    break;

  case MENU_D_HOLD:
    gEeprom.DTMF_AUTO_RESET_TIME = gSubMenuSelection;
    break;

  case MENU_D_PRE:
    gEeprom.DTMF_PRELOAD_TIME = gSubMenuSelection * 10;
    break;

  case MENU_PTT_ID:
    gTxVfo->DTMF_PTT_ID_TX_MODE = gSubMenuSelection;
    gRequestSaveChannel = 1;
    return;

  case MENU_D_DCD:
    gTxVfo->DTMF_DECODING_ENABLE = gSubMenuSelection;
    gRequestSaveChannel = 1;
    return;

  case MENU_D_LIST:
    gDTMFChosenContact = gSubMenuSelection - 1;
    if (gIsDtmfContactValid) {
      GUI_SelectNextDisplay(DISPLAY_MAIN);
      gDTMF_InputMode = true;
      gDTMF_InputIndex = 3;
      memcpy(gDTMF_InputBox, gDTMF_ID, 4);
      gRequestDisplayScreen = DISPLAY_INVALID;
    }
    return;
#endif
  case MENU_PONMSG:
    gEeprom.POWER_ON_DISPLAY_MODE = gSubMenuSelection;
    break;

#if defined (ENABLE_ROGERBEEP) || defined (ENABLE_MDC)
  case MENU_ROGER:
    gEeprom.ROGER = gSubMenuSelection;
    break;
#endif

#ifdef ENABLE_LCD_CONTRAST_OPTION
  case MENU_CONTRAST:
			gEeprom.LCD_CONTRAST = gSubMenuSelection;
			ST7565_SetContrast(gEeprom.LCD_CONTRAST);
			break;  
#endif
  case MENU_AM:
    gTxVfo->AM_CHANNEL_MODE = gSubMenuSelection;
    gRequestSaveChannel = 1;
    return;

  case MENU_DEL_CH:
    SETTINGS_UpdateChannel(gSubMenuSelection, NULL, false);
    gVfoConfigureMode = VFO_CONFIGURE_RELOAD;
    gFlagResetVfos = true;
    return;

  case MENU_RESET:
    BOARD_FactoryReset(gSubMenuSelection);
    return;
	
  case MENU_UPCONVERTER:
    gUpconverter = gSubMenuSelection;
    return;	

  case MENU_350TX:
    gSetting_350TX = gSubMenuSelection;
    break;

  case MENU_F_LOCK:
    gSetting_F_LOCK = gSubMenuSelection;
    break;

  case MENU_200TX:
    gSetting_200TX = gSubMenuSelection;
    break;

  case MENU_500TX:
    gSetting_500TX = gSubMenuSelection;
    break;

  case MENU_ALL_TX:
    gSetting_ALL_TX = gSubMenuSelection;
    gRequestSaveSettings = true;
    gVfoConfigureMode = VFO_CONFIGURE_RELOAD;
    gFlagResetVfos = true;
    return;
#ifdef ENABLE_STATUS_BATTERY_PERC	
  case MENU_BATTYP:
	gEeprom.BATTERY_TYPE = gSubMenuSelection;
	break;
#endif
  case MENU_SCREN:
    gSetting_ScrambleEnable = gSubMenuSelection;
    gRequestSaveSettings = true;
    gFlagReconfigureVfos = true;
    return;

#ifdef ENABLE_MESSENGER
	#ifdef ENABLE_MESSENGER_ENCRYPTION
			case MENU_ENC_KEY:
				memset(gEeprom.ENC_KEY, 0, sizeof(gEeprom.ENC_KEY));
				memmove(gEeprom.ENC_KEY, edit, sizeof(gEeprom.ENC_KEY));
				memset(edit, 0, sizeof(edit));
				gUpdateStatus        = true;
				break;

			case MENU_MSG_ENC:
				gEeprom.MESSENGER_CONFIG.data.encrypt = gSubMenuSelection;
        // Clear old KEY ???
        //if ( gEeprom.MESSENGER_CONFIG.data.encrypt == 0 ) {
        //  SETTINGS_ClearEncryptionKey();
        //}
				break;
	#endif

		
			#ifdef ENABLE_MESSENGER_ROGERBEEP_NOTIFICATION
				case MENU_MSG_NOTIFICATION:
				gEeprom.MESSENGER_CONFIG.data.notification = gSubMenuSelection;
				break;
			#endif
			case MENU_MSG_ACK:
				gEeprom.MESSENGER_CONFIG.data.ack = gSubMenuSelection;
				break;

			case MENU_MSG_MODULATION:
				gEeprom.MESSENGER_CONFIG.data.modulation = gSubMenuSelection;
				break;   
#endif
  default:
    return;
  }

  gRequestSaveSettings = true;
}

void MENU_SelectNextCode(void) {
  uint8_t UpperLimit;

  if (gMenuCursor == MENU_R_DCS) {
    UpperLimit = 208;
  } else if (gMenuCursor == MENU_R_CTCS) {
    UpperLimit = 50;
  } else {
    return;
  }

  gSubMenuSelection = NUMBER_AddWithWraparound(
      gSubMenuSelection, gMenuScrollDirection, 1, UpperLimit);
  if (gMenuCursor == MENU_R_DCS) {
    if (gSubMenuSelection > 104) {
      gSelectedCodeType = CODE_TYPE_REVERSE_DIGITAL;
      gSelectedCode = gSubMenuSelection - 105;
    } else {
      gSelectedCodeType = CODE_TYPE_DIGITAL;
      gSelectedCode = gSubMenuSelection - 1;
    }

  } else {
    gSelectedCodeType = CODE_TYPE_CONTINUOUS_TONE;
    gSelectedCode = gSubMenuSelection - 1;
  }

  RADIO_SetupRegisters(true);

  if (gSelectedCodeType == CODE_TYPE_CONTINUOUS_TONE) {
    ScanPauseDelayIn10msec = 20;
  } else {
    ScanPauseDelayIn10msec = 30;
  }

  gUpdateDisplay = true;
}

static void MENU_ClampSelection(int8_t Direction) {
  uint8_t Min, Max;

  if (!MENU_GetLimits(gMenuCursor, &Min, &Max)) {
    uint8_t Selection = gSubMenuSelection;

    if (Selection < Min) {
      Selection = Min;
    } else if (Selection > Max) {
      Selection = Max;
    }
    gSubMenuSelection =
        NUMBER_AddWithWraparound(Selection, Direction, Min, Max);
  }
}

void MENU_ShowCurrentSetting(void) {
  switch (gMenuCursor) {
  case MENU_SQL:
    gSubMenuSelection = gEeprom.SQUELCH_LEVEL;
    break;

  case MENU_STEP:
    gSubMenuSelection = gTxVfo->STEP_SETTING;
    break;

  case MENU_TXP:
    gSubMenuSelection = gTxVfo->OUTPUT_POWER;
    break;

  case MENU_R_DCS:
    switch (gTxVfo->ConfigRX.CodeType) {
    case CODE_TYPE_DIGITAL:
      gSubMenuSelection = gTxVfo->ConfigRX.Code + 1;
      break;
    case CODE_TYPE_REVERSE_DIGITAL:
      gSubMenuSelection = gTxVfo->ConfigRX.Code + 105;
      break;
    default:
      gSubMenuSelection = 0;
      break;
    }
    break;

  case MENU_RESET:
    gSubMenuSelection = 0;
    break;

  case MENU_UPCONVERTER:
    gSubMenuSelection = gUpconverter;
    break;
	
  case MENU_R_CTCS:
    if (gTxVfo->ConfigRX.CodeType == CODE_TYPE_CONTINUOUS_TONE) {
      gSubMenuSelection = gTxVfo->ConfigRX.Code + 1;
    } else {
      gSubMenuSelection = 0;
    }
    break;

  case MENU_T_DCS:
    switch (gTxVfo->ConfigTX.CodeType) {
    case CODE_TYPE_DIGITAL:
      gSubMenuSelection = gTxVfo->ConfigTX.Code + 1;
      break;
    case CODE_TYPE_REVERSE_DIGITAL:
      gSubMenuSelection = gTxVfo->ConfigTX.Code + 105;
      break;
    default:
      gSubMenuSelection = 0;
      break;
    }
    break;

  case MENU_T_CTCS:
    if (gTxVfo->ConfigTX.CodeType == CODE_TYPE_CONTINUOUS_TONE) {
      gSubMenuSelection = gTxVfo->ConfigTX.Code + 1;
    } else {
      gSubMenuSelection = 0;
    }
    break;

  case MENU_SFT_D:
    gSubMenuSelection = gTxVfo->FREQUENCY_DEVIATION_SETTING;
    break;

  case MENU_OFFSET:
    gSubMenuSelection = gTxVfo->FREQUENCY_OF_DEVIATION;
    break;

  case MENU_W_N:
    gSubMenuSelection = gTxVfo->CHANNEL_BANDWIDTH;
    break;

  case MENU_SCR:
    gSubMenuSelection = gTxVfo->SCRAMBLING_TYPE;
    break;

  case MENU_BCL:
    gSubMenuSelection = gTxVfo->BUSY_CHANNEL_LOCK;
    break;

  case MENU_MEM_CH:
#ifndef ENABLE_KEEPNAMEONSAVE
    gSubMenuSelection = gEeprom.MrChannel[0];
#else
    gSubMenuSelection = gEeprom.MrChannel[gEeprom.TX_CHANNEL];
#endif
    break;

  case MENU_SAVE:
    gSubMenuSelection = gEeprom.BATTERY_SAVE;
    break;

  case MENU_VOX:
    if (gEeprom.VOX_SWITCH) {
      gSubMenuSelection = gEeprom.VOX_LEVEL + 1;
    } else {
      gSubMenuSelection = 0;
    }
    break;

  case MENU_ABR:
    gSubMenuSelection = gEeprom.BACKLIGHT;
    break;

  case MENU_TDR:
    gSubMenuSelection = gEeprom.DUAL_WATCH;
    break;

  case MENU_WX:
    gSubMenuSelection = gEeprom.CROSS_BAND_RX_TX;
    break;

#ifdef ENABLE_DOCK
		case MENU_REMOTE_UI:
			gSubMenuSelection = gSetting_Remote_UI;
			break;
#endif

  case MENU_BEEP:
    gSubMenuSelection = gEeprom.BEEP_CONTROL;
    break;

  case MENU_TOT:
    gSubMenuSelection = gEeprom.TX_TIMEOUT_TIMER;
    break;
/*
  case MENU_VOICE:
    gSubMenuSelection = gEeprom.VOICE_PROMPT;
    break;
*/
  case MENU_SC_REV:
    gSubMenuSelection = gEeprom.SCAN_RESUME_MODE;
    break;

  case MENU_MDF:
    gSubMenuSelection = gEeprom.CHANNEL_DISPLAY_MODE;
    break;

#ifdef ENABLE_LIVESEEK_MHZ_KEYPAD
	case MENU_LIVESEEK:
	gSubMenuSelection = CEC_LiveSeekMode;
	break;
#endif
			
  case MENU_AUTOLK:
    gSubMenuSelection = gEeprom.AUTO_KEYPAD_LOCK;
    break;

  case MENU_S_ADD1:
    gSubMenuSelection = gTxVfo->SCANLIST1_PARTICIPATION;
    break;

  case MENU_S_ADD2:
    gSubMenuSelection = gTxVfo->SCANLIST2_PARTICIPATION;
    break;

  case MENU_STE:
    gSubMenuSelection = gEeprom.TAIL_NOTE_ELIMINATION;
    break;

  case MENU_RP_STE:
    gSubMenuSelection = gEeprom.REPEATER_TAIL_TONE_ELIMINATION;
    break;

  case MENU_MIC:
    gSubMenuSelection = gEeprom.MIC_SENSITIVITY;
    break;

  case MENU_1_CALL:
    gSubMenuSelection = gEeprom.CHAN_1_CALL;
    break;

  case MENU_S_LIST:
//   gSubMenuSelection = gEeprom.SCAN_LIST_DEFAULT + 1;
    gSubMenuSelection = gEeprom.SCAN_LIST_DEFAULT;

    break;

  case MENU_SLIST1:
    gSubMenuSelection = RADIO_FindNextChannel(0, 1, true, 0);
    break;

  case MENU_SLIST2:
    gSubMenuSelection = RADIO_FindNextChannel(0, 1, true, 1);
    break;
#ifdef ENABLE_DTMF_CALLING
  case MENU_D_ST:
    gSubMenuSelection = gEeprom.DTMF_SIDE_TONE;
    break;

  case MENU_D_RSP:
    gSubMenuSelection = gEeprom.DTMF_DECODE_RESPONSE;
    break;

  case MENU_D_HOLD:
    gSubMenuSelection = gEeprom.DTMF_AUTO_RESET_TIME;
    break;

  case MENU_D_PRE:
    gSubMenuSelection = gEeprom.DTMF_PRELOAD_TIME / 10;
    break;

  case MENU_PTT_ID:
    gSubMenuSelection = gTxVfo->DTMF_PTT_ID_TX_MODE;
    break;

  case MENU_D_DCD:
    gSubMenuSelection = gTxVfo->DTMF_DECODING_ENABLE;
    break;

  case MENU_D_LIST:
    gSubMenuSelection = gDTMFChosenContact + 1;
    break;
#endif
  case MENU_PONMSG:
    gSubMenuSelection = gEeprom.POWER_ON_DISPLAY_MODE;
    break;
#if defined (ENABLE_ROGERBEEP) || defined (ENABLE_MDC)
  case MENU_ROGER:
    gSubMenuSelection = gEeprom.ROGER;
    break;
#endif
#ifdef ENABLE_LCD_CONTRAST_OPTION
  case MENU_CONTRAST:
			gSubMenuSelection = gEeprom.LCD_CONTRAST;
			break;	
#endif
  case MENU_AM:
    gSubMenuSelection = gTxVfo->AM_CHANNEL_MODE;
    break;

  case MENU_DEL_CH:
#ifndef ENABLE_KEEPNAMEONSAVE
    gSubMenuSelection =
        RADIO_FindNextChannel(gEeprom.MrChannel[0], 1, false, 1);
#else
    gSubMenuSelection = RADIO_FindNextChannel(
        gEeprom.MrChannel[gEeprom.TX_CHANNEL], 1, false, 1);
#endif
    break;

  case MENU_350TX:
    gSubMenuSelection = gSetting_350TX;
    break;

  case MENU_F_LOCK:
    gSubMenuSelection = gSetting_F_LOCK;
    break;

  case MENU_200TX:
    gSubMenuSelection = gSetting_200TX;
    break;

  case MENU_500TX:
    gSubMenuSelection = gSetting_500TX;
    break;

  case MENU_ALL_TX:
    gSubMenuSelection = gSetting_ALL_TX;
    break;

  case MENU_SCREN:
    gSubMenuSelection = gSetting_ScrambleEnable;
    break;
#ifdef ENABLE_MESSENGER	
#ifdef ENABLE_MESSENGER_ENCRYPTION
			case MENU_MSG_ENC:
				gSubMenuSelection = gEeprom.MESSENGER_CONFIG.data.encrypt;
				break;
#endif

		
			case MENU_MSG_ACK:
				gSubMenuSelection = gEeprom.MESSENGER_CONFIG.data.ack;
				break;
			case MENU_MSG_MODULATION:
				gSubMenuSelection = gEeprom.MESSENGER_CONFIG.data.modulation;
				break;
			#ifdef ENABLE_MESSENGER_ROGERBEEP_NOTIFICATION
				case MENU_MSG_NOTIFICATION:
				gSubMenuSelection = gEeprom.MESSENGER_CONFIG.data.notification;
				break;
			#endif				
#endif
        
#ifdef ENABLE_STATUS_BATTERY_PERC	
  case MENU_BATTYP:
	gSubMenuSelection = gEeprom.BATTERY_TYPE;
	break;	
#endif	
  }
}

//

static void MENU_Key_DIGITS(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld) {
  uint16_t Value = 0;

  if (bKeyHeld) {
    return;
  }
  if (!bKeyPressed) {
    return;
  }

  gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
  INPUTBOX_Append(Key);
  gRequestDisplayScreen = DISPLAY_MENU;
  if (!gIsInSubMenu) {
    switch (gInputBoxIndex) {
    case 1:
      Value = gInputBox[0];
      if (Value && Value <= gMenuListCount) {
        gMenuCursor = Value - 1;
        gFlagRefreshSetting = true;
        return;
      }
      break;
    case 2:
      gInputBoxIndex = 0;
      Value = (gInputBox[0] * 10) + gInputBox[1];
      if (Value && Value <= gMenuListCount) {
        gMenuCursor = Value - 1;
        gFlagRefreshSetting = true;
        return;
      }
      break;
    }
    gInputBoxIndex = 0;
  } else {
	#ifdef ENABLE_MESSENGER_ENCRYPTION	  
    if (edit_index != -1 && gMenuCursor == MENU_ENC_KEY) {
      if (edit_index < 10)
      {
        if (Key <= KEY_9)
        {
          edit[edit_index] = '0' + Key - KEY_0;

          if (++edit_index >= 10)
          {	// exit edit
            gFlagAcceptSetting  = false;
            gAskForConfirmation = 1;
          }

          gRequestDisplayScreen = DISPLAY_MENU;
        }
      }
      return;
    }
	#endif
    if (gMenuCursor == MENU_OFFSET) {
      uint32_t Frequency;

      if (gInputBoxIndex < 6) {
        return;
      }
      gInputBoxIndex = 0;
      NUMBER_Get(gInputBox, &Frequency);
      Frequency += 75;
      gSubMenuSelection =
          FREQUENCY_FloorToStep(Frequency, gTxVfo->StepFrequency, 0);
      return;
    }
    if (gMenuCursor == MENU_MEM_CH || gMenuCursor == MENU_DEL_CH ||
        gMenuCursor == MENU_1_CALL) {
      if (gInputBoxIndex < 3) {
        gRequestDisplayScreen = DISPLAY_MENU;
        return;
      }
      gInputBoxIndex = 0;
      Value = ((gInputBox[0] * 100) + (gInputBox[1] * 10) + gInputBox[2]) - 1;
      if (IS_MR_CHANNEL(Value)) {
        gSubMenuSelection = Value;
        return;
      }
    } else {
      uint8_t Min, Max;

      if (!MENU_GetLimits(gMenuCursor, &Min, &Max)) {
        uint8_t Offset;

        if (Max < 100) {
          if (Max < 10) {
            Offset = 1;
          } else {
            Offset = 2;
          }
        } else {
          Offset = 3;
        }
        switch (gInputBoxIndex) {
        case 1:
          Value = gInputBox[0];
          break;
        case 2:
          Value = (gInputBox[0] * 10) + gInputBox[1];
          break;
        case 3:
          Value = (gInputBox[0] * 100) + (gInputBox[1] * 10) + gInputBox[2];
          break;
        }
        if (Offset == gInputBoxIndex) {
          gInputBoxIndex = 0;
        }
        if (Value <= Max) {
          gSubMenuSelection = Value;
          return;
        }
      } else {
        gInputBoxIndex = 0;
      }
    }
  }
  gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
}

static void MENU_Key_EXIT(bool bKeyPressed, bool bKeyHeld) {
  if (!bKeyHeld && bKeyPressed) {
    gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
    if (gCssScanMode == CSS_SCAN_MODE_OFF) {
      if (gIsInSubMenu) {
        if (gInputBoxIndex == 0 || gMenuCursor != MENU_OFFSET) {
          gIsInSubMenu = false;
          gInputBoxIndex = 0;
          gFlagRefreshSetting = true;
          gAskForConfirmation = 0;
        } else {
          gInputBoxIndex--;
          gInputBox[gInputBoxIndex] = 10;
        }
        gRequestDisplayScreen = DISPLAY_MENU;
        return;
      }
      gRequestDisplayScreen = DISPLAY_MAIN;
    } else {
      MENU_StopCssScan();
      gRequestDisplayScreen = DISPLAY_MENU;
    }
    gPttWasReleased = true;
  }
}

static void MENU_Key_MENU(bool bKeyPressed, bool bKeyHeld) {
  if (!bKeyHeld && bKeyPressed) {
    gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
    gRequestDisplayScreen = DISPLAY_MENU;
    if (!gIsInSubMenu) {
      gAskForConfirmation = 0;
      gIsInSubMenu = true;
      edit_index     = -1;

    } else {

      #ifdef ENABLE_MESSENGER_ENCRYPTION
        if (gMenuCursor == MENU_ENC_KEY)
        {
          if (edit_index < 0)
          {	// enter encryption key edit mode
            // pad the encryption key out with '_'
            edit_index = strlen(edit);
            while (edit_index < 10)
              edit[edit_index++] = '_';
            edit[edit_index] = 0;
            edit_index = 0;  // 'edit_index' is going to be used as the cursor position

            return;
          }
          else if (edit_index >= 0 && edit_index < 10)
          {	// editing the encryption key characters

            if (++edit_index < 10)
              return;	// next char

            // exit, save encryption key
          }
        }
      #endif


      if (gMenuCursor == MENU_RESET || gMenuCursor == MENU_MEM_CH ||
          gMenuCursor == MENU_DEL_CH
#ifdef ENABLE_MESSENGER_ENCRYPTION
          || gMenuCursor == MENU_ENC_KEY
#endif
          ) {
        switch (gAskForConfirmation) {
        case 0:
          gAskForConfirmation = 1;
          break;
        case 1:
          gAskForConfirmation = 2;
          UI_DisplayMenu();
          if (gMenuCursor == MENU_RESET) {
            MENU_AcceptSetting();
#if defined(ENABLE_OVERLAY)
            overlay_FLASH_RebootToBootloader();
#else
            NVIC_SystemReset();
#endif
          }
          gFlagAcceptSetting = true;
          gIsInSubMenu = false;
          gAskForConfirmation = 0;
        }
      } else {
        gFlagAcceptSetting = true;
        gIsInSubMenu = false;
      }
      gCssScanMode = CSS_SCAN_MODE_OFF;
    }
    gInputBoxIndex = 0;
  }
}

static void MENU_Key_STAR(bool bKeyPressed, bool bKeyHeld) {
  if (!bKeyHeld && bKeyPressed) {
    gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;

    RADIO_SelectVfos();
    if (IS_NOT_NOAA_CHANNEL(gRxVfo->CHANNEL_SAVE) && !gRxVfo->ModulationType) {
      if (gMenuCursor == MENU_R_CTCS || gMenuCursor == MENU_R_DCS) {
        if (gCssScanMode == CSS_SCAN_MODE_OFF) {
          MENU_StartCssScan(1);
          gRequestDisplayScreen = DISPLAY_MENU;
        } else {
          MENU_StopCssScan();
          gRequestDisplayScreen = DISPLAY_MENU;
        }
      }
      gPttWasReleased = true;
      return;
    }
    gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
  }
}

static void MENU_Key_UP_DOWN(bool bKeyPressed, bool bKeyHeld,
                             int8_t Direction) {
  uint8_t VFO;
  uint8_t Channel;
  bool bCheckScanList;

  #ifdef ENABLE_MESSENGER_ENCRYPTION

  if (gIsInSubMenu && edit_index >= 0 && gMenuCursor == MENU_ENC_KEY )
	{	// change the character
		if (bKeyPressed && edit_index < 10 && Direction != 0)
		{
			// TODO: Allow special chars when setting encryption key
			const char   unwanted[] = "$%&!\"':;?^`|{}";
			char         c          = edit[edit_index] + Direction;
			unsigned int i          = 0;
			while (i < sizeof(unwanted) && c >= 32 && c <= 126)
			{
				if (c == unwanted[i++])
				{	// choose next character
					c += Direction;
					i = 0;
				}
			}
			edit[edit_index] = (c < 32) ? 126 : (c > 126) ? 32 : c;

			gRequestDisplayScreen = DISPLAY_MENU;
		}
		return;
	}

  #endif

  if (!bKeyHeld) {
    if (!bKeyPressed) {
      return;
    }
    gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
    gInputBoxIndex = 0;
  } else if (!bKeyPressed) {
    return;
  }

  if (gCssScanMode != CSS_SCAN_MODE_OFF) {
    MENU_StartCssScan(Direction);
    gPttWasReleased = true;
    gRequestDisplayScreen = DISPLAY_MENU;
    return;
  }

  if (!gIsInSubMenu) {
    gMenuCursor = NUMBER_AddWithWraparound(gMenuCursor, -Direction, 0,
                                           gMenuListCount - 1);
    gFlagRefreshSetting = true;
    gRequestDisplayScreen = DISPLAY_MENU;
    return;
  }

  if (gMenuCursor == MENU_OFFSET) {
    int32_t Offset;

    Offset = (Direction * gTxVfo->StepFrequency) + gSubMenuSelection;
    if (Offset < 99999990) {
      if (Offset < 0) {
        Offset = 99999990;
      }
    } else {
      Offset = 0;
    }
    gSubMenuSelection = FREQUENCY_FloorToStep(Offset, gTxVfo->StepFrequency, 0);
    gRequestDisplayScreen = DISPLAY_MENU;
    return;
  }

  VFO = 0;

  switch (gMenuCursor) {
  case MENU_DEL_CH:
  case MENU_1_CALL:
    bCheckScanList = false;
    break;
  case MENU_SLIST2:
    VFO = 1;
    // Fallthrough
  case MENU_SLIST1:
    bCheckScanList = true;
    break;
  default:
    MENU_ClampSelection(Direction);
    gRequestDisplayScreen = DISPLAY_MENU;
    return;
  }

  Channel = RADIO_FindNextChannel(gSubMenuSelection + Direction, Direction,
                                  bCheckScanList, VFO);
  if (Channel != 0xFF) {
    gSubMenuSelection = Channel;
  }
  gRequestDisplayScreen = DISPLAY_MENU;
}

void MENU_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld) {
  switch (Key) {
  case KEY_0:
  case KEY_1:
  case KEY_2:
  case KEY_3:
  case KEY_4:
  case KEY_5:
  case KEY_6:
  case KEY_7:
  case KEY_8:
  case KEY_9:
    MENU_Key_DIGITS(Key, bKeyPressed, bKeyHeld);
    break;
  case KEY_MENU:
    MENU_Key_MENU(bKeyPressed, bKeyHeld);
    break;
  case KEY_UP:
    MENU_Key_UP_DOWN(bKeyPressed, bKeyHeld, 1);
    break;
  case KEY_DOWN:
    MENU_Key_UP_DOWN(bKeyPressed, bKeyHeld, -1);
    break;
  case KEY_EXIT:
    MENU_Key_EXIT(bKeyPressed, bKeyHeld);
    break;
  case KEY_STAR:
    MENU_Key_STAR(bKeyPressed, bKeyHeld);
    break;
  case KEY_F:
#ifdef ENABLE_MESSENGER_ENCRYPTION  
    if (edit_index >= 0 && gMenuCursor == MENU_ENC_KEY ) {
      if (!bKeyHeld && bKeyPressed)
      {
        gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
        if (edit_index >= 0)
        {
          edit_index--;
          gRequestDisplayScreen = DISPLAY_MENU;
        }
      }
      break;
    }
#endif    
    GENERIC_Key_F(bKeyPressed, bKeyHeld);
    break;
  case KEY_PTT:
    GENERIC_Key_PTT(bKeyPressed);
    break;
  default:
    if (!bKeyHeld && bKeyPressed) {
      gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
    }
    break;
  }
  if (gScreenToDisplay == DISPLAY_MENU && gMenuCursor == MENU_VOL) {
    gVoltageMenuCountdown = 0x20;
  }
}
