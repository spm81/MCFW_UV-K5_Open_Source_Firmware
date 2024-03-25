#include "appmenu.h"
#include "../ui/welcome.h"
#ifdef ENABLE_MESSENGER
	#include "../app/messenger.h"
  #include "frequencies.h"
  #include "external/printf/printf.h"
  #include "settings.h"
#endif
void APPMENU_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld) {
  switch (Key) {
  case KEY_EXIT:
    gRequestDisplayScreen = DISPLAY_MAIN;
    break;

  case KEY_MENU:
    gRequestDisplayScreen = DISPLAY_MENU;
	  break;

  case KEY_0:
	  UI_DisplayWelcome();
    //gRequestDisplayScreen = DISPLAY_MAIN;
	  break;
#ifdef ENABLE_SCANLIST
  case KEY_1:
    gAppToDisplay = APP_SCANLIST;
    gRequestDisplayScreen = DISPLAY_MAIN;
    break;
#endif	
#if defined (ENABLE_MESSENGER) && defined (ENABLE_SCANLIST)
#if defined (ENABLE_MESSENGER_SHOW_RX_FREQ) && !defined (ENABLE_MESSENGER_SHOW_RX_TX_FREQ)
  case KEY_2:
    hasNewMessage = false;
    uint32_t frequency = gEeprom.VfoInfo[gEeprom.TX_CHANNEL].pTX->Frequency;
    if ( IsTXAllowed(gEeprom.VfoInfo[gEeprom.TX_CHANNEL].pTX->Frequency) ) {
      frequency = GetScreenF(frequency);
      sprintf(msgFreqInfo, "%u.%05u Mhz", frequency / 100000, frequency % 100000);
    } else {
      sprintf(msgFreqInfo, "TX DISABLE");
    }
    gUpdateStatus = true;
    gAppToDisplay = APP_MESSENGER;
	  gRequestDisplayScreen = DISPLAY_MAIN;
    break;
#elif defined (ENABLE_MESSENGER_SHOW_RX_TX_FREQ) && !defined (ENABLE_MESSENGER_SHOW_RX_FREQ) || defined (ENABLE_MESSENGER_SHOW_RX_TX_FREQ) && defined (ENABLE_MESSENGER_SHOW_RX_FREQ)
  case KEY_2:
    hasNewMessage = false;
    uint32_t txFrequency = gEeprom.VfoInfo[gEeprom.TX_CHANNEL].pTX->Frequency;
    uint32_t rxFrequency = gEeprom.VfoInfo[gEeprom.RX_CHANNEL].pRX->Frequency;
    
    if (IsTXAllowed(txFrequency)) {
      txFrequency = GetScreenF(txFrequency);
      rxFrequency = GetScreenF(rxFrequency);
      sprintf(msgFreqInfo, "R:%u.%05u T:%u.%05u Mhz", rxFrequency / 100000, rxFrequency % 100000, txFrequency / 100000, txFrequency % 100000);
    } else {
      sprintf(msgFreqInfo, "TX DISABLE");
    }

    gUpdateStatus = true;
    gAppToDisplay = APP_MESSENGER;
    gRequestDisplayScreen = DISPLAY_MAIN;
    break;
#else
  case KEY_2:
    hasNewMessage = false;    
    gUpdateStatus = true;
    gAppToDisplay = APP_MESSENGER;
    gRequestDisplayScreen = DISPLAY_MAIN;    
    break;
#endif	
#endif	

#if defined (ENABLE_MESSENGER) && !defined (ENABLE_SCANLIST)
#if defined (ENABLE_MESSENGER_SHOW_RX_FREQ) && !defined (ENABLE_MESSENGER_SHOW_RX_TX_FREQ)
  case KEY_1:
    hasNewMessage = false;
    uint32_t frequency = gEeprom.VfoInfo[gEeprom.TX_CHANNEL].pTX->Frequency;
    if ( IsTXAllowed(gEeprom.VfoInfo[gEeprom.TX_CHANNEL].pTX->Frequency) ) {
      frequency = GetScreenF(frequency);
      sprintf(msgFreqInfo, "%u.%05u Mhz", frequency / 100000, frequency % 100000);
    } else {
      sprintf(msgFreqInfo, "TX DISABLE");
    }
    gUpdateStatus = true;
    gAppToDisplay = APP_MESSENGER;
	  gRequestDisplayScreen = DISPLAY_MAIN;
    break;
#elif defined (ENABLE_MESSENGER_SHOW_RX_TX_FREQ) && !defined (ENABLE_MESSENGER_SHOW_RX_FREQ) || defined (ENABLE_MESSENGER_SHOW_RX_TX_FREQ) && defined (ENABLE_MESSENGER_SHOW_RX_FREQ)
  case KEY_1:
    hasNewMessage = false;
    uint32_t txFrequency = gEeprom.VfoInfo[gEeprom.TX_CHANNEL].pTX->Frequency;
    uint32_t rxFrequency = gEeprom.VfoInfo[gEeprom.RX_CHANNEL].pRX->Frequency;
    
    if (IsTXAllowed(txFrequency)) {
      txFrequency = GetScreenF(txFrequency);
      rxFrequency = GetScreenF(rxFrequency);
      sprintf(msgFreqInfo, "R:%u.%05u T:%u.%05u Mhz", rxFrequency / 100000, rxFrequency % 100000, txFrequency / 100000, txFrequency % 100000);
    } else {
      sprintf(msgFreqInfo, "TX DISABLE");
    }

    gUpdateStatus = true;
    gAppToDisplay = APP_MESSENGER;
    gRequestDisplayScreen = DISPLAY_MAIN;
    break;
#else
  case KEY_1:
    hasNewMessage = false;    
    gUpdateStatus = true;
    gAppToDisplay = APP_MESSENGER;
    gRequestDisplayScreen = DISPLAY_MAIN;    
    break;
#endif	
#endif	
/*
  case KEY_1:
    gAppToDisplay = APP_SPLIT;
    gRequestDisplayScreen = DISPLAY_MAIN;
    break;
  case KEY_2:
 1  gAppToDisplay = APP_OOK;
    gRequestDisplayScreen = DISPLAY_MAIN;
    break;
*/
  default:
    break;
  }
}