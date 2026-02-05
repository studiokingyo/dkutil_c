/*! @file
	@brief Bloom Filter（ブルームフィルタ）ライブラリ
	@author d.Kingyo
	@note
	確率的データ構造によるメンバーシップテスト

	特徴:
	- 要素の存在確認が O(k) （kはハッシュ関数数）
	- False Positive（偽陽性）の可能性あり
	- False Negative（偽陰性）なし
	- 削除不可（Counting Bloom Filterで対応）
	- 省メモリ

	用途:
	- キャッシュフィルタリング
	- スペルチェッカー
	- ネットワークパケットフィルタ
	- データベースクエリ最適化
*/

#ifndef DKC_BLOOMFILTER_INC_
#define DKC_BLOOMFILTER_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数
 * ==================================================================== */

#define dkcd_BLOOM_MAX_HASHES   16      /*!< 最大ハッシュ関数数 */
#define dkcd_BLOOM_DEFAULT_FPR  0.01    /*!< デフォルト偽陽性率 (1%) */

/* ====================================================================
 * 構造体
 * ==================================================================== */

/*!
	@brief Bloom Filter構造体
*/
typedef struct dkc_BloomFilter {
	uint8 *bits;            /*!< ビット配列 */
	size_t num_bits;        /*!< ビット数 (m) */
	size_t num_bytes;       /*!< バイト数 */
	uint32 num_hashes;      /*!< ハッシュ関数数 (k) */
	size_t num_items;       /*!< 追加済み要素数 */
	size_t capacity;        /*!< 想定最大要素数 (n) */
	uint32 seed1;           /*!< ハッシュシード1 */
	uint32 seed2;           /*!< ハッシュシード2 */
} DKC_BLOOM;

/*!
	@brief Counting Bloom Filter構造体
	@note 要素の削除が可能
*/
typedef struct dkc_CountingBloomFilter {
	uint8 *counters;        /*!< カウンタ配列 (4bit per counter) */
	size_t num_counters;    /*!< カウンタ数 (m) */
	size_t num_bytes;       /*!< バイト数 */
	uint32 num_hashes;      /*!< ハッシュ関数数 (k) */
	size_t num_items;       /*!< 追加済み要素数 */
	size_t capacity;        /*!< 想定最大要素数 */
	uint32 seed1;           /*!< ハッシュシード1 */
	uint32 seed2;           /*!< ハッシュシード2 */
} DKC_COUNTING_BLOOM;

/* ====================================================================
 * Bloom Filter 基本関数
 * ==================================================================== */

/*!
	@brief 要素数と偽陽性率からBloom Filterを作成
	@param bloom    Bloom Filter構造体
	@param capacity 想定最大要素数
	@param fpr      偽陽性率 (0.0〜1.0、例: 0.01 = 1%)
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcBloomCreate(DKC_BLOOM *bloom, size_t capacity, double fpr);

/*!
	@brief ビット数とハッシュ数を直接指定してBloom Filterを作成
	@param bloom      Bloom Filter構造体
	@param num_bits   ビット数 (m)
	@param num_hashes ハッシュ関数数 (k)
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcBloomCreateDirect(DKC_BLOOM *bloom,
	size_t num_bits, uint32 num_hashes);

/*!
	@brief Bloom Filterを解放
	@param bloom Bloom Filter構造体
*/
DKC_EXTERN void WINAPI dkcBloomFree(DKC_BLOOM *bloom);

/*!
	@brief 要素を追加
	@param bloom Bloom Filter構造体
	@param data  データ
	@param len   データ長
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcBloomAdd(DKC_BLOOM *bloom, const void *data, size_t len);

/*!
	@brief 文字列を追加
*/
DKC_EXTERN int WINAPI dkcBloomAddString(DKC_BLOOM *bloom, const char *str);

/*!
	@brief 要素の存在確認
	@param bloom Bloom Filter構造体
	@param data  データ
	@param len   データ長
	@return TRUE: 存在する可能性あり, FALSE: 確実に存在しない
	@note TRUEでも偽陽性の可能性あり
*/
DKC_EXTERN BOOL WINAPI dkcBloomContains(const DKC_BLOOM *bloom,
	const void *data, size_t len);

