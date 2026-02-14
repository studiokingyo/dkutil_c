/*! @file
	@brief ユニークID生成ライブラリ 実装
	@author d.Kingyo
*/

#include "dkcUniqueID.h"
#include "dkcMD5.h"
#include "dkcSHA1.h"
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

/* ====================================================================
 * 内部定数
 * ==================================================================== */

static const uint8 UUID_NS_DNS[16] = {
	0x6b,0xa7,0xb8,0x10,0x9d,0xad,0x11,0xd1,
	0x80,0xb4,0x00,0xc0,0x4f,0xd4,0x30,0xc8
};
static const uint8 UUID_NS_URL[16] = {
	0x6b,0xa7,0xb8,0x11,0x9d,0xad,0x11,0xd1,
	0x80,0xb4,0x00,0xc0,0x4f,0xd4,0x30,0xc8
};
static const uint8 UUID_NS_OID[16] = {
	0x6b,0xa7,0xb8,0x12,0x9d,0xad,0x11,0xd1,
	0x80,0xb4,0x00,0xc0,0x4f,0xd4,0x30,0xc8
};
static const uint8 UUID_NS_X500[16] = {
	0x6b,0xa7,0xb8,0x14,0x9d,0xad,0x11,0xd1,
	0x80,0xb4,0x00,0xc0,0x4f,0xd4,0x30,0xc8
};

