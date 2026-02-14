/*! @file
	@brief ChaCha20 Stream Cipher 実装
	@author d.Kingyo
	@note
	RFC 8439 - ChaCha20 and Poly1305 for IETF Protocols
	に基づく実装
*/

#include "dkcChaCha20.h"
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable: 4127) /* conditional expression is constant (do-while(0)) */
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/* "expand 32-byte k" をリトルエンディアン uint32 として */
#define CHACHA20_CONST0  0x61707865UL  /* "expa" */
#define CHACHA20_CONST1  0x3320646eUL  /* "nd 3" */
#define CHACHA20_CONST2  0x79622d32UL  /* "2-by" */
#define CHACHA20_CONST3  0x6b206574UL  /* "te k" */

/* ====================================================================
 * ユーティリティ関数
 * ==================================================================== */

/* リトルエンディアンで32ビット読み込み */
static DKC_INLINE uint32 chacha20_load32_le(const unsigned char *p)
{
	return ((uint32)p[0])       | ((uint32)p[1] << 8) |
	       ((uint32)p[2] << 16) | ((uint32)p[3] << 24);
}

/* リトルエンディアンで32ビット書き込み */
static DKC_INLINE void chacha20_store32_le(unsigned char *p, uint32 v)
{
	p[0] = (unsigned char)(v);
	p[1] = (unsigned char)(v >> 8);
	p[2] = (unsigned char)(v >> 16);
	p[3] = (unsigned char)(v >> 24);
}

/* 32ビット左ローテーション */
#define CHACHA20_ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* ====================================================================
 * ChaCha20 クォーターラウンド
 * ==================================================================== */

/* RFC 8439 Section 2.1: The ChaCha Quarter Round */
#define CHACHA20_QR(a, b, c, d) \
	do { \
		(a) += (b); (d) ^= (a); (d) = CHACHA20_ROTL32((d), 16); \
		(c) += (d); (b) ^= (c); (b) = CHACHA20_ROTL32((b), 12); \
		(a) += (b); (d) ^= (a); (d) = CHACHA20_ROTL32((d),  8); \
		(c) += (d); (b) ^= (c); (b) = CHACHA20_ROTL32((b),  7); \
	} while(0)

/* ====================================================================
 * ChaCha20 ブロック関数
 * ==================================================================== */

/*
	RFC 8439 Section 2.3: The ChaCha20 Block Function

	状態行列のレイアウト:
	 0  1  2  3   <- 定数 "expand 32-byte k"
	 4  5  6  7   <- キー (256ビット前半)
	 8  9 10 11   <- キー (256ビット後半)
	12 13 14 15   <- カウンタ + ノンス
*/
static void chacha20_block(const uint32 state[16], unsigned char out[64])
{
	uint32 x[16];
	int i;

	/* 作業コピーを作成 */
	for(i = 0; i < 16; i++){
		x[i] = state[i];
	}

	/* 20ラウンド = 10ダブルラウンド */
	for(i = 0; i < 10; i++){
		/* カラムラウンド */
		CHACHA20_QR(x[ 0], x[ 4], x[ 8], x[12]);
		CHACHA20_QR(x[ 1], x[ 5], x[ 9], x[13]);
		CHACHA20_QR(x[ 2], x[ 6], x[10], x[14]);
		CHACHA20_QR(x[ 3], x[ 7], x[11], x[15]);

		/* ダイアゴナルラウンド */
		CHACHA20_QR(x[ 0], x[ 5], x[10], x[15]);
		CHACHA20_QR(x[ 1], x[ 6], x[11], x[12]);
		CHACHA20_QR(x[ 2], x[ 7], x[ 8], x[13]);
		CHACHA20_QR(x[ 3], x[ 4], x[ 9], x[14]);
	}

	/* 元の状態を加算してリトルエンディアンで出力 */
	for(i = 0; i < 16; i++){
		chacha20_store32_le(out + i * 4, x[i] + state[i]);
	}
}

