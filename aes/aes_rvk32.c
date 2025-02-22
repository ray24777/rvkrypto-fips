//	aes_rvk32.c
//	2020-01-22	Markku-Juhani O. Saarinen <mjos@pqhsield.com>
//	Copyright (c) 2020, PQShield Ltd. All rights reserved.

//	===	FIPS 197 Algorithm Implementation for RV32 Krypto / AES32 

#include "riscv_crypto.h"

#ifdef RVKINTRIN_RV32

#include "aes_api.h"
#include "rv_endian.h"
#include <stddef.h>

#include <stdio.h>
//	=== ENCRYPTION ===

//	Encrypt rounds. Implements AES-128/192/256 depending on nr = {10,12,14}

void aes_enc_rounds_rvk32(uint8_t ct[16], const uint8_t pt[16],
						   const uint32_t rk[], int nr)
{
	uint32_t t0, t1, t2, t3;				//	even round state registers
	uint32_t u0, u1, u2, u3;				//	odd round state registers
	const uint32_t *kp = &rk[4 * nr];		//	key pointer as loop condition

	t0 = rk[0];								//	fetch even subkey
	t1 = rk[1];
	t2 = rk[2];
	t3 = rk[3];

	t0 ^= get32u_le(pt);					//	xor with plaintext block
	t1 ^= get32u_le(pt + 4);
	t2 ^= get32u_le(pt + 8);
	t3 ^= get32u_le(pt + 12);

	while (1) {								//	double round

		u0 = rk[4];							//	fetch odd subkey
		u1 = rk[5];
		u2 = rk[6];
		u3 = rk[7];

		u0 = _rv32_aes32esmi(u0, t0, 0);	//	AES round, 16 instructions
		u0 = _rv32_aes32esmi(u0, t1, 1);
		u0 = _rv32_aes32esmi(u0, t2, 2);
		u0 = _rv32_aes32esmi(u0, t3, 3);

		u1 = _rv32_aes32esmi(u1, t1, 0);
		u1 = _rv32_aes32esmi(u1, t2, 1);
		u1 = _rv32_aes32esmi(u1, t3, 2);
		u1 = _rv32_aes32esmi(u1, t0, 3);

		u2 = _rv32_aes32esmi(u2, t2, 0);
		u2 = _rv32_aes32esmi(u2, t3, 1);
		u2 = _rv32_aes32esmi(u2, t0, 2);
		u2 = _rv32_aes32esmi(u2, t1, 3);

		u3 = _rv32_aes32esmi(u3, t3, 0);
		u3 = _rv32_aes32esmi(u3, t0, 1);
		u3 = _rv32_aes32esmi(u3, t1, 2);
		u3 = _rv32_aes32esmi(u3, t2, 3);

		t0 = rk[8];							//	fetch even subkey
		t1 = rk[9];
		t2 = rk[10];
		t3 = rk[11];

		rk += 8;							//	step key pointer
		if (rk == kp)						//	final round ?
			break;

		t0 = _rv32_aes32esmi(t0, u0, 0);	//	AES round, 16 instructions
		t0 = _rv32_aes32esmi(t0, u1, 1);
		t0 = _rv32_aes32esmi(t0, u2, 2);
		t0 = _rv32_aes32esmi(t0, u3, 3);

		t1 = _rv32_aes32esmi(t1, u1, 0);
		t1 = _rv32_aes32esmi(t1, u2, 1);
		t1 = _rv32_aes32esmi(t1, u3, 2);
		t1 = _rv32_aes32esmi(t1, u0, 3);

		t2 = _rv32_aes32esmi(t2, u2, 0);
		t2 = _rv32_aes32esmi(t2, u3, 1);
		t2 = _rv32_aes32esmi(t2, u0, 2);
		t2 = _rv32_aes32esmi(t2, u1, 3);

		t3 = _rv32_aes32esmi(t3, u3, 0);
		t3 = _rv32_aes32esmi(t3, u0, 1);
		t3 = _rv32_aes32esmi(t3, u1, 2);
		t3 = _rv32_aes32esmi(t3, u2, 3);
	}

	t0 = _rv32_aes32esi(t0, u0, 0);			//	final round is different
	t0 = _rv32_aes32esi(t0, u1, 1);
	t0 = _rv32_aes32esi(t0, u2, 2);
	t0 = _rv32_aes32esi(t0, u3, 3);

	t1 = _rv32_aes32esi(t1, u1, 0);
	t1 = _rv32_aes32esi(t1, u2, 1);
	t1 = _rv32_aes32esi(t1, u3, 2);
	t1 = _rv32_aes32esi(t1, u0, 3);

	t2 = _rv32_aes32esi(t2, u2, 0);
	t2 = _rv32_aes32esi(t2, u3, 1);
	t2 = _rv32_aes32esi(t2, u0, 2);
	t2 = _rv32_aes32esi(t2, u1, 3);

	t3 = _rv32_aes32esi(t3, u3, 0);
	t3 = _rv32_aes32esi(t3, u0, 1);
	t3 = _rv32_aes32esi(t3, u1, 2);
	t3 = _rv32_aes32esi(t3, u2, 3);

	put32u_le(ct, t0);						//	write ciphertext block
	put32u_le(ct + 4, t1);
	put32u_le(ct + 8, t2);
	put32u_le(ct + 12, t3);
}

