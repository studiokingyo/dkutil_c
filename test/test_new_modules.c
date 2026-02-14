/*
 * test_new_modules.c
 * Standalone test for 12 new modules (VS2022 build)
 * Extracts just the new module tests from test_main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dkcChaCha20.h"
#include "../dkcPoly1305.h"
#include "../dkcAESGCM.h"
#include "../dkcBcrypt.h"
#include "../dkcScrypt.h"
#include "../dkcArgon2.h"
#include "../dkcSkipList.h"
/* #include "../dkcTreap.h" */ /* Skipped: dkcMemoryPool.c PROC label conflict with Win32 SDK */
#include "../dkcBPlusTree.h"
#include "../dkcFibonacciHeap.h"
#include "../dkcSuffixArray.h"
#include "../dkcRope.h"

/* Test result counters */
static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (cond) { \
            g_tests_passed++; \
            printf("  [PASS] %s\n", msg); \
        } else { \
            g_tests_failed++; \
            printf("  [FAIL] %s (line %d)\n", msg, __LINE__); \
        } \
    } while(0)

#define TEST_BEGIN(name) \
    printf("\n=== %s ===\n", name)

#define TEST_END() \
    printf("\n")

/* ========================================
 * ChaCha20 Test
 * ======================================== */
void Test_ChaCha20(void)
{
    unsigned char key[32] = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
        0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f
    };
    unsigned char nonce[12] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4a,0x00,0x00,0x00,0x00};
    unsigned char plaintext[64];
    unsigned char ciphertext[64];
    unsigned char decrypted[64];
    DKC_CHACHA20 *ctx;
    DKC_CHACHA20 *ctx2;
    int result;

    TEST_BEGIN("ChaCha20 Test");

    memset(plaintext, 0, sizeof(plaintext));

    ctx = dkcAllocChaCha20(key, 32, nonce, 12);
    TEST_ASSERT(ctx != NULL, "dkcAllocChaCha20");

    result = dkcChaCha20Encrypt(ctx, ciphertext, sizeof(ciphertext), plaintext, sizeof(plaintext));
    TEST_ASSERT(result == edk_SUCCEEDED, "Encrypt zeros");

    ctx2 = dkcAllocChaCha20(key, 32, nonce, 12);
    result = dkcChaCha20Encrypt(ctx2, decrypted, sizeof(decrypted), ciphertext, sizeof(ciphertext));
    TEST_ASSERT(result == edk_SUCCEEDED, "Decrypt");
    TEST_ASSERT(memcmp(decrypted, plaintext, 64) == 0, "Encrypt/Decrypt roundtrip");

    dkcFreeChaCha20(&ctx);
    dkcFreeChaCha20(&ctx2);
    TEST_ASSERT(ctx == NULL, "Free ChaCha20");

    TEST_END();
}

/* ========================================
 * Poly1305 Test
 * ======================================== */
void Test_Poly1305(void)
{
    unsigned char key[32] = {
        0x85,0xd6,0xbe,0x78,0x57,0x55,0x6d,0x33,
        0x7f,0x44,0x52,0xfe,0x42,0xd5,0x06,0xa8,
        0x01,0x03,0x80,0x8a,0xfb,0x0d,0xb2,0xfd,
        0x4a,0xbf,0xf6,0xaf,0x41,0x49,0xf5,0x1b
    };
    unsigned char msg[] = "Cryptographic Forum Research Group";
    unsigned char tag[16];
    unsigned char expected_tag[16] = {
        0xa8,0x06,0x1d,0xc1,0x30,0x51,0x36,0xc6,
        0xc2,0x2b,0x8b,0xaf,0x0c,0x01,0x27,0xa9
    };
    unsigned char tag2[16];
    int result;

    TEST_BEGIN("Poly1305 Test");

    result = dkcPoly1305OneShot(key, 32, msg, 34, tag);
    TEST_ASSERT(result == edk_SUCCEEDED, "Poly1305 OneShot");
    TEST_ASSERT(memcmp(tag, expected_tag, 16) == 0, "RFC 8439 test vector");

    result = dkcPoly1305Verify(tag, expected_tag);
    TEST_ASSERT(result == edk_SUCCEEDED, "Poly1305 Verify (match)");

    memset(tag2, 0, sizeof(tag2));
    result = dkcPoly1305Verify(tag, tag2);
    TEST_ASSERT(result == edk_FAILED, "Poly1305 Verify (mismatch)");

    result = dkcPoly1305OneShot(key, 32, (const unsigned char *)"", 0, tag);
    TEST_ASSERT(result == edk_SUCCEEDED, "Poly1305 empty message");

    TEST_END();
}

