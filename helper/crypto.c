/* Copyright 2024 kamilsss655
 * https://github.com/kamilsss655
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

#include "crypto.h"
#include "external/chacha/chacha.h"
#include "driver/bk4819.h"
#include "driver/systick.h"

bool     gRecalculateEncKey;

u_int8_t gEncryptionKey[32];

// salt used for hashing encryption key from eeprom used for sending packets
// we never actually use the key stored in eeprom directly
// 4 salts for each 8 bytes chunks of the encryption key
static const uint8_t encryptionSalt[4][8] = {
	{0xEF, 0x58, 0x0A, 0xC6, 0x12, 0x4A, 0xFA, 0x4F},
	{0xAE, 0x6F, 0x9D, 0x3C, 0xBB, 0x80, 0xAC, 0x4A},
	{0xF3, 0x5E, 0x11, 0x69, 0xC7, 0x97, 0xFB, 0xC6},
	{0x27, 0x4F, 0xB7, 0x1A, 0xA7, 0xE5, 0x77, 0x9C}
};

// salt used to display encryption key hash in menu
static const uint8_t displaySalt[32] = {
	0x22, 0xA1, 0xDA, 0x49, 0xA8, 0x22, 0x79, 0xBF, 0x2D, 0x98, 0xDE, 0xA8, 0x4F, 0xC3, 0x23, 0xF3,
	0x65, 0xB9, 0x69, 0x22, 0xB3, 0x6F, 0xB4, 0x59, 0xC7, 0x90, 0x10, 0x70, 0xFA, 0x51, 0xA0, 0x19
};

// Used for both encryption and decryption
void CRYPTO_Crypt(void *input, int input_len, void *output, void *nonce, const void *key, int key_len)
{
	struct chacha_ctx ctx;

	unsigned char keystream[CHACHA_BLOCKLEN];

	memset(&ctx, 0, sizeof(ctx));
	chacha_keysetup(&ctx, key, key_len);

	// init keystream and generate key
	memset(keystream, 0, sizeof(keystream));
	chacha_ivsetup(&ctx, nonce, NULL);
	chacha_encrypt_bytes(&ctx, keystream, keystream,sizeof(keystream));

	// crypt data, only works for input_len <= 32
	for (uint8_t i = 0; i < input_len; i++) {
		((unsigned char *)output)[i] =
			((unsigned char *)input)[i] ^ keystream[32 + i];
	}
}

// Generate random byte
uint8_t CRYPTO_RandomByte()
{
	uint8_t randByte = 0x00;
	uint8_t noise;
	for(uint8_t i = 0; i < 8; i++) {
		noise = BK4819_ReadRegister(BK4819_REG_65) & 0x007F;
		randByte |= (noise & 0x01)<< i;
		SYSTICK_DelayUs(979);
	}
	return randByte;
}

// Generate random number from the radio noise
void CRYPTO_Random(void *output, int len)
{
	for (uint8_t i = 0; i < len; i++) {
		((unsigned char *)output)[i] = CRYPTO_RandomByte();
	}
}


// Generate salted hash
// output is always 8 bytes (64 bits), 
// input can be upto 255 bytes
void CRYPTO_HashSalted(const void *input, void *output, const void *salt, int input_len, int salt_len)
{
	union eight_bytes hash;

	// FNV offset basis
	hash.u64 = 0xcbf29ce484222325;
	// FNV prime
	const uint64_t fnvPrime = 0x100000001b3;

	// hash input
	for(int i=0; i<input_len; i++){
		hash.u64 = hash.u64 * fnvPrime;
		hash.u64 = hash.u64 ^ ((unsigned char *)input)[i];
	}
	// hash salt
	for(int i=0; i<salt_len; i++){
		hash.u64 = hash.u64 * fnvPrime;
		hash.u64 = hash.u64 ^ ((unsigned char *)salt)[i];
	}
	// set output
	for(int j=0; j<8; j++){
		((uint8_t *)output)[j] = hash.b8[j];
	}
		
}

// Generate short 8 ASCII chars hash which can be safely displayed
// two devices can then compare this hash to determine if they have matching keys
// it is not used for an actual encryption
void CRYPTO_DisplayHash(void *input, void *output, int input_len)
{
	CRYPTO_HashSalted(input, output, displaySalt, input_len, sizeof(displaySalt));

	// convert uint8_t to ASCII chars
	for(int i=0; i<8; i++){
		((uint8_t *)output)[i]=(((uint8_t *)output)[i] % 91) + 32;
	}
}
// generate 32 bytes (256 bits) key by combining 4 x 8 byte hash of the same
// 10 byte (80 bits) encryption key (each hash with different salt)
void CRYPTO_Generate256BitKey(void *input, void *output, int input_len)
{
	for(int i=0; i<4; i++){
		CRYPTO_HashSalted(input, output+(i*8), encryptionSalt[i], input_len, sizeof(encryptionSalt[i]));
	}
}

			   