//	Wrappers

void aes128_enc_ecb_rvk32(uint8_t ct[16], const uint8_t pt[16],
						   const uint32_t rk[AES128_RK_WORDS])
{
	aes_enc_rounds_rvk32(ct, pt, rk, AES128_ROUNDS);
}

void aes192_enc_ecb_rvk32(uint8_t ct[16], const uint8_t pt[16],
						   const uint32_t rk[AES192_RK_WORDS])
{
	aes_enc_rounds_rvk32(ct, pt, rk, AES192_ROUNDS);
}

void aes256_enc_ecb_rvk32(uint8_t ct[16], const uint8_t pt[16],
						   const uint32_t rk[AES256_RK_WORDS])
{
	aes_enc_rounds_rvk32(ct, pt, rk, AES256_ROUNDS);
}

//	Key schedule for AES-128 Encryption.

void aes128_enc_key_rvk32(uint32_t rk[44], const uint8_t key[16])
{
	//	AES Round Constants

	/*Test code by ray*/
	// //print the default value of rk and key
	// for (uint8_t i = 0; i < 44; i++) {
	// 	printf("rk[%d]=%x\n",i,rk[i]);
	// }
	// for (uint8_t i = 0; i < 16; i++) {
	// 	printf("key[%d]=%x\n",i,key[i]);
	// }
	/*Test code by ray*/
	const uint8_t aes_rcon[] = {
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
	};

	uint32_t t0, t1, t2, t3, tr;			//	subkey registers
	const uint32_t *rke = &rk[44 - 4];		//	end pointer
	const uint8_t *rc = aes_rcon;			//	round constants

	t0 = get32u_le(key);					//	load secret key
	t1 = get32u_le(key + 4);
	t2 = get32u_le(key + 8);
	t3 = get32u_le(key + 12);

	while (1) {

		rk[0] = t0;							//	store subkey
		rk[1] = t1;
		rk[2] = t2;
		rk[3] = t3;

		if (rk == rke)						//	end condition
			return;
		rk += 4;							//	step pointer by one subkey

		t0 ^= (uint32_t) * rc++;			//	round constant
		tr = _rv32_ror(t3, 8);				//	rotate 8 bits (little endian!)
		t0 = _rv32_aes32esi(t0, tr, 0);		//	SubWord()
		t0 = _rv32_aes32esi(t0, tr, 1);
		t0 = _rv32_aes32esi(t0, tr, 2);
		t0 = _rv32_aes32esi(t0, tr, 3);
		t1 ^= t0;
		t2 ^= t1;
		t3 ^= t2;
	}
}

//	Key schedule for AES-192 encryption.

void aes192_enc_key_rvk32(uint32_t rk[52], const uint8_t key[24])
{
	//	AES Round Constants
	const uint8_t aes_rcon[] = {
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
	};

	uint32_t t0, t1, t2, t3, t4, t5, tr;	//	subkey registers
	const uint32_t *rke = &rk[52 - 4];		//	end pointer
	const uint8_t *rc = aes_rcon;			//	round constants

	t0 = get32u_le(key);					//	load secret key
	t1 = get32u_le(key + 4);
	t2 = get32u_le(key + 8);
	t3 = get32u_le(key + 12);
	t4 = get32u_le(key + 16);
	t5 = get32u_le(key + 20);

	while (1) {

		rk[0] = t0;							//	store subkey (or part)
		rk[1] = t1;
		rk[2] = t2;
		rk[3] = t3;
		if (rk == rke)						//	end condition
			return;
		rk[4] = t4;
		rk[5] = t5;
		rk += 6;							//	step pointer by 1.5 subkeys

		t0 ^= (uint32_t) * rc++;			//	round constant
		tr = _rv32_ror(t5, 8);				//	rotate 8 bits (little endian!)
		t0 = _rv32_aes32esi(t0, tr, 0);		//	SubWord()
		t0 = _rv32_aes32esi(t0, tr, 1);
		t0 = _rv32_aes32esi(t0, tr, 2);
		t0 = _rv32_aes32esi(t0, tr, 3);

		t1 ^= t0;
		t2 ^= t1;
		t3 ^= t2;
		t4 ^= t3;
		t5 ^= t4;
	}
}

//	Key schedule for AES-256 encryption.

