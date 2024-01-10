#ifndef APP_MSG_H
#define APP_MSG_H

#ifdef ENABLE_MESSENGER

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "driver/keyboard.h"

typedef enum KeyboardType
{
	UPPERCASE,
	LOWERCASE,
	NUMERIC,
	END_TYPE_KBRD
} KeyboardType;

typedef enum MsgStatus
{
	READY,
	SENDING,
	RECEIVING,
} MsgStatus;

#define TX_MSG_LENGTH 30

extern KeyboardType keyboardType;
extern char cMessage[TX_MSG_LENGTH];
#ifdef ENABLE_MESSENGER_MORE_ONE_LINE
extern char rxMessage[5][TX_MSG_LENGTH + 3];
#else
extern char rxMessage[4][TX_MSG_LENGTH + 3];
#endif
#if defined(ENABLE_MESSENGER_SHOW_RX_FREQ) || defined(ENABLE_MESSENGER_SHOW_RX_TX_FREQ)
extern char msgFreqInfo[30];
#endif
extern bool hasNewMessage;
extern uint8_t keyTickCounter;
extern MsgStatus msgStatus;

void MSG_EnableRX(const bool enable);
void MSG_StorePacket(const uint16_t interrupt_bits);
void MSG_Init();
void MSG_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld);
void MSG_Send(const char txMessage[TX_MSG_LENGTH], bool bServiceMessage);

#endif

#endif