/*!
	@brief 文字列の存在確認
*/
DKC_EXTERN BOOL WINAPI dkcBloomContainsString(const DKC_BLOOM *bloom, const char *str);

/*!
	@brief Bloom Filterをクリア
*/
DKC_EXTERN void WINAPI dkcBloomClear(DKC_BLOOM *bloom);

/*!
	@brief 2つのBloom Filterをマージ（OR演算）
	@param dest マージ先（結果格納）
	@param src  マージ元
	@return edk_SUCCEEDED または edk_FAILED
	@note 同じパラメータ（ビット数、ハッシュ数）である必要あり
*/
DKC_EXTERN int WINAPI dkcBloomMerge(DKC_BLOOM *dest, const DKC_BLOOM *src);

/* ====================================================================
 * Bloom Filter 情報取得
 * ==================================================================== */

/*!
	@brief 現在の推定偽陽性率を取得
*/
DKC_EXTERN double WINAPI dkcBloomGetFPR(const DKC_BLOOM *bloom);

/*!
	@brief ビット使用率を取得 (0.0〜1.0)
*/
DKC_EXTERN double WINAPI dkcBloomGetFillRatio(const DKC_BLOOM *bloom);

/*!
	@brief メモリ使用量を取得（バイト）
*/
DKC_EXTERN size_t WINAPI dkcBloomMemoryUsage(const DKC_BLOOM *bloom);

/* ====================================================================
 * Bloom Filter シリアライズ
 * ==================================================================== */

/*!
	@brief シリアライズに必要なバッファサイズを取得
*/
DKC_EXTERN size_t WINAPI dkcBloomSerializedSize(const DKC_BLOOM *bloom);

/*!
	@brief Bloom Filterをシリアライズ
*/
DKC_EXTERN int WINAPI dkcBloomSerialize(const DKC_BLOOM *bloom,
	void *buffer, size_t buffer_size);

/*!
	@brief Bloom Filterをデシリアライズ
*/
DKC_EXTERN int WINAPI dkcBloomDeserialize(DKC_BLOOM *bloom,
	const void *buffer, size_t buffer_size);

/* ====================================================================
 * Counting Bloom Filter 関数
 * ==================================================================== */

/*!
	@brief Counting Bloom Filterを作成
*/
DKC_EXTERN int WINAPI dkcCountingBloomCreate(DKC_COUNTING_BLOOM *bloom,
	size_t capacity, double fpr);

/*!
	@brief Counting Bloom Filterを解放
*/
DKC_EXTERN void WINAPI dkcCountingBloomFree(DKC_COUNTING_BLOOM *bloom);

/*!
	@brief 要素を追加
*/
DKC_EXTERN int WINAPI dkcCountingBloomAdd(DKC_COUNTING_BLOOM *bloom,
	const void *data, size_t len);

/*!
	@brief 要素を削除
	@return edk_SUCCEEDED または edk_FAILED（カウンタ0の場合）
*/
DKC_EXTERN int WINAPI dkcCountingBloomRemove(DKC_COUNTING_BLOOM *bloom,
	const void *data, size_t len);

/*!
	@brief 要素の存在確認
*/
DKC_EXTERN BOOL WINAPI dkcCountingBloomContains(const DKC_COUNTING_BLOOM *bloom,
	const void *data, size_t len);

/*!
	@brief Counting Bloom Filterをクリア
*/
DKC_EXTERN void WINAPI dkcCountingBloomClear(DKC_COUNTING_BLOOM *bloom);

/* ====================================================================
 * ユーティリティ
 * ==================================================================== */

/*!
	@brief 最適なビット数を計算
	@param n   想定要素数
	@param fpr 偽陽性率
	@return 最適なビット数 (m)
*/
DKC_EXTERN size_t WINAPI dkcBloomOptimalBits(size_t n, double fpr);

/*!
	@brief 最適なハッシュ関数数を計算
	@param m ビット数
	@param n 想定要素数
	@return 最適なハッシュ関数数 (k)
*/
DKC_EXTERN uint32 WINAPI dkcBloomOptimalHashes(size_t m, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* DKC_BLOOMFILTER_INC_ */