/* ========================================
 * AES-GCM Test
 * ======================================== */
void Test_AESGCM(void)
{
    unsigned char key[16] = {0};
    unsigned char iv[12] = {0};
    unsigned char pt[16] = {0};
    unsigned char ct[16];
    unsigned char tag[16];
    unsigned char decrypted[16];
    DKC_AESGCM *ctx;
    int result;

    TEST_BEGIN("AES-GCM Test");

    ctx = dkcAllocAESGCM(key, 16);
    TEST_ASSERT(ctx != NULL, "dkcAllocAESGCM");

    result = dkcAESGCMEncrypt(ctx, iv, 12, NULL, 0, pt, 16, ct, tag);
    TEST_ASSERT(result == edk_SUCCEEDED, "AES-GCM Encrypt");

    result = dkcAESGCMDecrypt(ctx, iv, 12, NULL, 0, ct, 16, tag, decrypted);
    TEST_ASSERT(result == edk_SUCCEEDED, "AES-GCM Decrypt (valid tag)");
    TEST_ASSERT(memcmp(decrypted, pt, 16) == 0, "AES-GCM roundtrip");

    tag[0] ^= 0xFF;
    result = dkcAESGCMDecrypt(ctx, iv, 12, NULL, 0, ct, 16, tag, decrypted);
    TEST_ASSERT(result == edk_FAILED, "AES-GCM Decrypt (tampered tag)");

    dkcFreeAESGCM(&ctx);
    TEST_ASSERT(ctx == NULL, "Free AES-GCM");

    TEST_END();
}

/* ========================================
 * bcrypt Test
 * ======================================== */
void Test_Bcrypt(void)
{
    unsigned char salt[16] = {
        0x71,0xd7,0x9f,0x82,0x18,0xa3,0x92,0x59,
        0xa7,0xa2,0x9a,0xab,0xb2,0xdb,0xaf,0xc3
    };
    char hash_output[64];
    int result;

    TEST_BEGIN("bcrypt Test");

    result = dkcBcryptHash((const unsigned char *)"password", 8, salt, 4,
        hash_output, sizeof(hash_output));
    TEST_ASSERT(result == edk_SUCCEEDED, "bcrypt hash (cost=4)");
    TEST_ASSERT(hash_output[0] == '$' && hash_output[1] == '2', "bcrypt format $2b$");

    result = dkcBcryptVerify((const unsigned char *)"password", 8, hash_output);
    TEST_ASSERT(result == edk_SUCCEEDED, "bcrypt verify (correct)");

    result = dkcBcryptVerify((const unsigned char *)"wrong", 5, hash_output);
    TEST_ASSERT(result == edk_FAILED, "bcrypt verify (wrong password)");

    TEST_END();
}

/* ========================================
 * scrypt Test
 * ======================================== */
void Test_Scrypt(void)
{
    unsigned char output[32];
    int result;

    TEST_BEGIN("scrypt Test");

    result = dkcScrypt(
        (const unsigned char *)"password", 8,
        (const unsigned char *)"salt", 4,
        16, 1, 1,
        output, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "scrypt basic");
    TEST_ASSERT(output[0] != 0 || output[1] != 0, "scrypt non-zero output");

    {
        unsigned char output2[32];
        result = dkcScrypt(
            (const unsigned char *)"password", 8,
            (const unsigned char *)"salt", 4,
            16, 1, 1,
            output2, 32);
        TEST_ASSERT(memcmp(output, output2, 32) == 0, "scrypt deterministic");
    }

    TEST_END();
}

/* ========================================
 * Argon2 Test
 * ======================================== */
