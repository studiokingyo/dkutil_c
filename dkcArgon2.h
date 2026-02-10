/*! @file
	@brief Argon2 パスワードハッシュ
	@author d.Kingyo
	@note
	Argon2はBiryukov, Dinu, Khovratovich (2015)が設計した
	メモリハードパスワードハッシュ関数で、
	RFC 9106で標準化されています。

	バリアント:
	- Argon2d: データ依存メモリアクセス（サイドチャネル耐性なし）
	- Argon2i: データ独立メモリアクセス（サイドチャネル耐性あり）
	- Argon2id: ハイブリッド（推奨）

	参考: RFC 9106 - Argon2 Memory-Hard Function
*/

#ifndef DKC_ARGON2_INC_
#define DKC_ARGON2_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/*! @brief Argon2バリアント */
#define dkcd_ARGON2D    0
#define dkcd_ARGON2I    1
#define dkcd_ARGON2ID   2

/*! @brief デフォルトパラメータ */
#define dkcd_ARGON2_DEFAULT_T_COST      3
#define dkcd_ARGON2_DEFAULT_M_COST      65536  /* 64 MiB */
#define dkcd_ARGON2_DEFAULT_PARALLELISM 1

/*! @brief ブロックサイズ (1024バイト) */
#define dkcd_ARGON2_BLOCK_SIZE  1024

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

/*!
	@brief Argon2パスワードハッシュ
	@param type バリアント（dkcd_ARGON2D/dkcd_ARGON2I/dkcd_ARGON2ID）
	@param t_cost 反復回数（1以上）
	@param m_cost メモリコスト（KiB単位、8以上）
	@param parallelism 並列度（1固定、シングルスレッド実装）
	@param pwd パスワード
	@param pwdlen パスワード長
	@param salt ソルト
	@param saltlen ソルト長（8バイト以上推奨）
	@param hash ハッシュ出力バッファ
	@param hashlen ハッシュ長（4バイト以上）
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcArgon2Hash(
	int type, uint32 t_cost, uint32 m_cost, uint32 parallelism,
	const unsigned char *pwd, size_t pwdlen,
	const unsigned char *salt, size_t saltlen,
	unsigned char *hash, size_t hashlen);

#ifdef __cplusplus
}
#endif

#endif /* DKC_ARGON2_INC_ */
