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
#include <stdbool.h>
#include <stdint.h>
extern bool          gRecalculateEncKey;
extern uint8_t       gEncryptionKey[32];
static const uint8_t encryptionSalt[4][8];
static const uint8_t displaySalt[32];

union eight_bytes {
  uint64_t u64;
  uint8_t b8[sizeof(uint64_t)];
};

// Used for both encryption and decryption
void CRYPTO_Crypt(void *input, int input_len, void *output, void *nonce, const void *key, int key_len);
void CRYPTO_Random(void *output, int len);
uint8_t CRYPTO_RandomByte();
void CRYPTO_DisplayHash(void *input, void *output, int input_len);
void CRYPTO_Generate256BitKey(void *input, void *output, int input_len);
void CRYPTO_HashSalted(const void *input, void *output, const void *salt, int input_len, int salt_len);