/*! @file
	@brief ユニークID生成ライブラリ
	@author d.Kingyo
	@note
	UUID (RFC 9562) および各種ユニークID形式の実装

	対応形式:
	- UUID v1-v8
	- ULID
	- Snowflake
	- KSUID
	- XID
	- NanoID
	- CUID/CUID2
*/

#ifndef DKC_UNIQUEID_INC_
#define DKC_UNIQUEID_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * UUID 構造体・定数
 * ==================================================================== */

#define dkcd_UUID_SIZE          16
#define dkcd_UUID_STRING_SIZE   36

typedef struct dkc_UUID {
	uint8 bytes[dkcd_UUID_SIZE];
} DKC_UUID;

typedef enum {
	edkcUUID_V1 = 1, edkcUUID_V2 = 2, edkcUUID_V3 = 3, edkcUUID_V4 = 4,
	edkcUUID_V5 = 5, edkcUUID_V6 = 6, edkcUUID_V7 = 7, edkcUUID_V8 = 8
} edkcUUIDVersion;

typedef enum {
	edkcUUID_NS_DNS = 0, edkcUUID_NS_URL = 1,
	edkcUUID_NS_OID = 2, edkcUUID_NS_X500 = 3
} edkcUUIDNamespace;

typedef enum {
	edkcUUID_DOMAIN_PERSON = 0, edkcUUID_DOMAIN_GROUP = 1, edkcUUID_DOMAIN_ORG = 2
} edkcUUIDLocalDomain;

/*! @brief UUIDジェネレータ */
typedef struct dkc_UUIDGenerator {
	uint32 rand_state;      /*!< 乱数状態 */
	uint16 clock_seq;       /*!< クロックシーケンス */
	uint8 node_id[6];       /*!< ノードID */
	int initialized;        /*!< 初期化済みフラグ */
} DKC_UUID_GEN;

/* ====================================================================
 * ULID 構造体・定数
 * ==================================================================== */

#define dkcd_ULID_SIZE          16
#define dkcd_ULID_STRING_SIZE   26

typedef struct dkc_ULID {
	uint8 bytes[dkcd_ULID_SIZE];
} DKC_ULID;

/*! @brief ULIDジェネレータ */
typedef struct dkc_ULIDGenerator {
	uint32 rand_state;      /*!< 乱数状態 */
	uint64 last_timestamp;  /*!< 最後のタイムスタンプ */
	uint8 last_random[10];  /*!< 最後のランダム部分（単調増加用） */
} DKC_ULID_GEN;

/* ====================================================================
 * Snowflake 構造体・定数
 * ==================================================================== */

#define dkcd_SNOWFLAKE_SIZE     8

typedef struct dkc_Snowflake {
	uint64 id;
} DKC_SNOWFLAKE;

typedef struct dkc_SnowflakeGenerator {
	uint32 rand_state;      /*!< 乱数状態 */
	uint64 epoch;
	uint16 worker_id;
	uint16 datacenter_id;
	uint16 sequence;
	uint64 last_timestamp;
} DKC_SNOWFLAKE_GEN;

/* ====================================================================
 * KSUID 構造体・定数
 * ==================================================================== */

#define dkcd_KSUID_SIZE         20
#define dkcd_KSUID_STRING_SIZE  27
#define dkcd_KSUID_EPOCH        1400000000

typedef struct dkc_KSUID {
	uint8 bytes[dkcd_KSUID_SIZE];
} DKC_KSUID;

typedef struct dkc_KSUIDGenerator {
	uint32 rand_state;
} DKC_KSUID_GEN;

/* ====================================================================
 * XID 構造体・定数
 * ==================================================================== */

#define dkcd_XID_SIZE           12
#define dkcd_XID_STRING_SIZE    20

typedef struct dkc_XID {
	uint8 bytes[dkcd_XID_SIZE];
} DKC_XID;

typedef struct dkc_XIDGenerator {
	uint32 rand_state;
	uint8 machine_id[3];
	uint16 pid;
	uint32 counter;
} DKC_XID_GEN;

/* ====================================================================
 * NanoID 定数・構造体
 * ==================================================================== */

#define dkcd_NANOID_DEFAULT_SIZE    21
#define dkcd_NANOID_DEFAULT_ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-"

typedef struct dkc_NanoIDGenerator {
	uint32 rand_state;
} DKC_NANOID_GEN;

/* ====================================================================
 * CUID 定数・構造体
 * ==================================================================== */

#define dkcd_CUID_SIZE          25
#define dkcd_CUID2_DEFAULT_SIZE 24

typedef struct dkc_CUIDGenerator {
	uint32 rand_state;
	uint32 counter;
} DKC_CUID_GEN;

