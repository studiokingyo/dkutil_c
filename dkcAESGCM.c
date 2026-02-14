/*! @file
	@brief AES-GCM (Galois/Counter Mode) 実装
	@author d.Kingyo
	@note
	NIST SP 800-38D に基づく実装。
	既存のdkcRijndael (AES) を内部で使用。
*/

#include "dkcAESGCM.h"
#include <string.h>

/* ====================================================================
 * ユーティリティ関数
 * ==================================================================== */

static DKC_INLINE void gcm_store32_be(unsigned char *p, uint32 v)
{
	p[0] = (unsigned char)(v >> 24);
	p[1] = (unsigned char)(v >> 16);
	p[2] = (unsigned char)(v >> 8);
	p[3] = (unsigned char)(v);
}

static DKC_INLINE uint32 gcm_load32_be(const unsigned char *p)
{
	return ((uint32)p[0] << 24) | ((uint32)p[1] << 16) |
	       ((uint32)p[2] << 8)  | ((uint32)p[3]);
}

static DKC_INLINE void gcm_store64_be(unsigned char *p, ULONGLONG v)
{
	p[0] = (unsigned char)(v >> 56);
	p[1] = (unsigned char)(v >> 48);
	p[2] = (unsigned char)(v >> 40);
	p[3] = (unsigned char)(v >> 32);
	p[4] = (unsigned char)(v >> 24);
	p[5] = (unsigned char)(v >> 16);
	p[6] = (unsigned char)(v >> 8);
	p[7] = (unsigned char)(v);
}

static DKC_INLINE void gcm_xor_block(unsigned char *dst, const unsigned char *src, size_t len)
{
	size_t i;
	for(i = 0; i < len; i++){
		dst[i] ^= src[i];
	}
}

/* AES ECB暗号化 (16バイトブロック) */
static void gcm_aes_encrypt_block(DKC_RIJNDAEL *aes, const unsigned char *in, unsigned char *out)
{
	/* dkcRijndaelBlockEncrypt takes sizes in bits */
	memcpy(out, in, 16);
	dkcRijndaelBlockEncrypt(aes, out, 128, in, 128);
}

/* ====================================================================
 * GF(2^128) 乗算 (bit-by-bit method)
 * ==================================================================== */

/*
 * GF(2^128) with reducing polynomial x^128 + x^7 + x^2 + x + 1
 * R = 0xE1000000 00000000 00000000 00000000 (MSB representation)
 */
static void ghash_multiply(const unsigned char *X, const unsigned char *Y, unsigned char *result)
{
	unsigned char V[16];
	unsigned char Z[16];
	int i, j;
	unsigned char carry;

	memset(Z, 0, 16);
	memcpy(V, Y, 16);

	for(i = 0; i < 16; i++){
		for(j = 7; j >= 0; j--){
			/* if bit (i*8 + (7-j)) of X is set, Z ^= V */
			if((X[i] >> j) & 1){
				gcm_xor_block(Z, V, 16);
			}

			/* V = V * x in GF(2^128) */
			carry = (unsigned char)(V[15] & 1);
			{
				int k;
				for(k = 15; k > 0; k--){
					V[k] = (unsigned char)((V[k] >> 1) | ((V[k-1] & 1) << 7));
				}
				V[0] = (unsigned char)(V[0] >> 1);
			}
			if(carry){
				V[0] ^= 0xE1; /* R = x^7 + x^2 + x + 1 */
			}
		}
	}

	memcpy(result, Z, 16);
}

/* GHASH: process data blocks */
static void ghash_update(const unsigned char *H, unsigned char *state,
	const unsigned char *data, size_t len)
{
	size_t i;
	unsigned char tmp[16];

	for(i = 0; i + 16 <= len; i += 16){
		gcm_xor_block(state, data + i, 16);
		ghash_multiply(state, H, tmp);
		memcpy(state, tmp, 16);
	}

	/* partial block */
	if(i < len){
		unsigned char padded[16];
		memset(padded, 0, 16);
		memcpy(padded, data + i, len - i);
		gcm_xor_block(state, padded, 16);
		ghash_multiply(state, H, tmp);
		memcpy(state, tmp, 16);
	}
}

