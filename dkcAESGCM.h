/*! @file
	@brief AES-GCM (Galois/Counter Mode) 認証付き暗号
	@author d.Kingyo
	@note
	AES-GCMはNIST SP 800-38Dで標準化された認証付き暗号モードです。

	特徴:
	- 認証付き暗号化（AEAD）
	- 128/192/256ビットAESキー
	- 96ビット推奨IV
	- 128ビット認証タグ
	- AAD（追加認証データ）サポート

	参考: NIST SP 800-38D - Recommendation for GCM Mode
*/

#ifndef DKC_AESGCM_INC_
#define DKC_AESGCM_INC_

#include "dkcOSIndependent.h"
#include "dkcRijndael.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/*! @brief タグサイズ（バイト） */
#define dkcd_AESGCM_TAG_SIZE    16

/*! @brief ブロックサイズ（バイト） */
#define dkcd_AESGCM_BLOCK_SIZE  16

/*! @brief 推奨IVサイズ（バイト） */
#define dkcd_AESGCM_IV_SIZE     12

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief AES-GCMコンテキスト構造体 */
typedef struct dkc_AESGCM{
	DKC_RIJNDAEL *aes;         /*!< AESコンテキスト */
	uint8 H[16];               /*!< GHASHキー = AES(K, 0^128) */
}DKC_AESGCM;

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

/*!
	@brief AES-GCMコンテキストを生成する
	@param key AESキー（16/24/32バイト）
	@param keysize キーサイズ（バイト）
	@return 生成されたコンテキスト、失敗時はNULL
*/
DKC_EXTERN DKC_AESGCM * WINAPI dkcAllocAESGCM(
	const unsigned char *key, size_t keysize);

/*!
	@brief AES-GCMコンテキストを解放する
	@param p コンテキストへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeAESGCM(DKC_AESGCM **p);

/*!
	@brief AES-GCMで暗号化する
	@param p コンテキスト
	@param iv IV（12バイト推奨）
	@param ivlen IVサイズ
	@param aad 追加認証データ（NULLも可）
	@param aadlen AADサイズ
	@param pt 平文
	@param ptlen 平文サイズ
	@param ct 暗号文出力（ptlenバイト以上）
	@param tag 認証タグ出力（16バイト）
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcAESGCMEncrypt(
	DKC_AESGCM *p,
	const unsigned char *iv, size_t ivlen,
	const unsigned char *aad, size_t aadlen,
	const unsigned char *pt, size_t ptlen,
	unsigned char *ct, unsigned char *tag);

/*!
	@brief AES-GCMで復号化する（タグ検証付き）
	@param p コンテキスト
	@param iv IV
	@param ivlen IVサイズ
	@param aad 追加認証データ（NULLも可）
	@param aadlen AADサイズ
	@param ct 暗号文
	@param ctlen 暗号文サイズ
	@param tag 認証タグ（16バイト）
	@param pt 平文出力（ctlenバイト以上）
	@return edk_SUCCEEDED:成功（タグ一致） edk_FAILED:失敗（タグ不一致含む）
*/
DKC_EXTERN int WINAPI dkcAESGCMDecrypt(
	DKC_AESGCM *p,
	const unsigned char *iv, size_t ivlen,
	const unsigned char *aad, size_t aadlen,
	const unsigned char *ct, size_t ctlen,
	const unsigned char *tag,
	unsigned char *pt);

#ifdef __cplusplus
}
#endif

#endif /* DKC_AESGCM_INC_ */
