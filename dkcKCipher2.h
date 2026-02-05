/*! @file
	@brief KCipher-2 Stream Cipher
	@author threeBrane
	@note
	KCipher-2はKDDI研究所が開発したストリーム暗号で、
	ISO/IEC 18033-4で標準化されています。

	特徴:
	- 128ビットキー
	- 128ビットIV (初期化ベクトル)
	- 2つのフィードバックシフトレジスタ (FSR-A, FSR-B)
	- 高速かつ安全

	参考: RFC 7008 - A Description of the KCipher-2 Encryption Algorithm
*/

#ifndef DKC_KCIPHER2_INC_
#define DKC_KCIPHER2_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/*! @brief キーサイズ（バイト） */
#define dkcd_KCIPHER2_KEY_SIZE      16

/*! @brief IVサイズ（バイト） */
#define dkcd_KCIPHER2_IV_SIZE       16

/*! @brief 内部レジスタサイズ */
#define dkcd_KCIPHER2_FSR_A_SIZE    5
#define dkcd_KCIPHER2_FSR_B_SIZE    11

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief KCipher-2コンテキスト構造体 */
typedef struct dkc_KCipher2{
	uint32 A[dkcd_KCIPHER2_FSR_A_SIZE];   /*!< FSR-A レジスタ */
	uint32 B[dkcd_KCIPHER2_FSR_B_SIZE];   /*!< FSR-B レジスタ */
	uint32 L1, L2;                         /*!< 内部レジスタ L1, L2 */
	uint32 R1, R2;                         /*!< 内部レジスタ R1, R2 */

	/*! @brief バイト単位出力バッファ */
	union {
		uint64 a64[4];
		uint32 a32[8];
		uint16 a16[16];
		uint8  a8[32];
	} u_store;

	uint8 store_size;                      /*!< バッファ内の残りバイト数 */
}DKC_KCIPHER2;

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

/*!
	@brief KCipher-2コンテキストを生成する
	@param key 128ビット（16バイト）のキー
	@param keysize キーサイズ（16でなければならない）
	@param iv 128ビット（16バイト）のIV
	@param ivsize IVサイズ（16でなければならない）
	@return 生成されたコンテキスト、失敗時はNULL
*/
DKC_EXTERN DKC_KCIPHER2 * WINAPI dkcAllocKCipher2(
	const unsigned char *key, size_t keysize,
	const unsigned char *iv, size_t ivsize);

/*!
	@brief KCipher-2コンテキストを初期化する（既存コンテキストを再利用）
	@param p コンテキスト
	@param key 128ビット（16バイト）のキー
	@param keysize キーサイズ
	@param iv 128ビット（16バイト）のIV
	@param ivsize IVサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcKCipher2Init(
	DKC_KCIPHER2 *p,
	const unsigned char *key, size_t keysize,
	const unsigned char *iv, size_t ivsize);

/*!
	@brief KCipher-2コンテキストを解放する
	@param p コンテキストへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeKCipher2(DKC_KCIPHER2 **p);

/*!
	@brief 1バイトのキーストリームを生成する
	@param p コンテキスト
	@return 生成されたキーストリームバイト
*/
DKC_EXTERN uint8 WINAPI dkcKCipher2Process(DKC_KCIPHER2 *p);

/*! @brief dkcKCipher2Processの別名 */
#define dkcKCipher2_Byte(p) dkcKCipher2Process(p)

/*!
	@brief データを暗号化/復号化する（出力先指定）
	@param p コンテキスト
	@param dest 出力バッファ
	@param dsize 出力バッファサイズ
	@param src 入力データ
	@param ssize 入力データサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
	@note ストリーム暗号なので暗号化と復号化は同じ操作
*/
DKC_EXTERN int WINAPI dkcKCipher2Encrypt(
	DKC_KCIPHER2 *p,
	unsigned char *dest, size_t dsize,
	const unsigned char *src, size_t ssize);

/*! @brief 復号化（暗号化と同じ） */
#define dkcKCipher2Decrypt(a,b,c,d,e) dkcKCipher2Encrypt(a,b,c,d,e)

/*!
	@brief データをインプレースで暗号化/復号化する
	@param p コンテキスト
	@param inout 入出力バッファ
	@param size データサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcKCipher2EncryptNoDest(
	DKC_KCIPHER2 *p,
	unsigned char *inout, size_t size);

/*! @brief インプレース復号化（暗号化と同じ） */
#define dkcKCipher2DecryptNoDest(a,b,c) dkcKCipher2EncryptNoDest(a,b,c)

/*!
	@brief 32ビットのキーストリームワードを生成する
	@param p コンテキスト
	@param high 上位32ビット出力先
	@param low 下位32ビット出力先
	@note 1回の呼び出しで64ビット（8バイト）を生成
*/
DKC_EXTERN void WINAPI dkcKCipher2Next(
	DKC_KCIPHER2 *p,
	uint32 *high, uint32 *low);

#ifdef __cplusplus
}
#endif

#endif /* DKC_KCIPHER2_INC_ */
