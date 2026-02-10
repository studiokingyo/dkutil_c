/*! @file
	@brief Poly1305 Message Authentication Code
	@author d.Kingyo
	@note
	Poly1305はDaniel J. Bernsteinが設計したメッセージ認証コードで、
	RFC 8439で標準化されています。

	特徴:
	- 256ビット（32バイト）キー（r:16バイト + s:16バイト）
	- 128ビット（16バイト）タグ出力
	- 高速かつ安全

	参考: RFC 8439 - ChaCha20 and Poly1305 for IETF Protocols
*/

#ifndef DKC_POLY1305_INC_
#define DKC_POLY1305_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/*! @brief キーサイズ（バイト） */
#define dkcd_POLY1305_KEY_SIZE      32

/*! @brief タグサイズ（バイト） */
#define dkcd_POLY1305_TAG_SIZE      16

/*! @brief ブロックサイズ（バイト） */
#define dkcd_POLY1305_BLOCK_SIZE    16

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief Poly1305コンテキスト構造体 */
typedef struct dkc_Poly1305{
	uint32 r[5];          /*!< クランプ済みr値（基数2^26の5リム） */
	uint32 h[5];          /*!< アキュムレータ */
	uint32 pad[4];        /*!< s = key[16..31] */
	uint8  buffer[16];    /*!< 部分ブロックバッファ */
	size_t buffer_len;    /*!< バッファ内のバイト数 */
	BYTE   finalized;     /*!< ファイナライズ済みフラグ */
}DKC_POLY1305;

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

/*!
	@brief Poly1305コンテキストを生成する
	@param key 256ビット（32バイト）のキー
	@param keysize キーサイズ（32でなければならない）
	@return 生成されたコンテキスト、失敗時はNULL
*/
DKC_EXTERN DKC_POLY1305 * WINAPI dkcAllocPoly1305(
	const unsigned char *key, size_t keysize);

/*!
	@brief Poly1305コンテキストを解放する
	@param p コンテキストへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreePoly1305(DKC_POLY1305 **p);

/*!
	@brief Poly1305コンテキストを初期化する
	@param p コンテキスト
	@param key 256ビット（32バイト）のキー
	@param keysize キーサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcPoly1305Init(
	DKC_POLY1305 *p,
	const unsigned char *key, size_t keysize);

/*!
	@brief データを追加する
	@param p コンテキスト
	@param data データ
	@param len データ長
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcPoly1305Update(
	DKC_POLY1305 *p,
	const unsigned char *data, size_t len);

/*!
	@brief MACタグを計算してファイナライズする
	@param p コンテキスト
	@param tag 16バイトのタグ出力先
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcPoly1305Final(
	DKC_POLY1305 *p, unsigned char *tag);

/*!
	@brief 2つのタグを定時間比較する
	@param tag1 タグ1（16バイト）
	@param tag2 タグ2（16バイト）
	@return edk_SUCCEEDED:一致 edk_FAILED:不一致
*/
DKC_EXTERN int WINAPI dkcPoly1305Verify(
	const unsigned char *tag1, const unsigned char *tag2);

/*!
	@brief ワンショットでMACを計算する
	@param key 256ビット（32バイト）のキー
	@param keysize キーサイズ
	@param msg メッセージ
	@param msglen メッセージ長
	@param tag 16バイトのタグ出力先
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcPoly1305OneShot(
	const unsigned char *key, size_t keysize,
	const unsigned char *msg, size_t msglen,
	unsigned char *tag);

#ifdef __cplusplus
}
#endif

#endif /* DKC_POLY1305_INC_ */
