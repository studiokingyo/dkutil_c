/*! @file
	@brief Threefish Block Cipher
	@author d.Kingyo
	@note
	Threefishは、SHA-3コンペティションのファイナリストであるSkeinハッシュ関数で
	使用されるtweakable block cipherです。

	特徴:
	- ブロックサイズ: 256, 512, 1024ビット
	- キーサイズ: ブロックサイズと同じ
	- Tweakサイズ: 128ビット
	- ARX構造（加算、ローテーション、XORのみ）
	- S-boxを使用しない

	参考: https://www.schneier.com/academic/skein/
*/

#ifndef DKC_THREEFISH_INC_
#define DKC_THREEFISH_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/*! @brief Threefish-256のブロックサイズ（バイト） */
#define dkcd_THREEFISH256_BLOCK_SIZE    32

/*! @brief Threefish-512のブロックサイズ（バイト） */
#define dkcd_THREEFISH512_BLOCK_SIZE    64

/*! @brief Threefish-1024のブロックサイズ（バイト） */
#define dkcd_THREEFISH1024_BLOCK_SIZE   128

/*! @brief Tweakサイズ（バイト） */
#define dkcd_THREEFISH_TWEAK_SIZE       16

/*! @brief Threefish-256のラウンド数 */
#define dkcd_THREEFISH256_ROUNDS        72

/*! @brief Threefish-512のラウンド数 */
#define dkcd_THREEFISH512_ROUNDS        72

/*! @brief Threefish-1024のラウンド数 */
#define dkcd_THREEFISH1024_ROUNDS       80

/*! @brief Key schedule parity constant (C240) */
#define dkcd_THREEFISH_PARITY           0x1BD11BDAA9FC1A22ULL

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief Threefishバリアント種別 */
typedef enum {
	edkcThreefish256  = 256,
	edkcThreefish512  = 512,
	edkcThreefish1024 = 1024
} edkcThreefishVariant;

/*! @brief Threefish-256コンテキスト構造体 */
typedef struct dkc_Threefish256{
	uint64 key[5];      /*!< 拡張キー (4 words + parity) */
	uint64 tweak[3];    /*!< 拡張Tweak (2 words + XOR) */
}DKC_THREEFISH256;

/*! @brief Threefish-512コンテキスト構造体 */
typedef struct dkc_Threefish512{
	uint64 key[9];      /*!< 拡張キー (8 words + parity) */
	uint64 tweak[3];    /*!< 拡張Tweak (2 words + XOR) */
}DKC_THREEFISH512;

/*! @brief Threefish-1024コンテキスト構造体 */
typedef struct dkc_Threefish1024{
	uint64 key[17];     /*!< 拡張キー (16 words + parity) */
	uint64 tweak[3];    /*!< 拡張Tweak (2 words + XOR) */
}DKC_THREEFISH1024;

/*! @brief 汎用Threefishコンテキスト構造体 */
typedef struct dkc_Threefish{
	edkcThreefishVariant variant;  /*!< バリアント種別 */
	union {
		DKC_THREEFISH256  tf256;
		DKC_THREEFISH512  tf512;
		DKC_THREEFISH1024 tf1024;
	} ctx;
}DKC_THREEFISH;

/* ====================================================================
 * Threefish-256 関数
 * ==================================================================== */

/*!
	@brief Threefish-256コンテキストを生成する
	@param key 256ビット（32バイト）の鍵
	@param keylen 鍵の長さ（32以上）
	@param tweak 128ビット（16バイト）のTweak（NULLの場合はゼロ）
	@return 生成されたコンテキスト、失敗時はNULL
*/
DKC_EXTERN DKC_THREEFISH256 * WINAPI dkcAllocThreefish256(
	const unsigned char *key, size_t keylen,
	const unsigned char *tweak);

/*!
	@brief Threefish-256コンテキストを解放する
	@param p コンテキストへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeThreefish256(DKC_THREEFISH256 **p);

/*!
	@brief Threefish-256でブロックを暗号化する
	@param p コンテキスト
	@param dest 出力バッファ（32バイト以上）
	@param destsize 出力バッファサイズ
	@param src 入力データ
	@param srcsize 入力データサイズ（32の倍数）
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcThreefish256Encrypt(
	DKC_THREEFISH256 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize);

/*!
	@brief Threefish-256でブロックを復号化する
	@param p コンテキスト
	@param dest 出力バッファ（32バイト以上）
	@param destsize 出力バッファサイズ
	@param src 入力データ
	@param srcsize 入力データサイズ（32の倍数）
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcThreefish256Decrypt(
	DKC_THREEFISH256 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize);

/*!
	@brief Threefish-256でブロックをインプレース暗号化する
	@param p コンテキスト
	@param dest_and_src 入出力バッファ
	@param size データサイズ（32の倍数）
*/
DKC_EXTERN void WINAPI dkcThreefish256EncryptNoDest(
	DKC_THREEFISH256 *p,
	unsigned char *dest_and_src, size_t size);

/*!
	@brief Threefish-256でブロックをインプレース復号化する
	@param p コンテキスト
	@param dest_and_src 入出力バッファ
	@param size データサイズ（32の倍数）
*/
DKC_EXTERN void WINAPI dkcThreefish256DecryptNoDest(
	DKC_THREEFISH256 *p,
	unsigned char *dest_and_src, size_t size);

/*!
	@brief Threefish-256のTweakを更新する
	@param p コンテキスト
	@param tweak 新しいTweak（16バイト）
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcThreefish256SetTweak(
	DKC_THREEFISH256 *p, const unsigned char *tweak);

/* ====================================================================
 * Threefish-512 関数
 * ==================================================================== */