void aes256_enc_key_rvk32(uint32_t rk[60], const uint8_t key[32])
{
	//	AES Round Constants
	const uint8_t aes_rcon[] = {
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
	};

	uint32_t t0, t1, t2, t3, t4, t5, t6, t7, tr;	// subkey registers
	const uint32_t *rke = &rk[60 - 4];		//	end pointer
	const uint8_t *rc = aes_rcon;			//	round constants

	t0 = get32u_le(key);
	t1 = get32u_le(key + 4);
	t2 = get32u_le(key + 8);
	t3 = get32u_le(key + 12);
	t4 = get32u_le(key + 16);
	t5 = get32u_le(key + 20);
	t6 = get32u_le(key + 24);
	t7 = get32u_le(key + 28);

	rk[0] = t0;								//	store first subkey
	rk[1] = t1;
	rk[2] = t2;
	rk[3] = t3;

	while (1) {

		rk[4] = t4;							//	store odd subkey
		rk[5] = t5;
		rk[6] = t6;
		rk[7] = t7;
		rk += 8;							//	step pointer by 2 subkeys

		t0 ^= (uint32_t) * rc++;			//	round constant
		tr = _rv32_ror(t7, 8);				//	rotate 8 bits (little endian!)
		t0 = _rv32_aes32esi(t0, tr, 0);		//	SubWord()
		t0 = _rv32_aes32esi(t0, tr, 1);
		t0 = _rv32_aes32esi(t0, tr, 2);
		t0 = _rv32_aes32esi(t0, tr, 3);
		t1 ^= t0;
		t2 ^= t1;
		t3 ^= t2;

		rk[0] = t0;							//	store even subkey
		rk[1] = t1;
		rk[2] = t2;
		rk[3] = t3;
		if (rk == rke)						//	end condition
			return;

		t4 = _rv32_aes32esi(t4, t3, 0);		//	SubWord() - NO rotation
		t4 = _rv32_aes32esi(t4, t3, 1);
		t4 = _rv32_aes32esi(t4, t3, 2);
		t4 = _rv32_aes32esi(t4, t3, 3);
		t5 ^= t4;
		t6 ^= t5;
		t7 ^= t6;
	}
}

//	=== DECRYPTION ===

//	Decrypt rounds. Implements AES-128/192/256 depending on nr = {10,12,14}

void aes_dec_rounds_rvk32(uint8_t pt[16], const uint8_t ct[16],
						   const uint32_t rk[], int nr)
{
	uint32_t t0, t1, t2, t3;				//	even round state registers
	uint32_t u0, u1, u2, u3;				//	odd round state registers
	const uint32_t *kp = &rk[4 * nr];		//	key pointer

	t0 = kp[0];								//	fetch last subkey
	t1 = kp[1];
	t2 = kp[2];
	t3 = kp[3];
	kp -= 8;

	t0 ^= get32u_le(ct);					//	xor with ciphertext block
	t1 ^= get32u_le(ct + 4);
	t2 ^= get32u_le(ct + 8);
	t3 ^= get32u_le(ct + 12);

	while (1) {
		u0 = kp[4];							//	fetch odd subkey
		u1 = kp[5];
		u2 = kp[6];
		u3 = kp[7];

		u0 = _rv32_aes32dsmi(u0, t0, 0);	//	AES decryption round, 16 instr
		u0 = _rv32_aes32dsmi(u0, t3, 1);
		u0 = _rv32_aes32dsmi(u0, t2, 2);
		u0 = _rv32_aes32dsmi(u0, t1, 3);

		u1 = _rv32_aes32dsmi(u1, t1, 0);
		u1 = _rv32_aes32dsmi(u1, t0, 1);
		u1 = _rv32_aes32dsmi(u1, t3, 2);
		u1 = _rv32_aes32dsmi(u1, t2, 3);

		u2 = _rv32_aes32dsmi(u2, t2, 0);
		u2 = _rv32_aes32dsmi(u2, t1, 1);
		u2 = _rv32_aes32dsmi(u2, t0, 2);
		u2 = _rv32_aes32dsmi(u2, t3, 3);

		u3 = _rv32_aes32dsmi(u3, t3, 0);
		u3 = _rv32_aes32dsmi(u3, t2, 1);
		u3 = _rv32_aes32dsmi(u3, t1, 2);
		u3 = _rv32_aes32dsmi(u3, t0, 3);

		t0 = kp[0];							//	fetch even subkey
		t1 = kp[1];
		t2 = kp[2];
		t3 = kp[3];

		if (kp == rk)						//	final round
			break;
		kp -= 8;

		t0 = _rv32_aes32dsmi(t0, u0, 0);	//	AES decryption round, 16 instr
		t0 = _rv32_aes32dsmi(t0, u3, 1);
		t0 = _rv32_aes32dsmi(t0, u2, 2);
		t0 = _rv32_aes32dsmi(t0, u1, 3);

		t1 = _rv32_aes32dsmi(t1, u1, 0);
		t1 = _rv32_aes32dsmi(t1, u0, 1);
		t1 = _rv32_aes32dsmi(t1, u3, 2);
		t1 = _rv32_aes32dsmi(t1, u2, 3);

		t2 = _rv32_aes32dsmi(t2, u2, 0);
		t2 = _rv32_aes32dsmi(t2, u1, 1);
		t2 = _rv32_aes32dsmi(t2, u0, 2);
		t2 = _rv32_aes32dsmi(t2, u3, 3);

		t3 = _rv32_aes32dsmi(t3, u3, 0);
		t3 = _rv32_aes32dsmi(t3, u2, 1);
		t3 = _rv32_aes32dsmi(t3, u1, 2);
		t3 = _rv32_aes32dsmi(t3, u0, 3);
	}

	t0 = _rv32_aes32dsi(t0, u0, 0);			//	final decryption round, 16 ins.
	t0 = _rv32_aes32dsi(t0, u3, 1);
	t0 = _rv32_aes32dsi(t0, u2, 2);
	t0 = _rv32_aes32dsi(t0, u1, 3);

	t1 = _rv32_aes32dsi(t1, u1, 0);
	t1 = _rv32_aes32dsi(t1, u0, 1);
	t1 = _rv32_aes32dsi(t1, u3, 2);
	t1 = _rv32_aes32dsi(t1, u2, 3);

	t2 = _rv32_aes32dsi(t2, u2, 0);
	t2 = _rv32_aes32dsi(t2, u1, 1);
	t2 = _rv32_aes32dsi(t2, u0, 2);
	t2 = _rv32_aes32dsi(t2, u3, 3);

	t3 = _rv32_aes32dsi(t3, u3, 0);
	t3 = _rv32_aes32dsi(t3, u2, 1);
	t3 = _rv32_aes32dsi(t3, u1, 2);
	t3 = _rv32_aes32dsi(t3, u0, 3);

	put32u_le(pt, t0);						//	write plaintext block
	put32u_le(pt + 4, t1);
	put32u_le(pt + 8, t2);
	put32u_le(pt + 12, t3);
}

