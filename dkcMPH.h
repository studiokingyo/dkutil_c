/*! @file
	@brief 最小完全ハッシュ（Minimal Perfect Hash）ライブラリ
	@author d.Kingyo
	@note
	CHD (Compress, Hash, Displace) アルゴリズムによる実装

	参考文献:
	- Belazzougui, Botelho, Dietzfelbinger (2009)
	  "Hash, Displace, and Compress"

	特徴:
	- n個のキーを0〜n-1の整数に一対一マッピング
	- 衝突なし（Perfect Hash）
	- 空きスロットなし（Minimal）
	- O(1)ルックアップ
	- 約2.07 bits/key のメモリ効率
*/

#ifndef DKC_MPH_INC_
#define DKC_MPH_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数
 * ==================================================================== */

#define dkcd_MPH_MAX_KEYS       (1 << 24)   /*!< 最大キー数 (16M) */
#define dkcd_MPH_DEFAULT_LAMBDA 5           /*!< デフォルト負荷係数 */

/* ====================================================================
 * 構造体
 * ==================================================================== */

/*!
	@brief 最小完全ハッシュ構造体
	@note CHD (Compress, Hash, Displace) アルゴリズムを使用
*/
typedef struct dkc_MPH {
	uint32 *g;              /*!< 変位配列 (displacement array) */
	size_t g_size;          /*!< 変位配列サイズ (バケット数) */
	size_t num_keys;        /*!< キー数 */
	uint32 seed1;           /*!< ハッシュシード1 (バケット選択用) */
	uint32 seed2;           /*!< ハッシュシード2 (スロット選択用) */
	int lambda;             /*!< 負荷係数 (バケットあたり平均キー数) */
} DKC_MPH;

/* ====================================================================
 * コールバック型
 * ==================================================================== */

/*!
	@brief キー取得コールバック関数型
	@param context  ユーザーコンテキスト
	@param index    キーのインデックス (0〜num_keys-1)
	@param key      [out] キーデータへのポインタを格納
	@param key_len  [out] キー長を格納
	@return edk_SUCCEEDED または edk_FAILED
	@note 大量のキーをストリーミング処理する場合に使用
*/
typedef int (WINAPI *dkcMPHKeyFunc)(void *context, size_t index,
	const void **key, size_t *key_len);

/* ====================================================================
 * 基本関数
 * ==================================================================== */

/*!
	@brief MPH構造体を初期化
	@param mph MPH構造体へのポインタ
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcMPHInit(DKC_MPH *mph);

/*!
	@brief MPH構造体のリソースを解放
	@param mph MPH構造体へのポインタ
*/
DKC_EXTERN void WINAPI dkcMPHFree(DKC_MPH *mph);

/* ====================================================================
 * 構築関数
 * ==================================================================== */

/*!
	@brief 配列からMPHを構築
	@param mph       MPH構造体
	@param keys      キー配列 (各要素はvoid*ポインタ)
	@param key_lens  各キーの長さ配列
	@param num_keys  キー数
	@return edk_SUCCEEDED または edk_FAILED
	@note CHDアルゴリズムで構築。O(n)時間。
*/
DKC_EXTERN int WINAPI dkcMPHBuild(DKC_MPH *mph,
	const void **keys, const size_t *key_lens, size_t num_keys);

/*!
	@brief 文字列配列からMPHを構築
	@param mph         MPH構造体
	@param strings     NULL終端文字列の配列
	@param num_strings 文字列数
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcMPHBuildStrings(DKC_MPH *mph,
	const char **strings, size_t num_strings);

/*!
	@brief コールバックでMPHを構築
	@param mph       MPH構造体
	@param key_func  キー取得コールバック関数
	@param context   コールバックに渡すユーザーコンテキスト
	@param num_keys  キー数
	@return edk_SUCCEEDED または edk_FAILED
	@note 大量のキーをメモリに保持せずに構築可能
*/
DKC_EXTERN int WINAPI dkcMPHBuildCallback(DKC_MPH *mph,
	dkcMPHKeyFunc key_func, void *context, size_t num_keys);

/* ====================================================================
 * ルックアップ関数
 * ==================================================================== */

/*!
	@brief キーのインデックスを取得
	@param mph      MPH構造体
	@param key      検索キー
	@param key_len  キー長
	@return 0〜num_keys-1 のインデックス
	@warning キーが構築時のセットに含まれていることが前提。
	         存在しないキーを渡した場合の結果は未定義。
	@note O(1)時間
*/
DKC_EXTERN size_t WINAPI dkcMPHLookup(const DKC_MPH *mph,
	const void *key, size_t key_len);

/*!
	@brief 文字列キーのインデックスを取得
	@param mph  MPH構造体
	@param str  NULL終端文字列
	@return 0〜num_keys-1 のインデックス
*/
DKC_EXTERN size_t WINAPI dkcMPHLookupString(const DKC_MPH *mph,
	const char *str);

/* ====================================================================
 * シリアライズ（永続化用）
 * ==================================================================== */

/*!
	@brief シリアライズに必要なバッファサイズを取得
	@param mph MPH構造体
	@return 必要なバイト数
*/
DKC_EXTERN size_t WINAPI dkcMPHSerializedSize(const DKC_MPH *mph);

/*!
	@brief MPHをバッファにシリアライズ
	@param mph         MPH構造体
	@param buffer      出力バッファ
	@param buffer_size バッファサイズ
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcMPHSerialize(const DKC_MPH *mph,
	void *buffer, size_t buffer_size);

/*!
	@brief バッファからMPHをデシリアライズ
	@param mph         MPH構造体（出力）
	@param buffer      入力バッファ
	@param buffer_size バッファサイズ
	@return edk_SUCCEEDED または edk_FAILED
	@note デシリアライズ後はdkcMPHFree()で解放が必要
*/
DKC_EXTERN int WINAPI dkcMPHDeserialize(DKC_MPH *mph,
	const void *buffer, size_t buffer_size);

/* ====================================================================
 * ユーティリティ
 * ==================================================================== */

/*!
	@brief メモリ使用量を取得
	@param mph MPH構造体
	@return 使用メモリ量（バイト）
*/
DKC_EXTERN size_t WINAPI dkcMPHMemoryUsage(const DKC_MPH *mph);

/*!
	@brief bits per keyを取得
	@param mph MPH構造体
	@return キーあたりのビット数（効率の指標、理論下限は約1.44）
	@note CHDは通常2.0〜2.5程度
*/
DKC_EXTERN double WINAPI dkcMPHBitsPerKey(const DKC_MPH *mph);

#ifdef __cplusplus
}
#endif

#endif /* DKC_MPH_INC_ */
