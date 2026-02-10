/*! @file
	@brief Poly1305 Message Authentication Code 実装
	@author d.Kingyo
	@note
	RFC 8439 - ChaCha20 and Poly1305 for IETF Protocols
	に基づく実装。基数2^26表現を使用。
*/

#include "dkcPoly1305.h"
#include <string.h>

/* ====================================================================
 * ユーティリティ関数
 * ==================================================================== */

static DKC_INLINE uint32 poly1305_load32_le(const unsigned char *p)
{
	return ((uint32)p[0])       | ((uint32)p[1] << 8) |
	       ((uint32)p[2] << 16) | ((uint32)p[3] << 24);
}

static DKC_INLINE void poly1305_store32_le(unsigned char *p, uint32 v)
{
	p[0] = (unsigned char)(v);
	p[1] = (unsigned char)(v >> 8);
	p[2] = (unsigned char)(v >> 16);
	p[3] = (unsigned char)(v >> 24);
}

/* ====================================================================
 * 内部関数
 * ==================================================================== */

/* 1ブロック（16バイト + hibitフラグ）を処理 */
static void poly1305_block(DKC_POLY1305 *p, const unsigned char *dat, int is_final_partial)
{
	uint32 r0, r1, r2, r3, r4;
	uint32 s1, s2, s3, s4;
	uint32 h0, h1, h2, h3, h4;
	ULONGLONG d0, d1, d2, d3, d4;
	uint32 c;
	uint32 hibit;

	r0 = p->r[0]; r1 = p->r[1]; r2 = p->r[2]; r3 = p->r[3]; r4 = p->r[4];
	s1 = r1 * 5; s2 = r2 * 5; s3 = r3 * 5; s4 = r4 * 5;
	h0 = p->h[0]; h1 = p->h[1]; h2 = p->h[2]; h3 = p->h[3]; h4 = p->h[4];

	hibit = is_final_partial ? 0 : (1 << 24); /* 2^128 bit for full blocks */

	/* h += m[i] */
	h0 += (poly1305_load32_le(dat +  0)     ) & 0x3ffffff;
	h1 += (poly1305_load32_le(dat +  3) >> 2) & 0x3ffffff;
	h2 += (poly1305_load32_le(dat +  6) >> 4) & 0x3ffffff;
	h3 += (poly1305_load32_le(dat +  9) >> 6) & 0x3ffffff;
	h4 += (poly1305_load32_le(dat + 12) >> 8) | hibit;

	/* h *= r (mod 2^130 - 5) */
	d0 = ((ULONGLONG)h0 * r0) + ((ULONGLONG)h1 * s4) + ((ULONGLONG)h2 * s3) + ((ULONGLONG)h3 * s2) + ((ULONGLONG)h4 * s1);
	d1 = ((ULONGLONG)h0 * r1) + ((ULONGLONG)h1 * r0) + ((ULONGLONG)h2 * s4) + ((ULONGLONG)h3 * s3) + ((ULONGLONG)h4 * s2);
	d2 = ((ULONGLONG)h0 * r2) + ((ULONGLONG)h1 * r1) + ((ULONGLONG)h2 * r0) + ((ULONGLONG)h3 * s4) + ((ULONGLONG)h4 * s3);
	d3 = ((ULONGLONG)h0 * r3) + ((ULONGLONG)h1 * r2) + ((ULONGLONG)h2 * r1) + ((ULONGLONG)h3 * r0) + ((ULONGLONG)h4 * s4);
	d4 = ((ULONGLONG)h0 * r4) + ((ULONGLONG)h1 * r3) + ((ULONGLONG)h2 * r2) + ((ULONGLONG)h3 * r1) + ((ULONGLONG)h4 * r0);

	/* carry propagation */
	c = (uint32)(d0 >> 26); h0 = (uint32)d0 & 0x3ffffff;
	d1 += c; c = (uint32)(d1 >> 26); h1 = (uint32)d1 & 0x3ffffff;
	d2 += c; c = (uint32)(d2 >> 26); h2 = (uint32)d2 & 0x3ffffff;
	d3 += c; c = (uint32)(d3 >> 26); h3 = (uint32)d3 & 0x3ffffff;
	d4 += c; c = (uint32)(d4 >> 26); h4 = (uint32)d4 & 0x3ffffff;
	h0 += c * 5; c = h0 >> 26; h0 &= 0x3ffffff;
	h1 += c;

	p->h[0] = h0; p->h[1] = h1; p->h[2] = h2; p->h[3] = h3; p->h[4] = h4;
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcPoly1305Init(
	DKC_POLY1305 *p,
	const unsigned char *key, size_t keysize)
{
	if(p == NULL || key == NULL) return edk_FAILED;
	if(keysize != dkcd_POLY1305_KEY_SIZE) return edk_FAILED;

	memset(p, 0, sizeof(DKC_POLY1305));

	/* r = key[0..15] with clamping */
	p->r[0] = (poly1305_load32_le(key +  0)     ) & 0x3ffffff & 0x3ffffff;
	p->r[1] = (poly1305_load32_le(key +  3) >> 2) & 0x3ffff03;
	p->r[2] = (poly1305_load32_le(key +  6) >> 4) & 0x3ffc0ff;
	p->r[3] = (poly1305_load32_le(key +  9) >> 6) & 0x3f03fff;
	p->r[4] = (poly1305_load32_le(key + 12) >> 8) & 0x00fffff;

	/* s = key[16..31] */
	p->pad[0] = poly1305_load32_le(key + 16);
	p->pad[1] = poly1305_load32_le(key + 20);
	p->pad[2] = poly1305_load32_le(key + 24);
	p->pad[3] = poly1305_load32_le(key + 28);

	p->h[0] = 0; p->h[1] = 0; p->h[2] = 0; p->h[3] = 0; p->h[4] = 0;
	p->buffer_len = 0;
	p->finalized = 0;

	return edk_SUCCEEDED;
}

DKC_EXTERN DKC_POLY1305 * WINAPI dkcAllocPoly1305(
	const unsigned char *key, size_t keysize)
{
	DKC_POLY1305 *p;

	if(key == NULL || keysize != dkcd_POLY1305_KEY_SIZE) return NULL;

	p = (DKC_POLY1305 *)dkcAllocateFill(sizeof(DKC_POLY1305), 0);
	if(p == NULL) return NULL;

	if(dkcPoly1305Init(p, key, keysize) != edk_SUCCEEDED){
		dkcFree((void **)&p);
		return NULL;
	}
	return p;
}

DKC_EXTERN int WINAPI dkcFreePoly1305(DKC_POLY1305 **p)
{
	if(p == NULL || *p == NULL) return edk_FAILED;
	memset(*p, 0, sizeof(DKC_POLY1305));
	dkcFree((void **)p);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcPoly1305Update(
	DKC_POLY1305 *p,
	const unsigned char *data, size_t len)
{
	size_t i;
	size_t want;

	if(p == NULL) return edk_FAILED;
	if(p->finalized) return edk_FAILED;
	if(len == 0) return edk_SUCCEEDED;
	if(data == NULL) return edk_FAILED;

	i = 0;

	/* バッファに残りがあればまず埋める */
	if(p->buffer_len > 0){
		want = dkcd_POLY1305_BLOCK_SIZE - p->buffer_len;
		if(want > len) want = len;
		memcpy(p->buffer + p->buffer_len, data, want);
		p->buffer_len += want;
		i += want;

		if(p->buffer_len == dkcd_POLY1305_BLOCK_SIZE){
			poly1305_block(p, p->buffer, 0);
			p->buffer_len = 0;
		}
	}

	/* フルブロックを直接処理 */
	while(i + dkcd_POLY1305_BLOCK_SIZE <= len){
		poly1305_block(p, data + i, 0);
		i += dkcd_POLY1305_BLOCK_SIZE;
	}

	/* 残りをバッファに保存 */
	if(i < len){
		memcpy(p->buffer + p->buffer_len, data + i, len - i);
		p->buffer_len += len - i;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcPoly1305Final(
	DKC_POLY1305 *p, unsigned char *tag)
{
	uint32 h0, h1, h2, h3, h4, c;
	uint32 g0, g1, g2, g3, g4;
	uint32 mask;
	ULONGLONG f;

	if(p == NULL || tag == NULL) return edk_FAILED;
	if(p->finalized) return edk_FAILED;

	/* 残りバッファを処理（パディング: 0x01 + 0x00...） */
	if(p->buffer_len > 0){
		unsigned char padded[16];
		memset(padded, 0, sizeof(padded));
		memcpy(padded, p->buffer, p->buffer_len);
		padded[p->buffer_len] = 0x01;
		poly1305_block(p, padded, 1);
	}

	/* 完全なリダクション */
	h0 = p->h[0]; h1 = p->h[1]; h2 = p->h[2]; h3 = p->h[3]; h4 = p->h[4];

	c = h1 >> 26; h1 &= 0x3ffffff;
	h2 += c; c = h2 >> 26; h2 &= 0x3ffffff;
	h3 += c; c = h3 >> 26; h3 &= 0x3ffffff;
	h4 += c; c = h4 >> 26; h4 &= 0x3ffffff;
	h0 += c * 5; c = h0 >> 26; h0 &= 0x3ffffff;
	h1 += c;

	/* h - (2^130 - 5) を計算して借りがあるか確認 */
	g0 = h0 + 5; c = g0 >> 26; g0 &= 0x3ffffff;
	g1 = h1 + c; c = g1 >> 26; g1 &= 0x3ffffff;
	g2 = h2 + c; c = g2 >> 26; g2 &= 0x3ffffff;
	g3 = h3 + c; c = g3 >> 26; g3 &= 0x3ffffff;
	g4 = h4 + c - (1 << 26);

	/* g4の最上位ビットが0なら h >= p なので g を選択、そうでなければ h を選択 */
	mask = (g4 >> ((sizeof(uint32) * 8) - 1)) - 1;
	g0 &= mask; g1 &= mask; g2 &= mask; g3 &= mask; g4 &= mask;
	mask = ~mask;
	h0 = (h0 & mask) | g0;
	h1 = (h1 & mask) | g1;
	h2 = (h2 & mask) | g2;
	h3 = (h3 & mask) | g3;
	h4 = (h4 & mask) | g4;

	/* リムを128ビットに変換 */
	h0 = ((h0)       | (h1 << 26)) & 0xffffffff;
	h1 = ((h1 >>  6) | (h2 << 20)) & 0xffffffff;
	h2 = ((h2 >> 12) | (h3 << 14)) & 0xffffffff;
	h3 = ((h3 >> 18) | (h4 <<  8)) & 0xffffffff;

	/* h += s (mod 2^128) */
	f = (ULONGLONG)h0 + p->pad[0]; h0 = (uint32)f;
	f = (ULONGLONG)h1 + p->pad[1] + (f >> 32); h1 = (uint32)f;
	f = (ULONGLONG)h2 + p->pad[2] + (f >> 32); h2 = (uint32)f;
	f = (ULONGLONG)h3 + p->pad[3] + (f >> 32); h3 = (uint32)f;

	/* リトルエンディアンで出力 */
	poly1305_store32_le(tag +  0, h0);
	poly1305_store32_le(tag +  4, h1);
	poly1305_store32_le(tag +  8, h2);
	poly1305_store32_le(tag + 12, h3);

	p->finalized = 1;
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcPoly1305Verify(
	const unsigned char *tag1, const unsigned char *tag2)
{
	unsigned char diff;
	int i;

	if(tag1 == NULL || tag2 == NULL) return edk_FAILED;

	diff = 0;
	for(i = 0; i < dkcd_POLY1305_TAG_SIZE; i++){
		diff |= tag1[i] ^ tag2[i];
	}

	return (diff == 0) ? edk_SUCCEEDED : edk_FAILED;
}

DKC_EXTERN int WINAPI dkcPoly1305OneShot(
	const unsigned char *key, size_t keysize,
	const unsigned char *msg, size_t msglen,
	unsigned char *tag)
{
	DKC_POLY1305 ctx;
	int ret;

	ret = dkcPoly1305Init(&ctx, key, keysize);
	if(ret != edk_SUCCEEDED) return ret;

	ret = dkcPoly1305Update(&ctx, msg, msglen);
	if(ret != edk_SUCCEEDED) return ret;

	ret = dkcPoly1305Final(&ctx, tag);
	memset(&ctx, 0, sizeof(ctx));
	return ret;
}
