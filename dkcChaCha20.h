/*! @file
	@brief ChaCha20 Stream Cipher
	@author d.Kingyo
	@note
	ChaCha20はDaniel J. Bernsteinが設計したストリーム暗号で、
	RFC 8439で標準化されています。

	特徴:
	- 256ビット（32バイト）キー
	- 96ビット（12バイト）ノンス
	- 64バイトブロック出力
	- 20ラウンド（10ダブルラウンド）

	参考: RFC 8439 - ChaCha20 and Poly1305 for IETF Protocols
*/

#ifndef DKC_CHACHA20_INC_
#define DKC_CHACHA20_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/*! @brief キーサイズ（バイト） */
#define dkcd_CHACHA20_KEY_SIZE      32

/*! @brief ノンスサイズ（バイト） */
#define dkcd_CHACHA20_NONCE_SIZE    12

/*! @brief ブロックサイズ（バイト） */
#define dkcd_CHACHA20_BLOCK_SIZE    64

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief ChaCha20コンテキスト構造体 */
typedef struct dkc_ChaCha20{
	uint32 state[16];       /*!< 4x4 状態行列 */
	uint8  keystream[64];   /*!< バッファリングされたキーストリームブロック */
	size_t position;        /*!< キーストリームバッファ内の位置 */
	uint32 counter;         /*!< ブロックカウンタ */
}DKC_CHACHA20;

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

/*!
	@brief ChaCha20コンテキストを生成する
	@param key 256ビット（32バイト）のキー
	@param keysize キーサイズ（32でなければならない）
	@param nonce 96ビット（12バイト）のノンス
	@param noncesize ノンスサイズ（12でなければならない）
	@return 生成されたコンテキスト、失敗時はNULL
*/
DKC_EXTERN DKC_CHACHA20 * WINAPI dkcAllocChaCha20(
	const unsigned char *key, size_t keysize,
	const unsigned char *nonce, size_t noncesize);

/*!
	@brief ChaCha20コンテキストを解放する
	@param p コンテキストへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeChaCha20(DKC_CHACHA20 **p);

/*!
	@brief ChaCha20コンテキストを初期化する（既存コンテキストを再利用）
	@param p コンテキスト
	@param key 256ビット（32バイト）のキー
	@param keysize キーサイズ
	@param nonce 96ビット（12バイト）のノンス
	@param noncesize ノンスサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcChaCha20Init(
	DKC_CHACHA20 *p,
	const unsigned char *key, size_t keysize,
	const unsigned char *nonce, size_t noncesize);

/*!
	@brief データを暗号化する（出力先指定）
	@param p コンテキスト
	@param dest 出力バッファ
	@param dsize 出力バッファサイズ
	@param src 入力データ
	@param ssize 入力データサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
	@note ストリーム暗号なので暗号化と復号化は同じ操作
*/
DKC_EXTERN int WINAPI dkcChaCha20Encrypt(
	DKC_CHACHA20 *p,
	unsigned char *dest, size_t dsize,
	const unsigned char *src, size_t ssize);

/*! @brief 復号化（暗号化と同じ） */
#define dkcChaCha20Decrypt(a,b,c,d,e) dkcChaCha20Encrypt(a,b,c,d,e)

/*!
	@brief データをインプレースで暗号化/復号化する
	@param p コンテキスト
	@param inout 入出力バッファ
	@param size データサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcChaCha20EncryptNoDest(
	DKC_CHACHA20 *p,
	unsigned char *inout, size_t size);

/*! @brief インプレース復号化（暗号化と同じ） */
#define dkcChaCha20DecryptNoDest(a,b,c) dkcChaCha20EncryptNoDest(a,b,c)

/*!
	@brief ブロックカウンタを設定する
	@param p コンテキスト
	@param counter 新しいカウンタ値
	@note カウンタを変更するとキーストリームバッファはリセットされる
*/
DKC_EXTERN void WINAPI dkcChaCha20SetCounter(
	DKC_CHACHA20 *p, uint32 counter);

#ifdef __cplusplus
}
#endif

#endif /* DKC_CHACHA20_INC_ */
