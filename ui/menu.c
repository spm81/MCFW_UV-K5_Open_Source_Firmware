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

#include "menu.h"
#ifdef ENABLE_DTMF_CALLING
#include "../app/dtmf.h"
#endif
#include "../bitmaps.h"
#include "../dcs.h"
#include "../driver/st7565.h"
#include "../external/printf/printf.h"
#include "../frequencies.h"
#include "../helper/battery.h"
#include "../helper/measurements.h"
#include "../misc.h"
#ifdef ENABLE_LCD_CONTRAST_OPTION
#include "driver/st7565.h"
#endif
#include "../settings.h"
#include "helper.h"
#include "inputbox.h"
#include "ui.h"
#include <string.h>
#ifdef ENABLE_DOCK
	#include "app/uart.h"
#endif

#ifdef ENABLE_MESSENGER_ENCRYPTION
	#include "helper/crypto.h"
#endif

//char               str[64];  // bigger cuz we can now do multi-line in one string (use '\n' char)

static const char MenuList[][8] = {
        // 0x00
    "Squelch",
    "Step",
    "TxPower",
    "RxDCS",
    "RxCTCS",
    "TxDCS",
    "TxCTCS",
    "TxODir",
    // 0x08
    "TxOffs",
    "Bandwid",
#ifdef ENABLE_LIVESEEK_MHZ_KEYPAD
	"LIVE.S",
#endif	
    "Scrambl",
    "BusyCL",
    "ChSave",
    "BatSave",
    "VOX",
    "Backlit",
/*	
    #ifdef ENABLE_LCD_INVERT_OPTION
    "Invert",
    #endif*/
#ifdef ENABLE_LCD_CONTRAST_OPTION
    "Contras",
#endif
    // 0x10
    "DualRx",
    "XBand",
#ifdef ENABLE_MESSENGER_ENCRYPTION
    "EncKey",
	"MsgEnc",
#endif
#ifdef ENABLE_MESSENGER
	"MsgAck",
	"MsgMod",
	#ifdef ENABLE_MESSENGER_ROGERBEEP_NOTIFICATION
	"MsgRBN",
	#endif	
#endif    
    "Beep",
    "TxTime",
   // "Voice",
    "ScnRev",
    "ChDisp",
    "KeyLock",
    // 0x18
    "ScAdd1",
    "ScAdd2",
    "STE",
    "RP STE",
    "Mic",
    "1 Call",
    "SList",
    "SList1",
    // 0x20
    "SList2",
#ifdef ENABLE_DTMF_CALLING
    "ANI ID",
    "UPCode",
    "DWCode",
    "D ST",
    "D Resp",
    "D Hold",
    // 0x28
    "D Prel",
    "PTT ID",
    "D Decd",
    "D List",    
#endif
    "PonMsg",
#if defined(ENABLE_ROGER_DEFAULT) || defined(ENABLE_ROGER_MOTOTRBO) || defined(ENABLE_ROGER_TPT) || defined(ENABLE_ROGER_MOTOTRBOT40) || defined(ENABLE_ROGER_MOTOTRBOTLKRT80) || defined(ENABLE_ROGER_ROGERCOBRAAM845) || defined(ENABLE_ROGER_POLICE_ITA) || defined(ENABLE_ROGER_UV5RC) || defined(ENABLE_ROGER_MARIO) || defined(ENABLE_MDC)
    "Roger",
#endif	
    "Voltage",
#ifdef ENABLE_STATUS_BATTERY_PERC	
    "BatTyp", // battery type 1600/2200mAh
#endif
    "Modulat",
// 0x30
    "ChDele",
    "Reset",
    "Upconv",
#ifdef ENABLE_DOCK
    "Remote",
#endif    
    "Tx 350",
    "F Lock",
    "Tx 200",
    "Tx 500",
    "Tx All",
    // 0x38
    "ScramEn",
};