/* ====================================================================
 * UUID 関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcUUIDGenInit(DKC_UUID_GEN *gen, uint32 seed);

DKC_EXTERN int WINAPI dkcUUIDv1Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid);
DKC_EXTERN int WINAPI dkcUUIDv2Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid,
	edkcUUIDLocalDomain domain, uint32 id);
DKC_EXTERN int WINAPI dkcUUIDv3Generate(DKC_UUID *uuid,
	const DKC_UUID *ns_uuid, const char *name, size_t name_len);
DKC_EXTERN int WINAPI dkcUUIDv3GenerateNS(DKC_UUID *uuid,
	edkcUUIDNamespace ns, const char *name, size_t name_len);
DKC_EXTERN int WINAPI dkcUUIDv4Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid);
DKC_EXTERN int WINAPI dkcUUIDv5Generate(DKC_UUID *uuid,
	const DKC_UUID *ns_uuid, const char *name, size_t name_len);
DKC_EXTERN int WINAPI dkcUUIDv5GenerateNS(DKC_UUID *uuid,
	edkcUUIDNamespace ns, const char *name, size_t name_len);
DKC_EXTERN int WINAPI dkcUUIDv6Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid);
DKC_EXTERN int WINAPI dkcUUIDv7Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid);
DKC_EXTERN int WINAPI dkcUUIDv8Generate(DKC_UUID *uuid,
	uint64 custom_a, uint16 custom_b, uint64 custom_c);

DKC_EXTERN int WINAPI dkcUUIDToString(const DKC_UUID *uuid, char *str);
DKC_EXTERN int WINAPI dkcUUIDFromString(const char *str, DKC_UUID *uuid);
DKC_EXTERN int WINAPI dkcUUIDGetVersion(const DKC_UUID *uuid);
DKC_EXTERN int WINAPI dkcUUIDCompare(const DKC_UUID *a, const DKC_UUID *b);
DKC_EXTERN BOOL WINAPI dkcUUIDIsNil(const DKC_UUID *uuid);
DKC_EXTERN void WINAPI dkcUUIDNil(DKC_UUID *uuid);
DKC_EXTERN int WINAPI dkcUUIDGetNamespace(edkcUUIDNamespace ns, DKC_UUID *uuid);

/* ====================================================================
 * ULID 関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcULIDGenInit(DKC_ULID_GEN *gen, uint32 seed);
DKC_EXTERN int WINAPI dkcULIDGenerate(DKC_ULID_GEN *gen, DKC_ULID *ulid);
DKC_EXTERN int WINAPI dkcULIDGenerateWithTime(DKC_ULID_GEN *gen, DKC_ULID *ulid, uint64 timestamp_ms);
DKC_EXTERN int WINAPI dkcULIDToString(const DKC_ULID *ulid, char *str);
DKC_EXTERN int WINAPI dkcULIDFromString(const char *str, DKC_ULID *ulid);
DKC_EXTERN uint64 WINAPI dkcULIDGetTimestamp(const DKC_ULID *ulid);
DKC_EXTERN int WINAPI dkcULIDCompare(const DKC_ULID *a, const DKC_ULID *b);

/* ====================================================================
 * Snowflake 関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcSnowflakeInit(DKC_SNOWFLAKE_GEN *gen,
	uint16 worker_id, uint16 datacenter_id, uint64 epoch, uint32 seed);
DKC_EXTERN int WINAPI dkcSnowflakeGenerate(DKC_SNOWFLAKE_GEN *gen, DKC_SNOWFLAKE *sf);
DKC_EXTERN int WINAPI dkcSnowflakeToString(const DKC_SNOWFLAKE *sf, char *str);
DKC_EXTERN int WINAPI dkcSnowflakeFromString(const char *str, DKC_SNOWFLAKE *sf);
DKC_EXTERN uint64 WINAPI dkcSnowflakeGetTimestamp(const DKC_SNOWFLAKE *sf, uint64 epoch);

/* ====================================================================
 * KSUID 関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcKSUIDGenInit(DKC_KSUID_GEN *gen, uint32 seed);
DKC_EXTERN int WINAPI dkcKSUIDGenerate(DKC_KSUID_GEN *gen, DKC_KSUID *ksuid);
DKC_EXTERN int WINAPI dkcKSUIDGenerateWithTime(DKC_KSUID_GEN *gen, DKC_KSUID *ksuid, uint32 timestamp);
DKC_EXTERN int WINAPI dkcKSUIDToString(const DKC_KSUID *ksuid, char *str);
DKC_EXTERN int WINAPI dkcKSUIDFromString(const char *str, DKC_KSUID *ksuid);
DKC_EXTERN uint32 WINAPI dkcKSUIDGetTimestamp(const DKC_KSUID *ksuid);
DKC_EXTERN int WINAPI dkcKSUIDCompare(const DKC_KSUID *a, const DKC_KSUID *b);

/* ====================================================================
 * XID 関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcXIDInit(DKC_XID_GEN *gen, uint32 seed);
DKC_EXTERN int WINAPI dkcXIDGenerate(DKC_XID_GEN *gen, DKC_XID *xid);
DKC_EXTERN int WINAPI dkcXIDToString(const DKC_XID *xid, char *str);
DKC_EXTERN int WINAPI dkcXIDFromString(const char *str, DKC_XID *xid);
DKC_EXTERN uint32 WINAPI dkcXIDGetTimestamp(const DKC_XID *xid);
DKC_EXTERN int WINAPI dkcXIDCompare(const DKC_XID *a, const DKC_XID *b);

/* ====================================================================
 * NanoID 関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcNanoIDGenInit(DKC_NANOID_GEN *gen, uint32 seed);
DKC_EXTERN int WINAPI dkcNanoIDGenerate(DKC_NANOID_GEN *gen, char *str);
DKC_EXTERN int WINAPI dkcNanoIDGenerateCustom(DKC_NANOID_GEN *gen, char *str, size_t size, const char *alphabet);

/* ====================================================================
 * CUID 関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcCUIDGenInit(DKC_CUID_GEN *gen, uint32 seed);
DKC_EXTERN int WINAPI dkcCUIDGenerate(DKC_CUID_GEN *gen, char *str);
DKC_EXTERN int WINAPI dkcCUID2Generate(DKC_CUID_GEN *gen, char *str);
DKC_EXTERN int WINAPI dkcCUID2GenerateCustom(DKC_CUID_GEN *gen, char *str, size_t length);

/* ====================================================================
 * ユーティリティ関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcUniqueIDRandomBytes(uint32 *rand_state, void *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* DKC_UNIQUEID_INC_ */