static const char CROCKFORD_BASE32[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
static const char BASE62[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const char XID_BASE32[] = "0123456789abcdefghijklmnopqrstuv";

#ifdef _MSC_VER
#define TWITTER_EPOCH 1288834974657ui64
#else
#define TWITTER_EPOCH 1288834974657ULL
#endif

/* ====================================================================
 * 内部関数
 * ==================================================================== */

static DKC_INLINE uint32 xorshift32(uint32 *state)
{
	uint32 x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;
	return x;
}

static uint64 get_timestamp_ms(void)
{
#ifdef _WIN32
	FILETIME ft;
	ULARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
#ifdef _MSC_VER
	return (li.QuadPart - 116444736000000000i64) / 10000;
#else
	return (li.QuadPart - 116444736000000000ULL) / 10000;
#endif
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64)tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

static uint32 get_timestamp_sec(void)
{
	return (uint32)time(NULL);
}

static uint64 get_uuid_timestamp(void)
{
	uint64 ms = get_timestamp_ms();
#ifdef _MSC_VER
	uint64 uuid_epoch_offset = 122192928000000000i64;
#else
	uint64 uuid_epoch_offset = 122192928000000000ULL;
#endif
	return ms * 10000 + uuid_epoch_offset;
}

static char to_hex(int val)
{
	return "0123456789abcdef"[val & 0x0F];
}

static int from_hex(char c)
{
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	if(c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}

static void set_uuid_version(DKC_UUID *uuid, int version)
{
	uuid->bytes[6] = (uint8)((uuid->bytes[6] & 0x0F) | ((version & 0x0F) << 4));
	uuid->bytes[8] = (uint8)((uuid->bytes[8] & 0x3F) | 0x80);
}

/* ====================================================================
 * UUID 実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcUUIDGenInit(DKC_UUID_GEN *gen, uint32 seed)
{
	uint32 r;
	if(gen == NULL) return edk_FAILED;

	if(seed == 0){
		seed = (uint32)time(NULL) ^ ((uint32)clock() << 16);
	}
	gen->rand_state = seed;
	gen->clock_seq = (uint16)(xorshift32(&gen->rand_state) & 0x3FFF);

	r = xorshift32(&gen->rand_state);
	gen->node_id[0] = (uint8)((uint8)(r) | 0x01);
	gen->node_id[1] = (uint8)(r >> 8);
	gen->node_id[2] = (uint8)(r >> 16);
	gen->node_id[3] = (uint8)(r >> 24);
	r = xorshift32(&gen->rand_state);
	gen->node_id[4] = (uint8)(r);
	gen->node_id[5] = (uint8)(r >> 8);

	gen->initialized = 1;
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDv1Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid)
{
	uint64 ts;
	if(gen == NULL || uuid == NULL) return edk_FAILED;
	if(!gen->initialized) dkcUUIDGenInit(gen, 0);

	ts = get_uuid_timestamp();
	gen->clock_seq = (uint16)((gen->clock_seq + 1) & 0x3FFF);

	uuid->bytes[0] = (uint8)(ts >> 24);
	uuid->bytes[1] = (uint8)(ts >> 16);
	uuid->bytes[2] = (uint8)(ts >> 8);
	uuid->bytes[3] = (uint8)(ts);
	uuid->bytes[4] = (uint8)(ts >> 40);
	uuid->bytes[5] = (uint8)(ts >> 32);
	uuid->bytes[6] = (uint8)(ts >> 56);
	uuid->bytes[7] = (uint8)(ts >> 48);
	uuid->bytes[8] = (uint8)(gen->clock_seq >> 8);
	uuid->bytes[9] = (uint8)(gen->clock_seq);
	memcpy(&uuid->bytes[10], gen->node_id, 6);

	set_uuid_version(uuid, 1);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDv2Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid,
	edkcUUIDLocalDomain domain, uint32 id)
{
	if(gen == NULL || uuid == NULL) return edk_FAILED;

	dkcUUIDv1Generate(gen, uuid);
	uuid->bytes[0] = (uint8)(id >> 24);
	uuid->bytes[1] = (uint8)(id >> 16);
	uuid->bytes[2] = (uint8)(id >> 8);
	uuid->bytes[3] = (uint8)(id);
	uuid->bytes[9] = (uint8)domain;

	set_uuid_version(uuid, 2);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDv3Generate(DKC_UUID *uuid,
	const DKC_UUID *ns_uuid, const char *name, size_t name_len)
{
	DKC_MD5 *md5;
	uint8 hash[16];

	if(uuid == NULL || ns_uuid == NULL || name == NULL) return edk_FAILED;

	md5 = dkcAllocMD5();
	if(md5 == NULL) return edk_FAILED;

	dkcMD5Load(md5, ns_uuid->bytes, 16);
	dkcMD5Load(md5, (const BYTE *)name, (DWORD)name_len);
	dkcMD5FinalDigest(md5, hash, 16);
	dkcFreeMD5(&md5);

	memcpy(uuid->bytes, hash, 16);
	set_uuid_version(uuid, 3);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDv3GenerateNS(DKC_UUID *uuid,
	edkcUUIDNamespace ns, const char *name, size_t name_len)
{
	DKC_UUID ns_uuid;
	if(dkcUUIDGetNamespace(ns, &ns_uuid) != edk_SUCCEEDED) return edk_FAILED;
	return dkcUUIDv3Generate(uuid, &ns_uuid, name, name_len);
}

DKC_EXTERN int WINAPI dkcUUIDv4Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid)
{
	uint32 r;
	int i;

	if(gen == NULL || uuid == NULL) return edk_FAILED;
	if(!gen->initialized) dkcUUIDGenInit(gen, 0);

	for(i = 0; i < 16; i += 4){
		r = xorshift32(&gen->rand_state);
		uuid->bytes[i]     = (uint8)(r);
		uuid->bytes[i + 1] = (uint8)(r >> 8);
		uuid->bytes[i + 2] = (uint8)(r >> 16);
		uuid->bytes[i + 3] = (uint8)(r >> 24);
	}

	set_uuid_version(uuid, 4);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDv5Generate(DKC_UUID *uuid,
	const DKC_UUID *ns_uuid, const char *name, size_t name_len)
{
	DKC_SHA1 *sha1;
	uint8 hash[20];

	if(uuid == NULL || ns_uuid == NULL || name == NULL) return edk_FAILED;

	sha1 = dkcAllocSHA1();
	if(sha1 == NULL) return edk_FAILED;

	dkcSHA1Load(sha1, ns_uuid->bytes, 16);
	dkcSHA1Load(sha1, (const BYTE *)name, (DWORD)name_len);
	dkcSHA1FinalDigest(sha1, hash, 20);
	dkcFreeSHA1(&sha1);

	memcpy(uuid->bytes, hash, 16);
	set_uuid_version(uuid, 5);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDv5GenerateNS(DKC_UUID *uuid,
	edkcUUIDNamespace ns, const char *name, size_t name_len)
{
	DKC_UUID ns_uuid;
	if(dkcUUIDGetNamespace(ns, &ns_uuid) != edk_SUCCEEDED) return edk_FAILED;
	return dkcUUIDv5Generate(uuid, &ns_uuid, name, name_len);
}

DKC_EXTERN int WINAPI dkcUUIDv6Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid)
{
	uint64 ts;
	uint32 time_high, time_mid, time_low;

	if(gen == NULL || uuid == NULL) return edk_FAILED;
	if(!gen->initialized) dkcUUIDGenInit(gen, 0);

	ts = get_uuid_timestamp();
	gen->clock_seq = (uint16)((gen->clock_seq + 1) & 0x3FFF);

	time_high = (uint32)(ts >> 28);
	time_mid = (uint32)((ts >> 12) & 0xFFFF);
	time_low = (uint32)(ts & 0x0FFF);

	uuid->bytes[0] = (uint8)(time_high >> 24);
	uuid->bytes[1] = (uint8)(time_high >> 16);
	uuid->bytes[2] = (uint8)(time_high >> 8);
	uuid->bytes[3] = (uint8)(time_high);
	uuid->bytes[4] = (uint8)(time_mid >> 8);
	uuid->bytes[5] = (uint8)(time_mid);
	uuid->bytes[6] = (uint8)(time_low >> 8);
	uuid->bytes[7] = (uint8)(time_low);
	uuid->bytes[8] = (uint8)(gen->clock_seq >> 8);
	uuid->bytes[9] = (uint8)(gen->clock_seq);
	memcpy(&uuid->bytes[10], gen->node_id, 6);

	set_uuid_version(uuid, 6);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDv7Generate(DKC_UUID_GEN *gen, DKC_UUID *uuid)
{
	uint64 ts_ms;
	uint32 r1, r2, r3;

	if(gen == NULL || uuid == NULL) return edk_FAILED;
	if(!gen->initialized) dkcUUIDGenInit(gen, 0);

	ts_ms = get_timestamp_ms();

	uuid->bytes[0] = (uint8)(ts_ms >> 40);
	uuid->bytes[1] = (uint8)(ts_ms >> 32);
	uuid->bytes[2] = (uint8)(ts_ms >> 24);
	uuid->bytes[3] = (uint8)(ts_ms >> 16);
	uuid->bytes[4] = (uint8)(ts_ms >> 8);
	uuid->bytes[5] = (uint8)(ts_ms);

	r1 = xorshift32(&gen->rand_state);
	r2 = xorshift32(&gen->rand_state);
	r3 = xorshift32(&gen->rand_state);

	uuid->bytes[6] = (uint8)(r1 >> 8);
	uuid->bytes[7] = (uint8)(r1);
	uuid->bytes[8] = (uint8)(r2 >> 24);
	uuid->bytes[9] = (uint8)(r2 >> 16);
	uuid->bytes[10] = (uint8)(r2 >> 8);
	uuid->bytes[11] = (uint8)(r2);
	uuid->bytes[12] = (uint8)(r3 >> 24);
	uuid->bytes[13] = (uint8)(r3 >> 16);
	uuid->bytes[14] = (uint8)(r3 >> 8);
	uuid->bytes[15] = (uint8)(r3);

	set_uuid_version(uuid, 7);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDv8Generate(DKC_UUID *uuid,
	uint64 custom_a, uint16 custom_b, uint64 custom_c)
{
	if(uuid == NULL) return edk_FAILED;

	uuid->bytes[0] = (uint8)(custom_a >> 40);
	uuid->bytes[1] = (uint8)(custom_a >> 32);
	uuid->bytes[2] = (uint8)(custom_a >> 24);
	uuid->bytes[3] = (uint8)(custom_a >> 16);
	uuid->bytes[4] = (uint8)(custom_a >> 8);
	uuid->bytes[5] = (uint8)(custom_a);
	uuid->bytes[6] = (uint8)(custom_b >> 8);
	uuid->bytes[7] = (uint8)(custom_b);
	uuid->bytes[8] = (uint8)(custom_c >> 56);
	uuid->bytes[9] = (uint8)(custom_c >> 48);
	uuid->bytes[10] = (uint8)(custom_c >> 40);
	uuid->bytes[11] = (uint8)(custom_c >> 32);
	uuid->bytes[12] = (uint8)(custom_c >> 24);
	uuid->bytes[13] = (uint8)(custom_c >> 16);
	uuid->bytes[14] = (uint8)(custom_c >> 8);
	uuid->bytes[15] = (uint8)(custom_c);

	set_uuid_version(uuid, 8);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDToString(const DKC_UUID *uuid, char *str)
{
	int i, j = 0;
	if(uuid == NULL || str == NULL) return edk_FAILED;

	for(i = 0; i < 16; i++){
		if(i == 4 || i == 6 || i == 8 || i == 10) str[j++] = '-';
		str[j++] = to_hex(uuid->bytes[i] >> 4);
		str[j++] = to_hex(uuid->bytes[i]);
	}
	str[j] = '\0';
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDFromString(const char *str, DKC_UUID *uuid)
{
	int i, j = 0, h, l;
	if(str == NULL || uuid == NULL) return edk_FAILED;

	for(i = 0; i < 16; i++){
		if(j == 8 || j == 13 || j == 18 || j == 23){
			if(str[j] != '-') return edk_FAILED;
			j++;
		}
		h = from_hex(str[j++]);
		l = from_hex(str[j++]);
		if(h < 0 || l < 0) return edk_FAILED;
		uuid->bytes[i] = (uint8)((h << 4) | l);
	}
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcUUIDGetVersion(const DKC_UUID *uuid)
{
	if(uuid == NULL) return 0;
	return (uuid->bytes[6] >> 4) & 0x0F;
}

DKC_EXTERN int WINAPI dkcUUIDCompare(const DKC_UUID *a, const DKC_UUID *b)
{
	if(a == NULL || b == NULL) return 0;
	return memcmp(a->bytes, b->bytes, 16);
}

DKC_EXTERN BOOL WINAPI dkcUUIDIsNil(const DKC_UUID *uuid)
{
	int i;
	if(uuid == NULL) return TRUE;
	for(i = 0; i < 16; i++) if(uuid->bytes[i] != 0) return FALSE;
	return TRUE;
}

DKC_EXTERN void WINAPI dkcUUIDNil(DKC_UUID *uuid)
{
	if(uuid) memset(uuid->bytes, 0, 16);
}

DKC_EXTERN int WINAPI dkcUUIDGetNamespace(edkcUUIDNamespace ns, DKC_UUID *uuid)
{
	const uint8 *src;
	if(uuid == NULL) return edk_FAILED;

	switch(ns){
	case edkcUUID_NS_DNS:  src = UUID_NS_DNS;  break;
	case edkcUUID_NS_URL:  src = UUID_NS_URL;  break;
	case edkcUUID_NS_OID:  src = UUID_NS_OID;  break;
	case edkcUUID_NS_X500: src = UUID_NS_X500; break;
	default: return edk_FAILED;
	}
	memcpy(uuid->bytes, src, 16);
	return edk_SUCCEEDED;
}

/* ====================================================================
 * ULID 実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcULIDGenInit(DKC_ULID_GEN *gen, uint32 seed)
{
	if(gen == NULL) return edk_FAILED;
	if(seed == 0) seed = (uint32)time(NULL) ^ ((uint32)clock() << 16);
	gen->rand_state = seed;
	gen->last_timestamp = 0;
	memset(gen->last_random, 0, 10);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcULIDGenerate(DKC_ULID_GEN *gen, DKC_ULID *ulid)
{
	return dkcULIDGenerateWithTime(gen, ulid, get_timestamp_ms());
}

DKC_EXTERN int WINAPI dkcULIDGenerateWithTime(DKC_ULID_GEN *gen, DKC_ULID *ulid, uint64 timestamp_ms)
{
	uint32 r1, r2, r3;
	int i;

	if(gen == NULL || ulid == NULL) return edk_FAILED;

	ulid->bytes[0] = (uint8)(timestamp_ms >> 40);
	ulid->bytes[1] = (uint8)(timestamp_ms >> 32);
	ulid->bytes[2] = (uint8)(timestamp_ms >> 24);
	ulid->bytes[3] = (uint8)(timestamp_ms >> 16);
	ulid->bytes[4] = (uint8)(timestamp_ms >> 8);
	ulid->bytes[5] = (uint8)(timestamp_ms);

	if(timestamp_ms == gen->last_timestamp){
		/* 単調増加 */
		for(i = 9; i >= 0; i--){
			gen->last_random[i]++;
			if(gen->last_random[i] != 0) break;
		}
		memcpy(&ulid->bytes[6], gen->last_random, 10);
	}
	else{
		r1 = xorshift32(&gen->rand_state);
		r2 = xorshift32(&gen->rand_state);
		r3 = xorshift32(&gen->rand_state);

		ulid->bytes[6] = (uint8)(r1 >> 24);
		ulid->bytes[7] = (uint8)(r1 >> 16);
		ulid->bytes[8] = (uint8)(r1 >> 8);
		ulid->bytes[9] = (uint8)(r1);
		ulid->bytes[10] = (uint8)(r2 >> 24);
		ulid->bytes[11] = (uint8)(r2 >> 16);
		ulid->bytes[12] = (uint8)(r2 >> 8);
		ulid->bytes[13] = (uint8)(r2);
		ulid->bytes[14] = (uint8)(r3 >> 8);
		ulid->bytes[15] = (uint8)(r3);

		memcpy(gen->last_random, &ulid->bytes[6], 10);
		gen->last_timestamp = timestamp_ms;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcULIDToString(const DKC_ULID *ulid, char *str)
{
	if(ulid == NULL || str == NULL) return edk_FAILED;

	str[0] = CROCKFORD_BASE32[(ulid->bytes[0] >> 5) & 0x07];
	str[1] = CROCKFORD_BASE32[ulid->bytes[0] & 0x1F];
	str[2] = CROCKFORD_BASE32[(ulid->bytes[1] >> 3) & 0x1F];
	str[3] = CROCKFORD_BASE32[((ulid->bytes[1] & 0x07) << 2) | ((ulid->bytes[2] >> 6) & 0x03)];
	str[4] = CROCKFORD_BASE32[(ulid->bytes[2] >> 1) & 0x1F];
	str[5] = CROCKFORD_BASE32[((ulid->bytes[2] & 0x01) << 4) | ((ulid->bytes[3] >> 4) & 0x0F)];
	str[6] = CROCKFORD_BASE32[((ulid->bytes[3] & 0x0F) << 1) | ((ulid->bytes[4] >> 7) & 0x01)];
	str[7] = CROCKFORD_BASE32[(ulid->bytes[4] >> 2) & 0x1F];
	str[8] = CROCKFORD_BASE32[((ulid->bytes[4] & 0x03) << 3) | ((ulid->bytes[5] >> 5) & 0x07)];
	str[9] = CROCKFORD_BASE32[ulid->bytes[5] & 0x1F];
	str[10] = CROCKFORD_BASE32[(ulid->bytes[6] >> 3) & 0x1F];
	str[11] = CROCKFORD_BASE32[((ulid->bytes[6] & 0x07) << 2) | ((ulid->bytes[7] >> 6) & 0x03)];
	str[12] = CROCKFORD_BASE32[(ulid->bytes[7] >> 1) & 0x1F];
	str[13] = CROCKFORD_BASE32[((ulid->bytes[7] & 0x01) << 4) | ((ulid->bytes[8] >> 4) & 0x0F)];
	str[14] = CROCKFORD_BASE32[((ulid->bytes[8] & 0x0F) << 1) | ((ulid->bytes[9] >> 7) & 0x01)];
	str[15] = CROCKFORD_BASE32[(ulid->bytes[9] >> 2) & 0x1F];
	str[16] = CROCKFORD_BASE32[((ulid->bytes[9] & 0x03) << 3) | ((ulid->bytes[10] >> 5) & 0x07)];
	str[17] = CROCKFORD_BASE32[ulid->bytes[10] & 0x1F];
	str[18] = CROCKFORD_BASE32[(ulid->bytes[11] >> 3) & 0x1F];
	str[19] = CROCKFORD_BASE32[((ulid->bytes[11] & 0x07) << 2) | ((ulid->bytes[12] >> 6) & 0x03)];
	str[20] = CROCKFORD_BASE32[(ulid->bytes[12] >> 1) & 0x1F];
	str[21] = CROCKFORD_BASE32[((ulid->bytes[12] & 0x01) << 4) | ((ulid->bytes[13] >> 4) & 0x0F)];
	str[22] = CROCKFORD_BASE32[((ulid->bytes[13] & 0x0F) << 1) | ((ulid->bytes[14] >> 7) & 0x01)];
	str[23] = CROCKFORD_BASE32[(ulid->bytes[14] >> 2) & 0x1F];
	str[24] = CROCKFORD_BASE32[((ulid->bytes[14] & 0x03) << 3) | ((ulid->bytes[15] >> 5) & 0x07)];
	str[25] = CROCKFORD_BASE32[ulid->bytes[15] & 0x1F];
	str[26] = '\0';

	return edk_SUCCEEDED;
}

static int crockford_decode(char c)
{
	if(c >= '0' && c <= '9') return c - '0';
	c = (char)((c >= 'a' && c <= 'z') ? (c - 32) : c);
	if(c == 'O') return 0;
	if(c == 'I' || c == 'L') return 1;
	if(c >= 'A' && c <= 'H') return c - 'A' + 10;
	if(c >= 'J' && c <= 'K') return c - 'J' + 18;
	if(c >= 'M' && c <= 'N') return c - 'M' + 20;
	if(c >= 'P' && c <= 'T') return c - 'P' + 22;
	if(c >= 'V' && c <= 'Z') return c - 'V' + 27;
	return -1;
}

DKC_EXTERN int WINAPI dkcULIDFromString(const char *str, DKC_ULID *ulid)
{
	int v[26], i;
	if(str == NULL || ulid == NULL) return edk_FAILED;

	for(i = 0; i < 26; i++){
		v[i] = crockford_decode(str[i]);
		if(v[i] < 0) return edk_FAILED;
	}

	ulid->bytes[0] = (uint8)((v[0] << 5) | v[1]);
	ulid->bytes[1] = (uint8)((v[2] << 3) | (v[3] >> 2));
	ulid->bytes[2] = (uint8)((v[3] << 6) | (v[4] << 1) | (v[5] >> 4));
	ulid->bytes[3] = (uint8)((v[5] << 4) | (v[6] >> 1));
	ulid->bytes[4] = (uint8)((v[6] << 7) | (v[7] << 2) | (v[8] >> 3));
	ulid->bytes[5] = (uint8)((v[8] << 5) | v[9]);
	ulid->bytes[6] = (uint8)((v[10] << 3) | (v[11] >> 2));
	ulid->bytes[7] = (uint8)((v[11] << 6) | (v[12] << 1) | (v[13] >> 4));
	ulid->bytes[8] = (uint8)((v[13] << 4) | (v[14] >> 1));
	ulid->bytes[9] = (uint8)((v[14] << 7) | (v[15] << 2) | (v[16] >> 3));
	ulid->bytes[10] = (uint8)((v[16] << 5) | v[17]);
	ulid->bytes[11] = (uint8)((v[18] << 3) | (v[19] >> 2));
	ulid->bytes[12] = (uint8)((v[19] << 6) | (v[20] << 1) | (v[21] >> 4));
	ulid->bytes[13] = (uint8)((v[21] << 4) | (v[22] >> 1));
	ulid->bytes[14] = (uint8)((v[22] << 7) | (v[23] << 2) | (v[24] >> 3));
	ulid->bytes[15] = (uint8)((v[24] << 5) | v[25]);

	return edk_SUCCEEDED;
}

DKC_EXTERN uint64 WINAPI dkcULIDGetTimestamp(const DKC_ULID *ulid)
{
	if(ulid == NULL) return 0;
	return ((uint64)ulid->bytes[0] << 40) | ((uint64)ulid->bytes[1] << 32) |
	       ((uint64)ulid->bytes[2] << 24) | ((uint64)ulid->bytes[3] << 16) |
	       ((uint64)ulid->bytes[4] << 8) | ((uint64)ulid->bytes[5]);
}

DKC_EXTERN int WINAPI dkcULIDCompare(const DKC_ULID *a, const DKC_ULID *b)
{
	if(a == NULL || b == NULL) return 0;
	return memcmp(a->bytes, b->bytes, 16);
}

/* ====================================================================
 * Snowflake 実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcSnowflakeInit(DKC_SNOWFLAKE_GEN *gen,
	uint16 worker_id, uint16 datacenter_id, uint64 epoch, uint32 seed)
{
	if(gen == NULL) return edk_FAILED;
	if(worker_id > 1023 || datacenter_id > 31) return edk_FAILED;

	if(seed == 0) seed = (uint32)time(NULL) ^ ((uint32)clock() << 16);
	gen->rand_state = seed;
	gen->worker_id = worker_id;
	gen->datacenter_id = datacenter_id;
	gen->epoch = (epoch == 0) ? TWITTER_EPOCH : epoch;
	gen->sequence = 0;
	gen->last_timestamp = 0;

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSnowflakeGenerate(DKC_SNOWFLAKE_GEN *gen, DKC_SNOWFLAKE *sf)
{
	uint64 ts;
	if(gen == NULL || sf == NULL) return edk_FAILED;

	ts = get_timestamp_ms();

	if(ts == gen->last_timestamp){
		gen->sequence = (uint16)((gen->sequence + 1) & 0x0FFF);
		if(gen->sequence == 0){
			while(ts <= gen->last_timestamp) ts = get_timestamp_ms();
		}
	}
	else{
		gen->sequence = 0;
	}

	gen->last_timestamp = ts;

	sf->id = ((ts - gen->epoch) << 22) |
	         ((uint64)gen->datacenter_id << 17) |
	         ((uint64)gen->worker_id << 12) |
	         gen->sequence;

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSnowflakeToString(const DKC_SNOWFLAKE *sf, char *str)
{
	uint64 val;
	int i;
	char tmp[21];

	if(sf == NULL || str == NULL) return edk_FAILED;

	val = sf->id;
	i = 20;
	tmp[i--] = '\0';

	if(val == 0){ tmp[i--] = '0'; }
	else{ while(val > 0){ tmp[i--] = (char)('0' + (val % 10)); val /= 10; } }

	strcpy(str, &tmp[i + 1]);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSnowflakeFromString(const char *str, DKC_SNOWFLAKE *sf)
{
	uint64 val = 0;
	if(str == NULL || sf == NULL) return edk_FAILED;

	while(*str){
		if(*str < '0' || *str > '9') return edk_FAILED;
		val = val * 10 + (*str - '0');
		str++;
	}
	sf->id = val;
	return edk_SUCCEEDED;
}

DKC_EXTERN uint64 WINAPI dkcSnowflakeGetTimestamp(const DKC_SNOWFLAKE *sf, uint64 epoch)
{
	if(sf == NULL) return 0;
	return (sf->id >> 22) + epoch;
}

/* ====================================================================
 * KSUID 実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcKSUIDGenInit(DKC_KSUID_GEN *gen, uint32 seed)
{
	if(gen == NULL) return edk_FAILED;
	if(seed == 0) seed = (uint32)time(NULL) ^ ((uint32)clock() << 16);
	gen->rand_state = seed;
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcKSUIDGenerate(DKC_KSUID_GEN *gen, DKC_KSUID *ksuid)
{
	return dkcKSUIDGenerateWithTime(gen, ksuid, get_timestamp_sec());
}

DKC_EXTERN int WINAPI dkcKSUIDGenerateWithTime(DKC_KSUID_GEN *gen, DKC_KSUID *ksuid, uint32 timestamp)
{
	uint32 ts_adj, r;
	int i;

	if(gen == NULL || ksuid == NULL) return edk_FAILED;

	ts_adj = timestamp - dkcd_KSUID_EPOCH;

	ksuid->bytes[0] = (uint8)(ts_adj >> 24);
	ksuid->bytes[1] = (uint8)(ts_adj >> 16);
	ksuid->bytes[2] = (uint8)(ts_adj >> 8);
	ksuid->bytes[3] = (uint8)(ts_adj);

	for(i = 4; i < 20; i += 4){
		r = xorshift32(&gen->rand_state);
		ksuid->bytes[i]     = (uint8)(r >> 24);
		ksuid->bytes[i + 1] = (uint8)(r >> 16);
		ksuid->bytes[i + 2] = (uint8)(r >> 8);
		ksuid->bytes[i + 3] = (uint8)(r);
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcKSUIDToString(const DKC_KSUID *ksuid, char *str)
{
	uint8 num[20];
	int i, j, carry;

	if(ksuid == NULL || str == NULL) return edk_FAILED;

	memcpy(num, ksuid->bytes, 20);

	for(i = 26; i >= 0; i--){
		carry = 0;
		for(j = 0; j < 20; j++){
			int val = carry * 256 + num[j];
			num[j] = (uint8)(val / 62);
			carry = val % 62;
		}
		str[i] = BASE62[carry];
	}
	str[27] = '\0';

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcKSUIDFromString(const char *str, DKC_KSUID *ksuid)
{
	uint8 num[20];
	int i, j, carry;
	const char *p;

	if(str == NULL || ksuid == NULL) return edk_FAILED;

	memset(num, 0, 20);

	for(i = 0; i < 27 && str[i]; i++){
		p = strchr(BASE62, str[i]);
		if(p == NULL) return edk_FAILED;
		carry = (int)(p - BASE62);

		for(j = 19; j >= 0; j--){
			int val = num[j] * 62 + carry;
			num[j] = (uint8)(val & 0xFF);
			carry = val >> 8;
		}
	}

	memcpy(ksuid->bytes, num, 20);
	return edk_SUCCEEDED;
}

DKC_EXTERN uint32 WINAPI dkcKSUIDGetTimestamp(const DKC_KSUID *ksuid)
{
	uint32 ts_adj;
	if(ksuid == NULL) return 0;

	ts_adj = ((uint32)ksuid->bytes[0] << 24) | ((uint32)ksuid->bytes[1] << 16) |
	         ((uint32)ksuid->bytes[2] << 8) | ((uint32)ksuid->bytes[3]);

	return ts_adj + dkcd_KSUID_EPOCH;
}

DKC_EXTERN int WINAPI dkcKSUIDCompare(const DKC_KSUID *a, const DKC_KSUID *b)
{
	if(a == NULL || b == NULL) return 0;
	return memcmp(a->bytes, b->bytes, 20);
}

/* ====================================================================
 * XID 実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcXIDInit(DKC_XID_GEN *gen, uint32 seed)
{
	uint32 r;
	if(gen == NULL) return edk_FAILED;

	if(seed == 0) seed = (uint32)time(NULL) ^ ((uint32)clock() << 16);
	gen->rand_state = seed;

	r = xorshift32(&gen->rand_state);
	gen->machine_id[0] = (uint8)(r);
	gen->machine_id[1] = (uint8)(r >> 8);
	gen->machine_id[2] = (uint8)(r >> 16);

#ifdef _WIN32
	gen->pid = (uint16)GetCurrentProcessId();
#else
	gen->pid = (uint16)getpid();
#endif

	gen->counter = xorshift32(&gen->rand_state) & 0x00FFFFFF;

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcXIDGenerate(DKC_XID_GEN *gen, DKC_XID *xid)
{
	uint32 ts;
	if(gen == NULL || xid == NULL) return edk_FAILED;

	ts = get_timestamp_sec();

	xid->bytes[0] = (uint8)(ts >> 24);
	xid->bytes[1] = (uint8)(ts >> 16);
	xid->bytes[2] = (uint8)(ts >> 8);
	xid->bytes[3] = (uint8)(ts);
	xid->bytes[4] = gen->machine_id[0];
	xid->bytes[5] = gen->machine_id[1];
	xid->bytes[6] = gen->machine_id[2];
	xid->bytes[7] = (uint8)(gen->pid >> 8);
	xid->bytes[8] = (uint8)(gen->pid);
	xid->bytes[9] = (uint8)(gen->counter >> 16);
	xid->bytes[10] = (uint8)(gen->counter >> 8);
	xid->bytes[11] = (uint8)(gen->counter);

	gen->counter = (gen->counter + 1) & 0x00FFFFFF;

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcXIDToString(const DKC_XID *xid, char *str)
{
	if(xid == NULL || str == NULL) return edk_FAILED;

	str[0]  = XID_BASE32[(xid->bytes[0] >> 3) & 0x1F];
	str[1]  = XID_BASE32[((xid->bytes[0] & 0x07) << 2) | ((xid->bytes[1] >> 6) & 0x03)];
	str[2]  = XID_BASE32[(xid->bytes[1] >> 1) & 0x1F];
	str[3]  = XID_BASE32[((xid->bytes[1] & 0x01) << 4) | ((xid->bytes[2] >> 4) & 0x0F)];
	str[4]  = XID_BASE32[((xid->bytes[2] & 0x0F) << 1) | ((xid->bytes[3] >> 7) & 0x01)];
	str[5]  = XID_BASE32[(xid->bytes[3] >> 2) & 0x1F];
	str[6]  = XID_BASE32[((xid->bytes[3] & 0x03) << 3) | ((xid->bytes[4] >> 5) & 0x07)];
	str[7]  = XID_BASE32[xid->bytes[4] & 0x1F];
	str[8]  = XID_BASE32[(xid->bytes[5] >> 3) & 0x1F];
	str[9]  = XID_BASE32[((xid->bytes[5] & 0x07) << 2) | ((xid->bytes[6] >> 6) & 0x03)];
	str[10] = XID_BASE32[(xid->bytes[6] >> 1) & 0x1F];
	str[11] = XID_BASE32[((xid->bytes[6] & 0x01) << 4) | ((xid->bytes[7] >> 4) & 0x0F)];
	str[12] = XID_BASE32[((xid->bytes[7] & 0x0F) << 1) | ((xid->bytes[8] >> 7) & 0x01)];
	str[13] = XID_BASE32[(xid->bytes[8] >> 2) & 0x1F];
	str[14] = XID_BASE32[((xid->bytes[8] & 0x03) << 3) | ((xid->bytes[9] >> 5) & 0x07)];
	str[15] = XID_BASE32[xid->bytes[9] & 0x1F];
	str[16] = XID_BASE32[(xid->bytes[10] >> 3) & 0x1F];
	str[17] = XID_BASE32[((xid->bytes[10] & 0x07) << 2) | ((xid->bytes[11] >> 6) & 0x03)];
	str[18] = XID_BASE32[(xid->bytes[11] >> 1) & 0x1F];
	str[19] = XID_BASE32[(xid->bytes[11] & 0x01) << 4];
	str[20] = '\0';

	return edk_SUCCEEDED;
}

static int xid_base32_decode(char c)
{
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'v') return c - 'a' + 10;
	return -1;
}

DKC_EXTERN int WINAPI dkcXIDFromString(const char *str, DKC_XID *xid)
{
	int v[20], i;
	if(str == NULL || xid == NULL) return edk_FAILED;

	for(i = 0; i < 20; i++){
		v[i] = xid_base32_decode(str[i]);
		if(v[i] < 0) return edk_FAILED;
	}

	xid->bytes[0]  = (uint8)((v[0] << 3) | (v[1] >> 2));
	xid->bytes[1]  = (uint8)((v[1] << 6) | (v[2] << 1) | (v[3] >> 4));
	xid->bytes[2]  = (uint8)((v[3] << 4) | (v[4] >> 1));
	xid->bytes[3]  = (uint8)((v[4] << 7) | (v[5] << 2) | (v[6] >> 3));
	xid->bytes[4]  = (uint8)((v[6] << 5) | v[7]);
	xid->bytes[5]  = (uint8)((v[8] << 3) | (v[9] >> 2));
	xid->bytes[6]  = (uint8)((v[9] << 6) | (v[10] << 1) | (v[11] >> 4));
	xid->bytes[7]  = (uint8)((v[11] << 4) | (v[12] >> 1));
	xid->bytes[8]  = (uint8)((v[12] << 7) | (v[13] << 2) | (v[14] >> 3));
	xid->bytes[9]  = (uint8)((v[14] << 5) | v[15]);
	xid->bytes[10] = (uint8)((v[16] << 3) | (v[17] >> 2));
	xid->bytes[11] = (uint8)((v[17] << 6) | (v[18] << 1) | (v[19] >> 4));

	return edk_SUCCEEDED;
}

DKC_EXTERN uint32 WINAPI dkcXIDGetTimestamp(const DKC_XID *xid)
{
	if(xid == NULL) return 0;
	return ((uint32)xid->bytes[0] << 24) | ((uint32)xid->bytes[1] << 16) |
	       ((uint32)xid->bytes[2] << 8) | ((uint32)xid->bytes[3]);
}

DKC_EXTERN int WINAPI dkcXIDCompare(const DKC_XID *a, const DKC_XID *b)
{
	if(a == NULL || b == NULL) return 0;
	return memcmp(a->bytes, b->bytes, 12);
}

/* ====================================================================
 * NanoID 実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcNanoIDGenInit(DKC_NANOID_GEN *gen, uint32 seed)
{
	if(gen == NULL) return edk_FAILED;
	if(seed == 0) seed = (uint32)time(NULL) ^ ((uint32)clock() << 16);
	gen->rand_state = seed;
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcNanoIDGenerate(DKC_NANOID_GEN *gen, char *str)
{
	return dkcNanoIDGenerateCustom(gen, str, dkcd_NANOID_DEFAULT_SIZE, NULL);
}

DKC_EXTERN int WINAPI dkcNanoIDGenerateCustom(DKC_NANOID_GEN *gen, char *str, size_t size, const char *alphabet)
{
	const char *alpha;
	size_t alpha_len, i;
	uint32 r;
	int mask;

	if(gen == NULL || str == NULL || size == 0) return edk_FAILED;

	alpha = (alphabet != NULL) ? alphabet : dkcd_NANOID_DEFAULT_ALPHABET;
	alpha_len = strlen(alpha);

	if(alpha_len == 0 || alpha_len > 256) return edk_FAILED;

	mask = 1;
	while((size_t)mask < alpha_len) mask <<= 1;
	mask--;

	for(i = 0; i < size; ){
		r = xorshift32(&gen->rand_state);
		while(r > 0 && i < size){
			int idx = r & mask;
			if((size_t)idx < alpha_len) str[i++] = alpha[idx];
			r >>= 8;
		}
	}

	str[size] = '\0';
	return edk_SUCCEEDED;
}

/* ====================================================================
 * CUID 実装
 * ==================================================================== */

static char to_base36(int val)
{
	if(val < 10) return (char)('0' + val);
	return (char)('a' + val - 10);
}

static void to_base36_str(char *str, uint32 val, int len)
{
	int i;
	for(i = len - 1; i >= 0; i--){
		str[i] = to_base36(val % 36);
		val /= 36;
	}
}

DKC_EXTERN int WINAPI dkcCUIDGenInit(DKC_CUID_GEN *gen, uint32 seed)
{
	if(gen == NULL) return edk_FAILED;
	if(seed == 0) seed = (uint32)time(NULL) ^ ((uint32)clock() << 16);
	gen->rand_state = seed;
	gen->counter = 0;
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcCUIDGenerate(DKC_CUID_GEN *gen, char *str)
{
	uint64 ts;
	uint32 r1, r2;
	char *p;

	if(gen == NULL || str == NULL) return edk_FAILED;

	ts = get_timestamp_ms();
	r1 = xorshift32(&gen->rand_state);
	r2 = xorshift32(&gen->rand_state);

	p = str;
	*p++ = 'c';
	to_base36_str(p, (uint32)(ts & 0xFFFFFFFF), 8); p += 8;
	to_base36_str(p, gen->counter++, 4); p += 4;
	to_base36_str(p, r1, 6); p += 6;
	to_base36_str(p, r2, 6); p += 6;
	*p = '\0';

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcCUID2Generate(DKC_CUID_GEN *gen, char *str)
{
	return dkcCUID2GenerateCustom(gen, str, dkcd_CUID2_DEFAULT_SIZE);
}

DKC_EXTERN int WINAPI dkcCUID2GenerateCustom(DKC_CUID_GEN *gen, char *str, size_t length)
{
	const char *alpha = "abcdefghijklmnopqrstuvwxyz";
	size_t i;
	uint32 r;

	if(gen == NULL || str == NULL || length < 4 || length > 32) return edk_FAILED;

	str[0] = alpha[xorshift32(&gen->rand_state) % 26];

	for(i = 1; i < length; ){
		r = xorshift32(&gen->rand_state);
		while(r > 0 && i < length){
			int idx = r % 36;
			str[i++] = to_base36(idx);
			r /= 36;
		}
	}

	str[length] = '\0';
	return edk_SUCCEEDED;
}

/* ====================================================================
 * ユーティリティ 実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcUniqueIDRandomBytes(uint32 *rand_state, void *buf, size_t size)
{
	uint8 *p = (uint8 *)buf;
	uint32 r;
	size_t i;

	if(rand_state == NULL || buf == NULL) return edk_FAILED;

	for(i = 0; i < size; ){
		r = xorshift32(rand_state);
		while(r > 0 && i < size){
			p[i++] = (uint8)r;
			r >>= 8;
		}
	}

	return edk_SUCCEEDED;
}