#ifdef ENABLE_LIVESEEK_MHZ_KEYPAD
const char gSubMenu_LIVESEEK[3][6] =
{
	"NONE",
	"LIVE",
	"LIVE+"
};
#endif
#ifdef ENABLE_STATUS_BATTERY_PERC	
const char gSubMenu_BATTYP[][9] =
{
	"1600mAh",
	"2200mAh"
};
#endif
static const char gSubMenu_TXP[3][5] = {
    "LOW",
    "MID",
    "HIGH",
};

static const char gSubMenu_SFT_D[3][4] = {
    "OFF",
    "+",
    "-",
};

static const char gSubMenu_SAVE[5][4] = {
    "OFF", "1:1", "1:2", "1:3", "1:4",
};

static const char gSubMenu_CHAN[3][7] = {
    "OFF",
    "CHAN_A",
    "CHAN_B",
};
/*
static const char gSubMenu_VOICE[3][4] = {
    "OFF",
    "CHI",
    "ENG",
};
*/
static const char gSubMenu_SC_REV[3][3] = {
    "TO",
    "CO",
    "SE",
};

static const char gSubMenu_MDF[4][7] = {"FREQ", "CHAN", "NAME", "NAME+F"};

#ifdef ENABLE_DTMF_CALLING
static const char gSubMenu_D_RSP[4][6] = {
    "NULL",
    "RING",
    "REPLY",
    "BOTH",
};

static const char gSubMenu_PTT_ID[4][5] = {
    "OFF",
    "BOT",
    "EOT",
    "BOTH",
};
#endif
static const char gSubMenu_PONMSG[3][5] = {
    "FULL",
    "MSG",
    "VOL",
};

#if defined(ENABLE_ROGER_DEFAULT) || defined(ENABLE_ROGER_MOTOTRBO) || defined(ENABLE_ROGER_TPT) || defined(ENABLE_ROGER_MOTOTRBOT40) || defined(ENABLE_ROGER_MOTOTRBOTLKRT80) || defined(ENABLE_ROGER_ROGERCOBRAAM845) || defined(ENABLE_ROGER_POLICE_ITA) || defined(ENABLE_ROGER_UV5RC) || defined(ENABLE_ROGER_MARIO) || defined(ENABLE_MDC)
const char gSubMenu_ROGER[][9] = {
  "OFF",


#ifdef ENABLE_ROGER_DEFAULT
    "DEFAULT",
#endif

#ifdef ENABLE_ROGER_MOTOTRBO
    "MOTOTRBO",
#endif

#ifdef ENABLE_ROGER_TPT
    "MOTO TPT",
#endif

#ifdef ENABLE_ROGER_MOTOTRBOT40
    "MOTO T40",
#endif

#ifdef ENABLE_ROGER_MOTOTRBOTLKRT80
    "MOTO T80",
#endif

#ifdef ENABLE_ROGER_ROGERCOBRAAM845
    "C.AM845",
#endif

#ifdef ENABLE_ROGER_POLICE_ITA
    "POLIZIA",
#endif

#ifdef ENABLE_ROGER_UV5RC
    "B.UV-5RC",
#endif

#ifdef ENABLE_ROGER_MARIO
    "MARIO",
#endif

#ifdef ENABLE_MDC
    "MDC",
#endif

};
#endif

#ifdef ENABLE_MESSENGER
const char gSubMenu_MSG_MODULATION[3][10] = {
    "AFSK 1.2K",
    "FSK 700",
    "FSK 450",
};
#endif

static const char gSubMenu_RESET[2][4] = {
    "VFO",
    "ALL",
};

static const char gSubMenu_F_LOCK[5][8] = {
    "OFF", "FCC", "CE", "GB", "LPD PMR",
};
const char gSubMenuBacklight[8][7] = {"OFF",   "5 sec", "10 sec", "20 sec",
                                      "1 min", "2 min", "ON"};

