//	test_main.c
//	2021-02-13	Markku-Juhani O. Saarinen <mjos@pqshield();om>
//	Copyright (c) 2021, PQShield Ltd. All rights reserved.

//	=== Main driver for the algorithm tests.

#include "riscv_crypto.h"
#include "test_rvkat.h"

#include "aes/aes_api.h"

//	algorithm tests

int test_aes();		//	test_aes.c
int test_gcm();		//	test_gcm.c
int test_sha2();	//	test_sha2.c
int test_sha3();	//	test_sha3.c
int test_sm3();		//	test_sm3.c
int test_sm4();		//	test_sm4.c
int test_present(); //	test_present.c
int test_zkr(); 	//	test_zkr.c

//	stub main: run unit tests

// int main()
// {
// 	int fail = 0;

// 	fail += test_aes();
// 	fail += test_gcm();
// 	fail += test_sha2();
// 	fail += test_sha3();
// 	fail += test_sm3();
// 	fail += test_sm4();
// 	fail += test_present();
// #ifdef RVKINTRIN_ZKR
// 	fail += test_zkr();
// #endif

// 	if (fail) {
// 		rvkat_info("RVKAT self-test finished: FAIL (there were errors)");
// 	} else {
// 		rvkat_info("RVKAT self-test finished: PASS (no errors)");
// 	}

// 	return fail;
// }

/*---Ray's Main---*/

int main ()
{
	uint8_t plain_text[16] = {'H','e','l','l','o',' ','W','o','r','l','d','!'};
	uint8_t key[32] = {1,1,4,5,1,4,1,9,1,9,8,1,0,1,1,4,5,1,4,1,9,1,9,8,1,0,1,1,4,5,1,4};

	uint8_t cipher_text[16]={0};
	uint32_t rk[AES256_RK_WORDS];

	aes_enc(plain_text,cipher_text,key,rk);

	//print cipher text
	printf("Printing cipher text:\n");
	for(int i=0;i<16;i++)
	{
		printf("%x",cipher_text[i]);
	}
	printf("\n");

	//decrypt
	aes_dec(plain_text,cipher_text,key,rk);

	//print plain text
	printf("Printing plain text:\n");
	for(int i=0;i<16;i++)
	{
		if (plain_text[i] == NULL)
			break;
		printf("%c",plain_text[i]);
	}
	printf("\n");

	return 0;
}
/*---Ray's Main---*/