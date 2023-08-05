
#include "riscv_crypto.h"

#include "aes/aes_api.h"
#include "aes/aes_rvk32.h"
#include "aes/aes_rvk64.h"
#include "aes/aes_otf_rvk64.h"

#include "test_rvkat.h"


void init_aes()
{
    aes128_enc_key = aes128_enc_key_rvk32;
	aes192_enc_key = aes192_enc_key_rvk32;
	aes256_enc_key = aes256_enc_key_rvk32;

	aes128_enc_ecb = aes128_enc_ecb_rvk32;
	aes192_enc_ecb = aes192_enc_ecb_rvk32;
	aes256_enc_ecb = aes256_enc_ecb_rvk32;

	aes128_dec_key = aes128_dec_key_rvk32;
	aes192_dec_key = aes192_dec_key_rvk32;
	aes256_dec_key = aes256_dec_key_rvk32;

	aes128_dec_ecb = aes128_dec_ecb_rvk32;
	aes192_dec_ecb = aes192_dec_ecb_rvk32;
	aes256_dec_ecb = aes256_dec_ecb_rvk32;

}

void aes_enc(uint8_t pt[16], uint8_t ct[16],
                      uint8_t key[32],uint32_t rk[AES256_RK_WORDS])
{   
    aes128_enc_key = aes128_enc_key_rvk32;
    aes128_enc_ecb = aes128_enc_ecb_rvk32;
    aes128_enc_key(rk, key);
    aes128_enc_ecb(ct, pt, rk);
    return;
}

void aes_dec(uint8_t pt[16], uint8_t ct[16],
                      uint8_t key[32],uint32_t rk[AES256_RK_WORDS])
{   
    aes128_dec_key = aes128_dec_key_rvk32;
    aes128_dec_ecb = aes128_dec_ecb_rvk32;
    aes128_dec_key(rk, key);
    aes128_dec_ecb(pt, ct, rk);
    return;
}