static const char *defaultEnableDisable[3] = {"DEFAULT", "ENABLE", "DISABLE"};
static const char *offOn[2] = {"OFF", "ON"};
static const char *upconverterFreqNames[7] = {"OFF", "50M", "70M", "106M", "125M", "140M", "200M"};
bool gIsInSubMenu;

uint8_t gMenuCursor;
int8_t gMenuScrollDirection;
uint32_t gSubMenuSelection;

// edit box
char    edit[17];
int     edit_index;

void UI_DisplayMenu(void) {
  char String[16];
#ifdef ENABLE_DTMF_CALLING
  char Contact[16];
#endif
  uint8_t i;

  UI_DisplayClear();

  uint8_t offset = Clamp(gMenuCursor - 2, 0, gMenuListCount - 6);
  for (int i = 0; i < 6; ++i) {
    const char *s = MenuList[i + offset];
    bool isCurrent = gMenuCursor == i + offset;
    if (isCurrent) {
      UI_PrintStringSmallBold(s, 0, 0, i);
    } else {
      UI_PrintStringSmall(s, 0, 0, i);
    }
  }

  uint8_t menuScrollbarPosY =
      ConvertDomain(gMenuCursor, 0, gMenuListCount, 0, 7 * 8 - 3);

  for (i = 0; i < 7; i++) {
    gFrameBuffer[i][49] = 0xFF;
  }

  for (uint8_t a = 0; a < 3; a++) {
    PutPixel(48, menuScrollbarPosY + a, true);
    PutPixel(50, menuScrollbarPosY + a, true);
  }

  sprintf(String, "%03u", gMenuCursor + 1);
  UI_PrintStringSmallest(String, 36, 49, false, true);
  if (gIsInSubMenu) {
    memcpy(gFrameBuffer[0] + 50, BITMAP_CurrentIndicator,
           sizeof(BITMAP_CurrentIndicator));
  }

  memset(String, 0, sizeof(String));
  bool already_printed = false;

  switch (gMenuCursor) {
  case MENU_SQL:
  case MENU_MIC:
    sprintf(String, "%d", gSubMenuSelection);
    break;

  case MENU_STEP:
    sprintf(String, "%d.%02dKHz", StepFrequencyTable[gSubMenuSelection] / 100,
            StepFrequencyTable[gSubMenuSelection] % 100);
    break;

  case MENU_TXP:
    strcpy(String, gSubMenu_TXP[gSubMenuSelection]);
    break;

  case MENU_R_DCS:
  case MENU_T_DCS:
    if (gSubMenuSelection == 0) {
      strcpy(String, "OFF");
    } else if (gSubMenuSelection < 105) {
      sprintf(String, "D%03oN", DCS_Options[gSubMenuSelection - 1]);
    } else {
      sprintf(String, "D%03oI", DCS_Options[gSubMenuSelection - 105]);
    }
    break;

  case MENU_R_CTCS:
  case MENU_T_CTCS:
    if (gSubMenuSelection == 0) {
      strcpy(String, "OFF");
    } else {
      sprintf(String, "%d.%dHz", CTCSS_Options[gSubMenuSelection - 1] / 10,
              CTCSS_Options[gSubMenuSelection - 1] % 10);
    }
    break;

  case MENU_SFT_D:
    strcpy(String, gSubMenu_SFT_D[gSubMenuSelection]);
    break;

  case MENU_OFFSET:
    if (!gIsInSubMenu || gInputBoxIndex == 0) {
      sprintf(String, "%d.%05d", gSubMenuSelection / 100000,
              gSubMenuSelection % 100000);
      break;
    }
    for (i = 0; i < 3; i++) {
      if (gInputBox[i] == 10) {
        String[i] = '-';
      } else {
        String[i] = gInputBox[i] + '0';
      }
    }
    String[3] = '.';
    for (i = 3; i < 6; i++) {
      if (gInputBox[i] == 10) {
        String[i + 1] = '-';
      } else {
        String[i + 1] = gInputBox[i] + '0';
      }
    }
    String[7] = '-';
    String[8] = '-';
    String[9] = 0;
    String[10] = 0;
    String[11] = 0;
    break;

  case MENU_W_N:
    strcpy(String, bwNames[gSubMenuSelection]);
    break;

#ifdef ENABLE_LIVESEEK_MHZ_KEYPAD
	case MENU_LIVESEEK:
	strcpy(String, gSubMenu_LIVESEEK[gSubMenuSelection]);
	break;
#endif
	
  case MENU_SCR:
  case MENU_VOX:
    if (gSubMenuSelection == 0) {
      strcpy(String, "OFF");
    } else {
      sprintf(String, "%d", gSubMenuSelection);
    }
    break;

  case MENU_ABR:
    strcpy(String, gSubMenuBacklight[gSubMenuSelection]);
    break;
#ifdef ENABLE_LCD_CONTRAST_OPTION
  case MENU_CONTRAST:				
		sprintf(String, "%d", gSubMenuSelection);				
		ST7565_SetContrast(gSubMenuSelection);
	  break;
#endif
  
  case MENU_ALL_TX:
    strcpy(String, defaultEnableDisable[gSubMenuSelection]);
    break;

#ifdef ENABLE_DOCK
  case MENU_REMOTE_UI:
#endif
#ifdef ENABLE_MESSENGER_ENCRYPTION
  case MENU_MSG_ENC:
#endif
#ifdef ENABLE_MESSENGER
	#ifdef ENABLE_MESSENGER_ROGERBEEP_NOTIFICATION
		case MENU_MSG_NOTIFICATION:
	#endif
  case MENU_MSG_ACK:	
#endif
  case MENU_BCL:
  case MENU_BEEP:
  case MENU_AUTOLK:
  case MENU_S_ADD1:
  case MENU_S_ADD2:
#ifdef ENABLE_DTMF_CALLING
  case MENU_D_ST:
  case MENU_D_DCD:
#endif  
  case MENU_350TX:
  case MENU_200TX:
  case MENU_500TX:
  case MENU_SCREN:
  case MENU_STE:
    strcpy(String, offOn[gSubMenuSelection]);
    break;
  case MENU_AM:
    strcpy(String, modulationTypeOptions[gSubMenuSelection]);
    break;

  case MENU_MEM_CH:
  case MENU_1_CALL:
  case MENU_DEL_CH:
    UI_GenerateChannelStringEx(
        String, RADIO_CheckValidChannel((uint16_t)gSubMenuSelection, false, 0),
        (uint8_t)gSubMenuSelection);
    break;

  case MENU_SAVE:
    strcpy(String, gSubMenu_SAVE[gSubMenuSelection]);
    break;

  case MENU_TDR:
  case MENU_WX:
    strcpy(String, gSubMenu_CHAN[gSubMenuSelection]);
    break;

  case MENU_TOT:
    if (gSubMenuSelection == 0) {
      strcpy(String, "OFF");
    } else {
      sprintf(String, "%dmin", gSubMenuSelection);
    }
    break;
/*
  case MENU_VOICE:
    strcpy(String, gSubMenu_VOICE[gSubMenuSelection]);
    break;
*/
  case MENU_SC_REV:
    strcpy(String, gSubMenu_SC_REV[gSubMenuSelection]);
    break;

  case MENU_MDF:
    strcpy(String, gSubMenu_MDF[gSubMenuSelection]);
    break;

  case MENU_RP_STE:
    if (gSubMenuSelection == 0) {
      strcpy(String, "OFF");
    } else {
      sprintf(String, "%d*100ms", gSubMenuSelection);
    }
    break;

 /* case MENU_S_LIST:
    sprintf(String, "LIST%d", gSubMenuSelection);
    break;
*/
case MENU_S_LIST:
	//		strcpy(str, "SCAN LIST\n");
			if (gSubMenuSelection < 2)
				sprintf(String, "LIST%d", 1 + gSubMenuSelection);
			//	sprintf(String, "LIST%u", gSubMenuSelection);
			else
				sprintf(String, "ALL");
			break;
#ifdef ENABLE_DTMF_CALLING
			
  case MENU_ANI_ID:
    strcpy(String, gEeprom.ANI_DTMF_ID);
    break;

  case MENU_UPCODE:
    strcpy(String, gEeprom.DTMF_UP_CODE);
    break;

  case MENU_DWCODE:
    strcpy(String, gEeprom.DTMF_DOWN_CODE);
    break;

  case MENU_D_RSP:
    strcpy(String, gSubMenu_D_RSP[gSubMenuSelection]);
    break;

  case MENU_D_HOLD:
    sprintf(String, "%ds", gSubMenuSelection);
    break;

  case MENU_D_PRE:
    sprintf(String, "%d*10ms", gSubMenuSelection);
    break;

  case MENU_PTT_ID:
    strcpy(String, gSubMenu_PTT_ID[gSubMenuSelection]);
    break;

  case MENU_D_LIST:
    gIsDtmfContactValid =
        DTMF_GetContact((uint8_t)gSubMenuSelection - 1, Contact);
    if (!gIsDtmfContactValid) {
      // Ghidra being weird again...
      memcpy(String, "NULL\0\0\0", 8);
    } else {
      memcpy(String, Contact, 8);
    }
    break;
#endif
  case MENU_PONMSG:
    strcpy(String, gSubMenu_PONMSG[gSubMenuSelection]);
    break;
#if defined(ENABLE_ROGER_DEFAULT) || defined(ENABLE_ROGER_MOTOTRBO) || defined(ENABLE_ROGER_TPT) || defined(ENABLE_ROGER_MOTOTRBOT40) || defined(ENABLE_ROGER_MOTOTRBOTLKRT80) || defined(ENABLE_ROGER_ROGERCOBRAAM845) || defined(ENABLE_ROGER_POLICE_ITA) || defined(ENABLE_ROGER_UV5RC) || defined(ENABLE_ROGER_MARIO) || defined(ENABLE_MDC)
  case MENU_ROGER:
    strcpy(String, gSubMenu_ROGER[gSubMenuSelection]);
    break;
#endif
  case MENU_VOL:
#ifdef ENABLE_BATTERY_CHARGING  
        // Draw the charging indicator over the battery if we're charging
		// The charging indicator is 13x16, so we need 2 lines of 8 pixels to draw it.
		//By Tunas1337
		if (gChargingWithTypeC) {
			memcpy(gFrameBuffer[2]+113, BITMAP_SettingsBattCharging, 13);
			memcpy(gFrameBuffer[3]+113, BITMAP_SettingsBattCharging+13, 13);
		}
#endif		
    sprintf(String, "%d.%02dV", gBatteryVoltageAverage / 100,
            gBatteryVoltageAverage % 100);
    break;

  case MENU_RESET:
    strcpy(String, gSubMenu_RESET[gSubMenuSelection]);
    break;
#ifdef ENABLE_STATUS_BATTERY_PERC	
  case MENU_BATTYP:
		strcpy(String, gSubMenu_BATTYP[gSubMenuSelection]);
		break;	
#endif
  case MENU_UPCONVERTER:
    strcpy(String, upconverterFreqNames[gSubMenuSelection]);
    break;
	
  case MENU_F_LOCK:
    strcpy(String, gSubMenu_F_LOCK[gSubMenuSelection]);
    break;

#ifdef ENABLE_MESSENGER_ENCRYPTION
				case MENU_ENC_KEY:
				{
          //const unsigned int menu_item_x1    =  50;
          //const unsigned int menu_item_x2    = LCD_WIDTH - 1;
					if (!gIsInSubMenu)
					{	// show placeholder in main menu
						strcpy(String, "****");
						//UI_PrintString(String, menu_item_x1, menu_item_x2, 2, 8, true);
					}
					else
					{	// show the key being edited
						/*if (edit_index != -1 || gAskForConfirmation) {
							UI_PrintString(edit, (menu_item_x1 -2), 0, 2, 8, true);
							// show the cursor
							if(edit_index < 10)
								UI_PrintString(     "^", (menu_item_x1 -2) + (8 * edit_index), 0, 4, 8, true);  
						}
						else{*/            

            if (edit_index != -1 || gAskForConfirmation) {
              strcpy(String, edit); 
              UI_PrintStringSmall(String, 55, 0, 2);
              if(edit_index < 10)
							  UI_PrintStringSmall("^", 55 + (7 * edit_index), 0, 3);
              already_printed = true;
            } else {							

              UI_PrintStringSmall("hashed", 55, 127, 1);
							memset(String, 0, sizeof(String));
							
							CRYPTO_DisplayHash(gEeprom.ENC_KEY, String, sizeof(gEeprom.ENC_KEY));
							UI_PrintStringSmall(String, 55, 127, 3);
              already_printed = true;
						}			
					}
					
					break;
				}
			#endif

			#ifdef ENABLE_MESSENGER
				case MENU_MSG_MODULATION:
          //if not in range, reset it to default
          if(!(gSubMenuSelection>=0 && gSubMenuSelection<ARRAY_SIZE(gSubMenu_MSG_MODULATION))){
            gSubMenuSelection=0;
          }
					strcpy(String, gSubMenu_MSG_MODULATION[gSubMenuSelection]);
          //sprintf(String,"%x",gSubMenuSelection);
					break;
			#endif
  }

  if (!already_printed)
    UI_PrintString(String, 50, 127, 2, 8, true);

  if (gMenuCursor == MENU_OFFSET) {
    UI_PrintString("MHz", 50, 127, 4, 8, true);
  }

  if ((gMenuCursor == MENU_RESET || gMenuCursor == MENU_MEM_CH ||
       gMenuCursor == MENU_DEL_CH
        #ifdef ENABLE_MESSENGER_ENCRYPTION
          || gMenuCursor == MENU_ENC_KEY
        #endif
       ) &&
      gAskForConfirmation) {
    if (gAskForConfirmation == 1) {
      strcpy(String, "SURE?");
    } else {
      strcpy(String, "WAIT!");
    }
    UI_PrintString(String, 50, 127, 4, 8, true);
  }

  if ((gMenuCursor == MENU_R_CTCS || gMenuCursor == MENU_R_DCS) &&
      gCssScanMode != CSS_SCAN_MODE_OFF) {
    UI_PrintString("SCAN", 50, 127, 4, 8, true);
  }
#ifdef ENABLE_DTMF_CALLING

  if (gMenuCursor == MENU_UPCODE) {
    if (strlen(gEeprom.DTMF_UP_CODE) > 8) {
      UI_PrintString(gEeprom.DTMF_UP_CODE + 8, 50, 127, 4, 8, true);
    }
  }
  if (gMenuCursor == MENU_DWCODE) {
    if (strlen(gEeprom.DTMF_DOWN_CODE) > 8) {
      UI_PrintString(gEeprom.DTMF_DOWN_CODE + 8, 50, 127, 4, 8, true);
    }
  }
  if (gMenuCursor == MENU_D_LIST && gIsDtmfContactValid) {
    Contact[11] = 0;
    memcpy(&gDTMF_ID, Contact + 8, 4);
    sprintf(String, "ID:%s", Contact + 8);
    UI_PrintString(String, 50, 127, 4, 8, true);
  }
#endif
  if (gMenuCursor == MENU_R_CTCS || gMenuCursor == MENU_T_CTCS ||
      gMenuCursor == MENU_R_DCS || gMenuCursor == MENU_T_DCS
#ifdef ENABLE_DTMF_CALLING
	  || gMenuCursor == MENU_D_LIST) {
#else
) {
#endif	
		  /*
    uint8_t Offset;

    NUMBER_ToDigits((uint8_t)gSubMenuSelection, String);
    Offset = (gMenuCursor == MENU_D_LIST) ? 2 : 3;
    UI_DisplaySmallDigits(Offset, String + (8 - Offset), 105, 0);
  }

  if (gMenuCursor == MENU_SLIST1 || gMenuCursor == MENU_SLIST2) {
    i = gMenuCursor - MENU_SLIST1;
		i = (gMenuCursor == MENU_SLIST1) ? 0 : 1;

    if (gSubMenuSelection == 0xFF)
		if (gSubMenuSelection < 0)
		{
      sprintf(String, "NULL");
    } else {
      UI_GenerateChannelStringEx(String, true, (uint8_t)gSubMenuSelection);
    }

    if (gSubMenuSelection == 0xFF || !gEeprom.SCAN_LIST_ENABLED[i]) {
    if (gSubMenuSelection < 0 || !gEeprom.SCAN_LIST_ENABLED[i]) {
      UI_PrintString(String, 50, 127, 2, 8, true);
    } else {
      UI_PrintString(String, 50, 127, 0, 8, true);
      if (IS_MR_CHANNEL(gEeprom.SCANLIST_PRIORITY_CH1[i])) {
        sprintf(String, "PRI1:%d", gEeprom.SCANLIST_PRIORITY_CH1[i] + 1);
        UI_PrintString(String, 50, 127, 2, 8, true);
      }
      if (IS_MR_CHANNEL(gEeprom.SCANLIST_PRIORITY_CH2[i])) {
        sprintf(String, "PRI2:%d", gEeprom.SCANLIST_PRIORITY_CH2[i] + 1);
        UI_PrintString(String, 50, 127, 4, 8, true);
      }
    }
  }

  ST7565_BlitFullScreen();
}
*/
#ifdef ENABLE_DTMF_CALLING

 uint8_t Offset;

    NUMBER_ToDigits((uint8_t)gSubMenuSelection, String);
    Offset = (gMenuCursor == MENU_D_LIST) ? 2 : 3;
    UI_DisplaySmallDigits(Offset, String + (9 - Offset), 105, 0);
#endif	
  }
#ifdef ENABLE_BATTERY_CHARGING
	// If we're in the voltage menu option, also print the current
	if (gMenuCursor == MENU_VOL) {
		sprintf(String, "%dmA", gBatteryCurrent);
		UI_PrintString(String, 50, 127, 5, 8, true);
	}
#endif	
  if (gMenuCursor == MENU_SLIST1 || gMenuCursor == MENU_SLIST2) {
    i = gMenuCursor - MENU_SLIST1;

    if (gSubMenuSelection == 0xFF) {
      sprintf(String, "NULL");
    } else {
      UI_GenerateChannelStringEx(String, true, (uint8_t)gSubMenuSelection);
    }

    if (gSubMenuSelection == 0xFF || !gEeprom.SCAN_LIST_ENABLED[i]) {
      UI_PrintString(String, 50, 127, 2, 8, true);
    } else {
      UI_PrintString(String, 50, 127, 0, 8, true);
      if (IS_MR_CHANNEL(gEeprom.SCANLIST_PRIORITY_CH1[i])) {
        sprintf(String, "PRI1:%d", gEeprom.SCANLIST_PRIORITY_CH1[i] + 1);
        UI_PrintString(String, 50, 127, 2, 8, true);
      }
      if (IS_MR_CHANNEL(gEeprom.SCANLIST_PRIORITY_CH2[i])) {
        sprintf(String, "PRI2:%d", gEeprom.SCANLIST_PRIORITY_CH2[i] + 1);
        UI_PrintString(String, 50, 127, 4, 8, true);
      }
    }
  }

  ST7565_BlitFullScreen();
}