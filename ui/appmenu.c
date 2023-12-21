#include "appmenu.h"

void UI_DisplayAppMenu() {
  memset(gFrameBuffer, 0, sizeof(gFrameBuffer));
  
     // UI_PrintStringSmallest("Press MENU to enter MENU", 0, 8, false, true);

 //   UI_PrintStringSmallest("Telegram - https://t.me/PMR446PT", 0, 0, false, true);
 //   UI_PrintStringSmallest("Matoz - https://github.com/spm81", 0, 50, false, true);
/*
  UI_PrintStringSmallest("App menu", 0, 1, false, true);

  UI_PrintStringSmallest("Press MENU to enter Menu", 0, 10, false, true);
  UI_PrintStringSmallest("1. Edit Scanlist", 0, 16, false, true);

*/

 // UI_PrintStringSmallest("     Press MENU to enter Menu", 0, 1, false, true);
  //	UI_PrintString("APP MENU", 0, 127, 0, 9, true);
#ifdef ENABLE_MESSENGER
  UI_PrintStringSmallest("M. MENU           0. Info", 0, 10, false, true);
#else
  UI_PrintStringSmallest("M. MENU", 0, 10, false, true);
#endif

  UI_PrintStringSmallest("1. Edit Scanlist", 0, 19, false, true);

#ifdef ENABLE_MESSENGER
  UI_PrintStringSmallest("2. Messenger", 0, 28, false, true);
 #else 
  UI_PrintStringSmallest("0. Info", 0, 28, false, true);
#endif
  ST7565_BlitFullScreen();
      
  gUpdateDisplay = true;

}