/*!
	@brief Threefish-512コンテキストを生成する
	@param key 512ビット（64バイト）の鍵
	@param keylen 鍵の長さ（64以上）
	@param tweak 128ビット（16バイト）のTweak（NULLの場合はゼロ）
	@return 生成されたコンテキスト、失敗時はNULL
*/
DKC_EXTERN DKC_THREEFISH512 * WINAPI dkcAllocThreefish512(
	const unsigned char *key, size_t keylen,
	const unsigned char *tweak);

/*!
	@brief Threefish-512コンテキストを解放する
	@param p コンテキストへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeThreefish512(DKC_THREEFISH512 **p);

/*!
	@brief Threefish-512でブロックを暗号化する
*/
DKC_EXTERN int WINAPI dkcThreefish512Encrypt(
	DKC_THREEFISH512 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize);

/*!
	@brief Threefish-512でブロックを復号化する
*/
DKC_EXTERN int WINAPI dkcThreefish512Decrypt(
	DKC_THREEFISH512 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize);

/*!
	@brief Threefish-512でブロックをインプレース暗号化する
*/
DKC_EXTERN void WINAPI dkcThreefish512EncryptNoDest(
	DKC_THREEFISH512 *p,
	unsigned char *dest_and_src, size_t size);

/*!
	@brief Threefish-512でブロックをインプレース復号化する
*/
DKC_EXTERN void WINAPI dkcThreefish512DecryptNoDest(
	DKC_THREEFISH512 *p,
	unsigned char *dest_and_src, size_t size);

/*!
	@brief Threefish-512のTweakを更新する
*/
DKC_EXTERN int WINAPI dkcThreefish512SetTweak(
	DKC_THREEFISH512 *p, const unsigned char *tweak);

/* ====================================================================
 * Threefish-1024 関数
 * ==================================================================== */

/*!
	@brief Threefish-1024コンテキストを生成する
	@param key 1024ビット（128バイト）の鍵
	@param keylen 鍵の長さ（128以上）
	@param tweak 128ビット（16バイト）のTweak（NULLの場合はゼロ）
	@return 生成されたコンテキスト、失敗時はNULL
*/
DKC_EXTERN DKC_THREEFISH1024 * WINAPI dkcAllocThreefish1024(
	const unsigned char *key, size_t keylen,
	const unsigned char *tweak);

/*!
	@brief Threefish-1024コンテキストを解放する
	@param p コンテキストへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeThreefish1024(DKC_THREEFISH1024 **p);

/*!
	@brief Threefish-1024でブロックを暗号化する
*/
DKC_EXTERN int WINAPI dkcThreefish1024Encrypt(
	DKC_THREEFISH1024 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize);

/*!
	@brief Threefish-1024でブロックを復号化する
*/
DKC_EXTERN int WINAPI dkcThreefish1024Decrypt(
	DKC_THREEFISH1024 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize);

/*!
	@brief Threefish-1024でブロックをインプレース暗号化する
*/
DKC_EXTERN void WINAPI dkcThreefish1024EncryptNoDest(
	DKC_THREEFISH1024 *p,
	unsigned char *dest_and_src, size_t size);

/*!
	@brief Threefish-1024でブロックをインプレース復号化する
*/
DKC_EXTERN void WINAPI dkcThreefish1024DecryptNoDest(
	DKC_THREEFISH1024 *p,
	unsigned char *dest_and_src, size_t size);

/*!
	@brief Threefish-1024のTweakを更新する
*/
DKC_EXTERN int WINAPI dkcThreefish1024SetTweak(
	DKC_THREEFISH1024 *p, const unsigned char *tweak);

/* ====================================================================
 * 汎用Threefish関数
 * ==================================================================== */

/*!
	@brief 汎用Threefishコンテキストを生成する
	@param variant バリアント（256/512/1024）
	@param key 鍵データ
	@param keylen 鍵の長さ
	@param tweak Tweak（NULLの場合はゼロ）
	@return 生成されたコンテキスト、失敗時はNULL
*/
DKC_EXTERN DKC_THREEFISH * WINAPI dkcAllocThreefish(
	edkcThreefishVariant variant,
	const unsigned char *key, size_t keylen,
	const unsigned char *tweak);

/*!
	@brief 汎用Threefishコンテキストを解放する
*/
DKC_EXTERN int WINAPI dkcFreeThreefish(DKC_THREEFISH **p);

/*!
	@brief 汎用Threefishで暗号化する
*/
DKC_EXTERN int WINAPI dkcThreefishEncrypt(
	DKC_THREEFISH *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize);

/*!
	@brief 汎用Threefishで復号化する
*/
DKC_EXTERN int WINAPI dkcThreefishDecrypt(
	DKC_THREEFISH *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize);

/*!
	@brief 汎用Threefishでインプレース暗号化する
*/
DKC_EXTERN void WINAPI dkcThreefishEncryptNoDest(
	DKC_THREEFISH *p,
	unsigned char *dest_and_src, size_t size);

/*!
	@brief 汎用Threefishでインプレース復号化する
*/
DKC_EXTERN void WINAPI dkcThreefishDecryptNoDest(
	DKC_THREEFISH *p,
	unsigned char *dest_and_src, size_t size);

/*!
	@brief 汎用ThreefishのTweakを更新する
*/
DKC_EXTERN int WINAPI dkcThreefishSetTweak(
	DKC_THREEFISH *p, const unsigned char *tweak);

/*!
	@brief Threefishのブロックサイズを取得する
	@param p コンテキスト
	@return ブロックサイズ（バイト）
*/
DKC_EXTERN size_t WINAPI dkcThreefishGetBlockSize(DKC_THREEFISH *p);

#ifdef __cplusplus
}
#endif

#endif /* DKC_THREEFISH_INC_ */
