#ifndef APP_MSG_H
#define APP_MSG_H

#ifdef ENABLE_MESSENGER

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "driver/keyboard.h"

#ifdef ENABLE_MESSENGER_MORE_ONE_LINE
#define LAST_LINE 5
#else
#define LAST_LINE 4
#endif

// Modem Modulation                             // 2024 kamilsss655
typedef enum ModemModulation {
  MOD_AFSK_1200, // for good conditions
  MOD_FSK_700,   // for medium conditions
  MOD_FSK_450,   // for bad conditions
} ModemModulation;

typedef union {
  struct {
    uint8_t      
	  unused0    	:1,
	  notification	:1, // determines whether the radio will automatically respond to messages with Roger Notification
      ack        	:1, // determines whether the radio will automatically respond to messages with ACK
      encrypt    	:1, // determines whether outgoing messages will be encrypted
      unused1    	:1,
      modulation 	:2, // determines FSK modulation type
      unused2    	:2;
  } data;
  uint8_t __val;
} MessengerConfig;

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
#define NONCE_LENGTH 13

typedef enum PacketType {
    MESSAGE_PACKET = 100u,
    ENCRYPTED_MESSAGE_PACKET,
    ACK_PACKET,
} PacketType;
// Data Packet definition                            // 2024 kamilsss655
union DataPacket
{
  struct{
    uint8_t header;
    uint8_t payload[TX_MSG_LENGTH];
    unsigned char nonce[NONCE_LENGTH];
    // uint8_t signature[SIGNATURE_LENGTH];
  } data;
  // header + payload + nonce = must be an even number
  uint8_t serializedArray[1+TX_MSG_LENGTH+NONCE_LENGTH];
};

extern KeyboardType keyboardType;
extern char cMessage[TX_MSG_LENGTH];
extern char rxMessage[LAST_LINE + 1][TX_MSG_LENGTH + 2];
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