/* キーストリームバッファを生成し、カウンタをインクリメント */
static void chacha20_generate_block(DKC_CHACHA20 *p)
{
	/* カウンタを状態に設定 */
	p->state[12] = p->counter;

	/* ブロック生成 */
	chacha20_block(p->state, p->keystream);

	/* カウンタをインクリメント */
	p->counter++;

	/* バッファ位置をリセット */
	p->position = 0;
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcChaCha20Init(
	DKC_CHACHA20 *p,
	const unsigned char *key, size_t keysize,
	const unsigned char *nonce, size_t noncesize)
{
	int i;

	if(p == NULL || key == NULL || nonce == NULL) return edk_FAILED;
	if(keysize != dkcd_CHACHA20_KEY_SIZE) return edk_FAILED;
	if(noncesize != dkcd_CHACHA20_NONCE_SIZE) return edk_FAILED;

	/* 定数 "expand 32-byte k" */
	p->state[0] = CHACHA20_CONST0;
	p->state[1] = CHACHA20_CONST1;
	p->state[2] = CHACHA20_CONST2;
	p->state[3] = CHACHA20_CONST3;

	/* キー（8ワード = 32バイト） */
	for(i = 0; i < 8; i++){
		p->state[4 + i] = chacha20_load32_le(key + i * 4);
	}

	/* カウンタ（初期値0） */
	p->counter = 0;
	p->state[12] = 0;

	/* ノンス（3ワード = 12バイト） */
	p->state[13] = chacha20_load32_le(nonce);
	p->state[14] = chacha20_load32_le(nonce + 4);
	p->state[15] = chacha20_load32_le(nonce + 8);

	/* キーストリームバッファを空にする（次の使用時に生成） */
	p->position = dkcd_CHACHA20_BLOCK_SIZE;
	memset(p->keystream, 0, sizeof(p->keystream));

	return edk_SUCCEEDED;
}

DKC_EXTERN DKC_CHACHA20 * WINAPI dkcAllocChaCha20(
	const unsigned char *key, size_t keysize,
	const unsigned char *nonce, size_t noncesize)
{
	DKC_CHACHA20 *p;

	if(key == NULL || nonce == NULL) return NULL;
	if(keysize != dkcd_CHACHA20_KEY_SIZE) return NULL;
	if(noncesize != dkcd_CHACHA20_NONCE_SIZE) return NULL;

	p = (DKC_CHACHA20 *)dkcAllocateFill(sizeof(DKC_CHACHA20), 0);
	if(p == NULL) return NULL;

	if(dkcChaCha20Init(p, key, keysize, nonce, noncesize) != edk_SUCCEEDED){
		dkcFree((void **)&p);
		return NULL;
	}

	return p;
}

DKC_EXTERN int WINAPI dkcFreeChaCha20(DKC_CHACHA20 **p)
{
	if(p == NULL || *p == NULL) return edk_FAILED;

	/* センシティブデータをクリア */
	memset(*p, 0, sizeof(DKC_CHACHA20));
	dkcFree((void **)p);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcChaCha20Encrypt(
	DKC_CHACHA20 *p,
	unsigned char *dest, size_t dsize,
	const unsigned char *src, size_t ssize)
{
	size_t i;
	size_t avail;

	if(p == NULL || dest == NULL || src == NULL) return edk_FAILED;
	if(dsize < ssize) return edk_FAILED;

	i = 0;
	while(i < ssize){
		/* キーストリームバッファが空なら新しいブロックを生成 */
		if(p->position >= dkcd_CHACHA20_BLOCK_SIZE){
			chacha20_generate_block(p);
		}

		/* バッファ内の残りバイト数 */
		avail = dkcd_CHACHA20_BLOCK_SIZE - p->position;
		if(avail > ssize - i){
			avail = ssize - i;
		}

		/* XOR処理（バッチ） */
		{
			size_t j;
			for(j = 0; j < avail; j++){
				dest[i + j] = (unsigned char)(src[i + j] ^ p->keystream[p->position + j]);
			}
		}

		p->position += avail;
		i += avail;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcChaCha20EncryptNoDest(
	DKC_CHACHA20 *p,
	unsigned char *inout, size_t size)
{
	size_t i;
	size_t avail;

	if(p == NULL || inout == NULL) return edk_FAILED;

	i = 0;
	while(i < size){
		/* キーストリームバッファが空なら新しいブロックを生成 */
		if(p->position >= dkcd_CHACHA20_BLOCK_SIZE){
			chacha20_generate_block(p);
		}

		/* バッファ内の残りバイト数 */
		avail = dkcd_CHACHA20_BLOCK_SIZE - p->position;
		if(avail > size - i){
			avail = size - i;
		}

		/* XOR処理（バッチ） */
		{
			size_t j;
			for(j = 0; j < avail; j++){
				inout[i + j] ^= p->keystream[p->position + j];
			}
		}

		p->position += avail;
		i += avail;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcChaCha20SetCounter(
	DKC_CHACHA20 *p, uint32 counter)
{
	if(p == NULL) return;

	p->counter = counter;
	p->state[12] = counter;

	/* カウンタ変更時はバッファを無効化して再生成を強制 */
	p->position = dkcd_CHACHA20_BLOCK_SIZE;
}