//	Wrappers

void aes128_dec_ecb_rvk32(uint8_t pt[16], const uint8_t ct[16],
						   const uint32_t rk[AES128_RK_WORDS])
{
	aes_dec_rounds_rvk32(pt, ct, rk, AES128_ROUNDS);
}

void aes192_dec_ecb_rvk32(uint8_t pt[16], const uint8_t ct[16],
						   const uint32_t rk[AES192_RK_WORDS])
{
	aes_dec_rounds_rvk32(pt, ct, rk, AES192_ROUNDS);
}

void aes256_dec_ecb_rvk32(uint8_t pt[16], const uint8_t ct[16],
						   const uint32_t rk[AES256_RK_WORDS])
{
	aes_dec_rounds_rvk32(pt, ct, rk, AES256_ROUNDS);
}

//	Helper: apply inverse mixcolumns to a vector

void rvk32_dec_invmc(uint32_t * v, size_t len)
{
	size_t i;
	uint32_t x, y;

	for (i = 0; i < len; i++) {
		x = v[i];

		y = _rv32_aes32esi(0, x, 0);			//	SubWord()
		y = _rv32_aes32esi(y, x, 1);
		y = _rv32_aes32esi(y, x, 2);
		y = _rv32_aes32esi(y, x, 3);

		x = _rv32_aes32dsmi(0, y, 0);			//	Just want inv MixCol()
		x = _rv32_aes32dsmi(x, y, 1);
		x = _rv32_aes32dsmi(x, y, 2);
		x = _rv32_aes32dsmi(x, y, 3);

		v[i] = x;
	}
}

//	Key schedule for AES-128 decryption.

void aes128_dec_key_rvk32(uint32_t rk[44], const uint8_t key[16])
{
	//	create an encryption key and modify middle rounds
	aes128_enc_key(rk, key);
	rvk32_dec_invmc(rk + 4, AES128_RK_WORDS - 8);
}

//	Key schedule for AES-192 decryption.

void aes192_dec_key_rvk32(uint32_t rk[52], const uint8_t key[24])
{
	//	create an encryption key and modify middle rounds
	aes192_enc_key(rk, key);
	rvk32_dec_invmc(rk + 4, AES192_RK_WORDS - 8);
}

//	Key schedule for AES-256 decryption.

void aes256_dec_key_rvk32(uint32_t rk[60], const uint8_t key[32])
{
	//	create an encryption key and modify middle rounds
	aes256_enc_key(rk, key);
	rvk32_dec_invmc(rk + 4, AES256_RK_WORDS - 8);
}

#endif	//	RVKINTRIN_RV32