/* Increment counter (last 4 bytes, big-endian) */
static void gcm_inc32(unsigned char *block)
{
	uint32 ctr;
	ctr = gcm_load32_be(block + 12);
	ctr++;
	gcm_store32_be(block + 12, ctr);
}

/* GCTR: AES-CTR encryption */
static void gcm_gctr(DKC_RIJNDAEL *aes, const unsigned char *icb,
	const unsigned char *input, size_t len, unsigned char *output)
{
	unsigned char cb[16];
	unsigned char keystream[16];
	size_t i;
	size_t blocklen;

	memcpy(cb, icb, 16);

	for(i = 0; i < len; i += 16){
		gcm_aes_encrypt_block(aes, cb, keystream);
		blocklen = (len - i >= 16) ? 16 : (len - i);
		{
			size_t j;
			for(j = 0; j < blocklen; j++){
				output[i + j] = (unsigned char)(input[i + j] ^ keystream[j]);
			}
		}
		gcm_inc32(cb);
	}
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN DKC_AESGCM * WINAPI dkcAllocAESGCM(
	const unsigned char *key, size_t keysize)
{
	DKC_AESGCM *p;
	unsigned char zero[16];

	if(key == NULL) return NULL;
	if(keysize != 16 && keysize != 24 && keysize != 32) return NULL;

	p = (DKC_AESGCM *)dkcAllocateFill(sizeof(DKC_AESGCM), 0);
	if(p == NULL) return NULL;

	p->aes = dkcAllocRijndaelEncrypt(key, keysize);
	if(p->aes == NULL){
		dkcFree((void **)&p);
		return NULL;
	}

	/* H = AES(K, 0^128) */
	memset(zero, 0, 16);
	gcm_aes_encrypt_block(p->aes, zero, p->H);

	return p;
}

DKC_EXTERN int WINAPI dkcFreeAESGCM(DKC_AESGCM **p)
{
	if(p == NULL || *p == NULL) return edk_FAILED;

	if((*p)->aes != NULL){
		dkcFreeRijndael(&(*p)->aes);
	}
	memset(*p, 0, sizeof(DKC_AESGCM));
	dkcFree((void **)p);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcAESGCMEncrypt(
	DKC_AESGCM *p,
	const unsigned char *iv, size_t ivlen,
	const unsigned char *aad, size_t aadlen,
	const unsigned char *pt, size_t ptlen,
	unsigned char *ct, unsigned char *tag)
{
	unsigned char J0[16];
	unsigned char J0_inc[16];
	unsigned char ghash_state[16];
	unsigned char len_block[16];
	unsigned char tag_keystream[16];
	unsigned char tmp[16];

	if(p == NULL || iv == NULL || tag == NULL) return edk_FAILED;
	if(ptlen > 0 && (pt == NULL || ct == NULL)) return edk_FAILED;

	/* Step 1: Compute J0 */
	if(ivlen == 12){
		memcpy(J0, iv, 12);
		J0[12] = 0; J0[13] = 0; J0[14] = 0; J0[15] = 1;
	}else{
		/* J0 = GHASH(H, IV || pad || len(IV)*8) */
		memset(ghash_state, 0, 16);
		ghash_update(p->H, ghash_state, iv, ivlen);
		memset(len_block, 0, 16);
		gcm_store64_be(len_block + 8, (ULONGLONG)ivlen * 8);
		gcm_xor_block(ghash_state, len_block, 16);
		ghash_multiply(ghash_state, p->H, tmp);
		memcpy(J0, tmp, 16);
	}

	/* Step 2: GHASH AAD */
	memset(ghash_state, 0, 16);
	if(aadlen > 0 && aad != NULL){
		ghash_update(p->H, ghash_state, aad, aadlen);
	}

	/* Step 3: GCTR encrypt plaintext with inc32(J0) */
	memcpy(J0_inc, J0, 16);
	gcm_inc32(J0_inc);

	if(ptlen > 0){
		gcm_gctr(p->aes, J0_inc, pt, ptlen, ct);
	}

	/* Step 4: GHASH ciphertext */
	if(ptlen > 0){
		ghash_update(p->H, ghash_state, ct, ptlen);
	}

	/* Step 5: GHASH len(AAD)||len(CT) in bits */
	memset(len_block, 0, 16);
	gcm_store64_be(len_block, (ULONGLONG)aadlen * 8);
	gcm_store64_be(len_block + 8, (ULONGLONG)ptlen * 8);
	gcm_xor_block(ghash_state, len_block, 16);
	ghash_multiply(ghash_state, p->H, tmp);
	memcpy(ghash_state, tmp, 16);

	/* Step 6: Tag = GCTR(J0) XOR GHASH_final */
	gcm_aes_encrypt_block(p->aes, J0, tag_keystream);
	gcm_xor_block(ghash_state, tag_keystream, 16);
	memcpy(tag, ghash_state, dkcd_AESGCM_TAG_SIZE);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcAESGCMDecrypt(
	DKC_AESGCM *p,
	const unsigned char *iv, size_t ivlen,
	const unsigned char *aad, size_t aadlen,
	const unsigned char *ct, size_t ctlen,
	const unsigned char *tag,
	unsigned char *pt)
{
	unsigned char J0[16];
	unsigned char J0_inc[16];
	unsigned char ghash_state[16];
	unsigned char len_block[16];
	unsigned char tag_keystream[16];
	unsigned char computed_tag[16];
	unsigned char tmp[16];
	unsigned char diff;
	int i;

	if(p == NULL || iv == NULL || tag == NULL) return edk_FAILED;
	if(ctlen > 0 && (ct == NULL || pt == NULL)) return edk_FAILED;

	/* Step 1: Compute J0 */
	if(ivlen == 12){
		memcpy(J0, iv, 12);
		J0[12] = 0; J0[13] = 0; J0[14] = 0; J0[15] = 1;
	}else{
		memset(ghash_state, 0, 16);
		ghash_update(p->H, ghash_state, iv, ivlen);
		memset(len_block, 0, 16);
		gcm_store64_be(len_block + 8, (ULONGLONG)ivlen * 8);
		gcm_xor_block(ghash_state, len_block, 16);
		ghash_multiply(ghash_state, p->H, tmp);
		memcpy(J0, tmp, 16);
	}

	/* Step 2: GHASH AAD */
	memset(ghash_state, 0, 16);
	if(aadlen > 0 && aad != NULL){
		ghash_update(p->H, ghash_state, aad, aadlen);
	}

	/* Step 3: GHASH ciphertext */
	if(ctlen > 0){
		ghash_update(p->H, ghash_state, ct, ctlen);
	}

	/* Step 4: GHASH len(AAD)||len(CT) */
	memset(len_block, 0, 16);
	gcm_store64_be(len_block, (ULONGLONG)aadlen * 8);
	gcm_store64_be(len_block + 8, (ULONGLONG)ctlen * 8);
	gcm_xor_block(ghash_state, len_block, 16);
	ghash_multiply(ghash_state, p->H, tmp);
	memcpy(ghash_state, tmp, 16);

	/* Step 5: Compute tag */
	gcm_aes_encrypt_block(p->aes, J0, tag_keystream);
	gcm_xor_block(ghash_state, tag_keystream, 16);
	memcpy(computed_tag, ghash_state, 16);

	/* Step 6: Constant-time tag comparison */
	diff = 0;
	for(i = 0; i < dkcd_AESGCM_TAG_SIZE; i++){
		diff |= computed_tag[i] ^ tag[i];
	}
	if(diff != 0){
		if(pt != NULL && ctlen > 0){
			memset(pt, 0, ctlen);
		}
		return edk_FAILED;
	}

	/* Step 7: GCTR decrypt */
	memcpy(J0_inc, J0, 16);
	gcm_inc32(J0_inc);
	if(ctlen > 0){
		gcm_gctr(p->aes, J0_inc, ct, ctlen, pt);
	}

	return edk_SUCCEEDED;
}