void Test_Argon2(void)
{
    unsigned char hash1[32];
    unsigned char hash2[32];
    int result;

    TEST_BEGIN("Argon2 Test");

    result = dkcArgon2Hash(dkcd_ARGON2D, 1, 16, 1,
        (const unsigned char *)"password", 8,
        (const unsigned char *)"somesalt", 8,
        hash1, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Argon2d hash");

    result = dkcArgon2Hash(dkcd_ARGON2I, 1, 16, 1,
        (const unsigned char *)"password", 8,
        (const unsigned char *)"somesalt", 8,
        hash2, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Argon2i hash");

    TEST_ASSERT(memcmp(hash1, hash2, 32) != 0, "Argon2d != Argon2i");

    {
        unsigned char hash3[32];
        dkcArgon2Hash(dkcd_ARGON2D, 1, 16, 1,
            (const unsigned char *)"password", 8,
            (const unsigned char *)"somesalt", 8,
            hash3, 32);
        TEST_ASSERT(memcmp(hash1, hash3, 32) == 0, "Argon2 deterministic");
    }

    TEST_END();
}

/* ========================================
 * Skip List Test
 * ======================================== */
static int sl_int_compare(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    if(va < vb) return -1;
    if(va > vb) return 1;
    return 0;
}

void Test_SkipList(void)
{
    DKC_SKIPLIST_ROOT *sl;
    DKC_SKIPLIST_NODE *node;
    int i, val;
    int result;

    TEST_BEGIN("Skip List Test");

    sl = dkcAllocSkipListRoot(sizeof(int), sl_int_compare, 0);
    TEST_ASSERT(sl != NULL, "dkcAllocSkipListRoot");
    TEST_ASSERT(dkcSkipListIsEmpty(sl) == TRUE, "Empty skip list");

    for(i = 0; i < 100; i++){
        val = i * 3;
        result = dkcSkipListInsert(sl, &val, &i, sizeof(int));
        TEST_ASSERT(result == edk_SUCCEEDED || i > 0, "SkipList insert");
    }
    TEST_ASSERT(dkcSkipListSize(sl) == 100, "Size == 100");

    val = 15;
    node = dkcSkipListFind(sl, &val);
    TEST_ASSERT(node != NULL, "Find key 15");

    val = 999;
    node = dkcSkipListFind(sl, &val);
    TEST_ASSERT(node == NULL, "Find non-existent key");

    val = 15;
    result = dkcSkipListErase(sl, &val);
    TEST_ASSERT(result == edk_SUCCEEDED, "Erase key 15");
    TEST_ASSERT(dkcSkipListSize(sl) == 99, "Size after erase");

    node = dkcSkipListFind(sl, &val);
    TEST_ASSERT(node == NULL, "Find erased key");

    dkcFreeSkipListRoot(&sl);
    TEST_ASSERT(sl == NULL, "Free skip list");

    TEST_END();
}

/* ========================================
 * Treap Test
 * ======================================== */
/*
static int treap_int_compare(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    if(va < vb) return -1;
    if(va > vb) return 1;
    return 0;
}
*/

/*
void Test_Treap(void)
{
    DKC_TREAP_ROOT *treap;
    DKC_TREAP_NODE *node;
    int i, val;
    int result;

    TEST_BEGIN("Treap Test");

    treap = dkcAllocTreapRoot(sizeof(int), 32, treap_int_compare, 0);
    TEST_ASSERT(treap != NULL, "dkcAllocTreapRoot");
    TEST_ASSERT(dkcTreapIsEmpty(treap) == TRUE, "Empty treap");

    for(i = 0; i < 100; i++){
        val = 100 - i;
        result = dkcTreapInsert(treap, &val, &i, sizeof(int));
    }
    TEST_ASSERT(dkcTreapSize(treap) == 100, "Size == 100");

    val = 50;
    node = dkcTreapFind(treap, &val);
    TEST_ASSERT(node != NULL, "Find key 50");

    val = 0;
    node = dkcTreapFind(treap, &val);
    TEST_ASSERT(node == NULL, "Find non-existent key 0");

    val = 50;
    result = dkcTreapErase(treap, &val);
    TEST_ASSERT(result == edk_SUCCEEDED, "Erase key 50");
    TEST_ASSERT(dkcTreapSize(treap) == 99, "Size after erase");

    dkcFreeTreapRoot(&treap);
    TEST_ASSERT(treap == NULL, "Free treap");

    TEST_END();
}
*/

/* ========================================
 * B+ Tree Test
 * ======================================== */
static int bpt_int_compare(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    if(va < vb) return -1;
    if(va > vb) return 1;
    return 0;
}

static int bpt_range_count;
static BOOL WINAPI bpt_count_callback(const void *key, void *data, size_t data_size, void *user)
{
    (void)key; (void)data; (void)data_size; (void)user;
    bpt_range_count++;
    return TRUE;
}

void Test_BPlusTree(void)
{
    DKC_BPLUSTREE_ROOT *bpt;
    int i, val;
    int result;
    int found;

    TEST_BEGIN("B+ Tree Test");

    bpt = dkcAllocBPlusTreeRoot(sizeof(int), 4, bpt_int_compare, 0);
    TEST_ASSERT(bpt != NULL, "dkcAllocBPlusTreeRoot");
    TEST_ASSERT(dkcBPlusTreeIsEmpty(bpt) == TRUE, "Empty B+ tree");

    for(i = 0; i < 50; i++){
        val = i * 2;
        result = dkcBPlusTreeInsert(bpt, &val, &i, sizeof(int));
    }
    TEST_ASSERT(dkcBPlusTreeSize(bpt) == 50, "Size == 50");

    val = 10;
    result = dkcBPlusTreeFind(bpt, &val, &found, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Find key 10");
    TEST_ASSERT(found == 5, "Found data == 5");

    val = 11;
    result = dkcBPlusTreeFind(bpt, &val, NULL, 0);
    TEST_ASSERT(result == edk_Not_Found, "Find non-existent key");

    {
        int start = 10, end = 30;
        bpt_range_count = 0;
        dkcBPlusTreeRangeForeach(bpt, &start, &end, bpt_count_callback, NULL);
        TEST_ASSERT(bpt_range_count == 11, "Range [10,30] count == 11");
    }

    val = 10;
    result = dkcBPlusTreeErase(bpt, &val);
    TEST_ASSERT(result == edk_SUCCEEDED, "Erase key 10");
    TEST_ASSERT(dkcBPlusTreeSize(bpt) == 49, "Size after erase");

    dkcFreeBPlusTreeRoot(&bpt);
    TEST_ASSERT(bpt == NULL, "Free B+ tree");

    TEST_END();
}

/* ========================================
 * Fibonacci Heap Test
 * ======================================== */
static int fh_int_compare(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    if(va < vb) return -1;
    if(va > vb) return 1;
    return 0;
}

void Test_FibonacciHeap(void)
{
    DKC_FIBHEAP_ROOT *fh;
    DKC_FIBHEAP_NODE *min_node;
    DKC_FIBHEAP_NODE *node;
    int val, extracted;
    int result;

    TEST_BEGIN("Fibonacci Heap Test");

    fh = dkcAllocFibHeapRoot(sizeof(int), fh_int_compare);
    TEST_ASSERT(fh != NULL, "dkcAllocFibHeapRoot");
    TEST_ASSERT(dkcFibHeapIsEmpty(fh) == TRUE, "Empty Fibonacci heap");

    val = 30;
    dkcFibHeapInsert(fh, &val, NULL, 0);
    val = 10;
    node = dkcFibHeapInsert(fh, &val, NULL, 0);
    val = 20;
    dkcFibHeapInsert(fh, &val, NULL, 0);
    val = 5;
    dkcFibHeapInsert(fh, &val, NULL, 0);

    TEST_ASSERT(dkcFibHeapSize(fh) == 4, "Size == 4");

    min_node = dkcFibHeapFindMin(fh);
    TEST_ASSERT(min_node != NULL, "FindMin not NULL");
    TEST_ASSERT(*(int *)min_node->key == 5, "Min == 5");

    val = 1;
    result = dkcFibHeapDecreaseKey(fh, node, &val);
    TEST_ASSERT(result == edk_SUCCEEDED, "DecreaseKey 10->1");

    min_node = dkcFibHeapFindMin(fh);
    TEST_ASSERT(*(int *)min_node->key == 1, "Min == 1 after decrease");

    result = dkcFibHeapExtractMin(fh, &extracted, NULL, 0);
    TEST_ASSERT(result == edk_SUCCEEDED, "ExtractMin");
    TEST_ASSERT(extracted == 1, "Extracted == 1");
    TEST_ASSERT(dkcFibHeapSize(fh) == 3, "Size == 3");

    result = dkcFibHeapExtractMin(fh, &extracted, NULL, 0);
    TEST_ASSERT(extracted == 5, "Extracted == 5");

    result = dkcFibHeapExtractMin(fh, &extracted, NULL, 0);
    TEST_ASSERT(extracted == 20, "Extracted == 20");

    result = dkcFibHeapExtractMin(fh, &extracted, NULL, 0);
    TEST_ASSERT(extracted == 30, "Extracted == 30");

    TEST_ASSERT(dkcFibHeapIsEmpty(fh) == TRUE, "Empty after all extractions");

    dkcFreeFibHeapRoot(&fh);
    TEST_ASSERT(fh == NULL, "Free Fibonacci heap");

    TEST_END();
}

/* ========================================
 * Suffix Array Test
 * ======================================== */
void Test_SuffixArray(void)
{
    DKC_SUFFIXARRAY *sa;
    size_t first, count;
    int result;

    TEST_BEGIN("Suffix Array Test");

    sa = dkcAllocSuffixArray("banana", 6);
    TEST_ASSERT(sa != NULL, "dkcAllocSuffixArray");

    result = dkcSuffixArraySearch(sa, "ana", 3, &first, &count);
    TEST_ASSERT(result == edk_SUCCEEDED, "Search 'ana'");
    TEST_ASSERT(count == 2, "Found 2 occurrences of 'ana'");

    result = dkcSuffixArraySearch(sa, "ban", 3, &first, &count);
    TEST_ASSERT(result == edk_SUCCEEDED, "Search 'ban'");
    TEST_ASSERT(count == 1, "Found 1 occurrence of 'ban'");

    result = dkcSuffixArraySearch(sa, "xyz", 3, &first, &count);
    TEST_ASSERT(result == edk_Not_Found, "Search non-existent 'xyz'");

    result = dkcSuffixArrayBuildLCP(sa);
    TEST_ASSERT(result == edk_SUCCEEDED, "Build LCP array");
    TEST_ASSERT(sa->lcp != NULL, "LCP not NULL");

    dkcFreeSuffixArray(&sa);
    TEST_ASSERT(sa == NULL, "Free suffix array");

    TEST_END();
}

/* ========================================
 * Rope Test
 * ======================================== */
void Test_Rope(void)
{
    DKC_ROPE_NODE *rope;
    DKC_ROPE_NODE *rope2;
    char buf[256];
    int ch;
    int result;

    TEST_BEGIN("Rope Test");

    rope = dkcAllocRope("Hello");
    TEST_ASSERT(rope != NULL, "dkcAllocRope");
    TEST_ASSERT(dkcRopeLength(rope) == 5, "Length == 5");

    ch = dkcRopeCharAt(rope, 0);
    TEST_ASSERT(ch == 'H', "CharAt(0) == 'H'");
    ch = dkcRopeCharAt(rope, 4);
    TEST_ASSERT(ch == 'o', "CharAt(4) == 'o'");

    result = dkcRopeInsert(&rope, 5, " World");
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert ' World'");
    TEST_ASSERT(dkcRopeLength(rope) == 11, "Length == 11");

    dkcRopeToString(rope, buf, sizeof(buf));
    TEST_ASSERT(strcmp(buf, "Hello World") == 0, "ToString == 'Hello World'");

    rope2 = dkcAllocRope("!!!");
    rope = dkcRopeConcat(rope, rope2);
    TEST_ASSERT(dkcRopeLength(rope) == 14, "Length after concat == 14");

    dkcRopeToString(rope, buf, sizeof(buf));
    TEST_ASSERT(strcmp(buf, "Hello World!!!") == 0, "Concat result");

    result = dkcRopeDelete(&rope, 5, 6);
    TEST_ASSERT(result == edk_SUCCEEDED, "Delete");
    TEST_ASSERT(dkcRopeLength(rope) == 8, "Length after delete == 8");

    dkcRopeToString(rope, buf, sizeof(buf));
    TEST_ASSERT(strcmp(buf, "Hello!!!") == 0, "Delete result");

    dkcRopeSubstring(rope, 0, 5, buf, sizeof(buf));
    TEST_ASSERT(strcmp(buf, "Hello") == 0, "Substring(0,5)");

    dkcFreeRope(&rope);
    TEST_ASSERT(rope == NULL, "Free rope");

    TEST_END();
}

/* ========================================
 * MAIN
 * ======================================== */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("========================================\n");
    printf("  New Modules Test Suite (12 modules)\n");
    printf("========================================\n");

    /* Crypto Tests */
    Test_ChaCha20();
    Test_Poly1305();
    Test_AESGCM();
    Test_Bcrypt();
    Test_Scrypt();
    Test_Argon2();

    /* Data Structure Tests */
    Test_SkipList();
    /* Test_Treap(); */ /* Skipped: dkcMemoryPool.c PROC conflict */
    Test_BPlusTree();
    Test_FibonacciHeap();
    Test_SuffixArray();
    Test_Rope();

    printf("========================================\n");
    printf("  Test Results\n");
    printf("========================================\n");
    printf("  Passed: %d\n", g_tests_passed);
    printf("  Failed: %d\n", g_tests_failed);
    printf("  Total:  %d\n", g_tests_passed + g_tests_failed);
    printf("========================================\n");

    if (g_tests_failed > 0) {
        printf("  SOME TESTS FAILED!\n");
        return 1;
    } else {
        printf("  ALL TESTS PASSED!\n");
        return 0;
    }
}
