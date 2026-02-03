/*
 * test_main.c
 * dkutil_c Comprehensive Test Suite
 * VC6 Compatible (C89)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dkc.h"

/* Test result counters */
static int g_tests_passed = 0;
static int g_tests_failed = 0;

/* Test macros */
#define TEST_ASSERT(cond, msg) \
    do { \
        if (cond) { \
            g_tests_passed++; \
            printf("  [PASS] %s\n", msg); \
        } else { \
            g_tests_failed++; \
            printf("  [FAIL] %s\n", msg); \
        } \
    } while(0)

#define TEST_BEGIN(name) \
    printf("\n=== %s ===\n", name)

#define TEST_END() \
    printf("\n")

/* ========================================
 * CONTAINER TESTS
 * ======================================== */

/*
 * Test: dkcStack.c
 */
void Test_Stack(void)
{
    DKC_STACK *stack;
    int value;
    int result;

    TEST_BEGIN("dkcStack Test");

    stack = dkcAllocStack(10, sizeof(int));
    TEST_ASSERT(stack != NULL, "dkcAllocStack");

    TEST_ASSERT(dkcStackIsEmpty(stack) == TRUE, "New stack is empty");

    value = 100;
    result = dkcStackPush(stack, &value);
    TEST_ASSERT(result == edk_SUCCEEDED, "Push 100");

    value = 200;
    dkcStackPush(stack, &value);
    value = 300;
    dkcStackPush(stack, &value);

    TEST_ASSERT(dkcStackSize(stack) == 3, "Stack size is 3");

    dkcStackTop(stack, &value);
    TEST_ASSERT(value == 300, "Top is 300 (LIFO)");

    dkcStackPop(stack);
    dkcStackPop(stack);
    dkcStackPop(stack);

    TEST_ASSERT(dkcStackIsEmpty(stack) == TRUE, "Stack empty after pops");

    dkcFreeStack(&stack);
    TEST_ASSERT(stack == NULL, "Free stack");

    TEST_END();
}

/*
 * Test: dkcQueue.c
 */
void Test_Queue(void)
{
    DKC_QUEUE *queue;
    int value;

    TEST_BEGIN("dkcQueue Test");

    queue = dkcAllocQueue(10, sizeof(int));
    TEST_ASSERT(queue != NULL, "dkcAllocQueue");
    TEST_ASSERT(dkcQueueIsEmpty(queue) == TRUE, "New queue is empty");

    value = 10;
    dkcQueuePush(queue, &value);
    value = 20;
    dkcQueuePush(queue, &value);
    value = 30;
    dkcQueuePush(queue, &value);

    TEST_ASSERT(dkcQueueSize(queue) == 3, "Queue size is 3");

    dkcQueueTop(queue, &value);
    TEST_ASSERT(value == 10, "Front is 10 (FIFO)");

    dkcQueuePop(queue);
    dkcQueueTop(queue, &value);
    TEST_ASSERT(value == 20, "Front is 20 after pop");

    dkcQueueClear(queue);
    TEST_ASSERT(dkcQueueIsEmpty(queue) == TRUE, "Queue empty after clear");

    dkcFreeQueue(&queue);
    TEST_END();
}

/*
 * Test: dkcDeque.c
 */
void Test_Deque(void)
{
    DKC_DEQUE *deque;
    int value;
    int result;
    int i;

    TEST_BEGIN("dkcDeque Test");

    deque = dkcAllocDeque(10, sizeof(int));
    TEST_ASSERT(deque != NULL, "dkcAllocDeque");
    TEST_ASSERT(dkcDequeIsEmpty(deque) == TRUE, "New deque is empty");
    TEST_ASSERT(dkcDequeSize(deque) == 0, "Size is 0");

    /* Basic push/pop */
    value = 1;
    dkcDequePushBack(deque, &value);
    value = 2;
    dkcDequePushBack(deque, &value);
    value = 0;
    dkcDequePushFront(deque, &value);

    TEST_ASSERT(dkcDequeSize(deque) == 3, "Size is 3");
    TEST_ASSERT(dkcDequeIsEmpty(deque) == FALSE, "Not empty");

    /* Front/Back peek */
    dkcDequeFront(deque, &value);
    TEST_ASSERT(value == 0, "Front is 0");
    dkcDequeBack(deque, &value);
    TEST_ASSERT(value == 2, "Back is 2");

    /* Random access: should be [0, 1, 2] */
    dkcDequeGetPoint(deque, 0, &value, sizeof(int));
    TEST_ASSERT(value == 0, "GetPoint[0] = 0");
    dkcDequeGetPoint(deque, 1, &value, sizeof(int));
    TEST_ASSERT(value == 1, "GetPoint[1] = 1");
    dkcDequeGetPoint(deque, 2, &value, sizeof(int));
    TEST_ASSERT(value == 2, "GetPoint[2] = 2");

    /* SetPoint */
    value = 99;
    dkcDequeSetPoint(deque, 1, &value, sizeof(int));
    dkcDequeGetPoint(deque, 1, &value, sizeof(int));
    TEST_ASSERT(value == 99, "SetPoint[1] = 99");

    /* Pop order: should be 0, 2, 99 */
    dkcDequePopFront(deque, &value);
    TEST_ASSERT(value == 0, "PopFront returns 0");

    dkcDequePopBack(deque, &value);
    TEST_ASSERT(value == 2, "PopBack returns 2");

    dkcDequePopFront(deque, &value);
    TEST_ASSERT(value == 99, "PopFront returns 99");

    TEST_ASSERT(dkcDequeIsEmpty(deque) == TRUE, "Empty after pops");

    /* Pop from empty returns error */
    result = dkcDequePopFront(deque, &value);
    TEST_ASSERT(result == edk_FAILED, "PopFront on empty fails");
    result = dkcDequePopBack(deque, &value);
    TEST_ASSERT(result == edk_FAILED, "PopBack on empty fails");

    /* Clear test */
    value = 10;
    dkcDequePushBack(deque, &value);
    value = 20;
    dkcDequePushBack(deque, &value);
    dkcDequeClear(deque);
    TEST_ASSERT(dkcDequeSize(deque) == 0, "Clear empties deque");

    dkcFreeDeque(&deque);

    /* Dynamic growth test - push more than initial capacity */
    deque = dkcAllocDeque(4, sizeof(int));
    TEST_ASSERT(deque != NULL, "Alloc small deque (cap=4)");
    TEST_ASSERT(dkcDequeCapacity(deque) == 4, "Capacity is 4");

    for (i = 0; i < 20; i++) {
        value = i;
        result = dkcDequePushBack(deque, &value);
        if (result != edk_SUCCEEDED) break;
    }
    TEST_ASSERT(dkcDequeSize(deque) == 20, "Grew to 20 elements");
    TEST_ASSERT(dkcDequeCapacity(deque) >= 20, "Capacity >= 20");

    /* Verify order preserved after growth */
    for (i = 0; i < 20; i++) {
        dkcDequeGetPoint(deque, (size_t)i, &value, sizeof(int));
        if (value != i) break;
    }
    TEST_ASSERT(i == 20, "Order preserved after growth");

    dkcFreeDeque(&deque);

    /* Wrap-around test: alternate front/back pushes */
    deque = dkcAllocDeque(4, sizeof(int));

    value = 3;
    dkcDequePushBack(deque, &value);  /* [3] */
    value = 2;
    dkcDequePushFront(deque, &value); /* [2, 3] */
    value = 4;
    dkcDequePushBack(deque, &value);  /* [2, 3, 4] */
    value = 1;
    dkcDequePushFront(deque, &value); /* [1, 2, 3, 4] */

    /* Now at capacity, push should trigger growth */
    value = 5;
    result = dkcDequePushBack(deque, &value); /* [1, 2, 3, 4, 5] */
    TEST_ASSERT(result == edk_SUCCEEDED, "Push after wrap+grow");

    value = 0;
    dkcDequePushFront(deque, &value); /* [0, 1, 2, 3, 4, 5] */

    TEST_ASSERT(dkcDequeSize(deque) == 6, "6 elements after wrap test");

    for (i = 0; i < 6; i++) {
        dkcDequeGetPoint(deque, (size_t)i, &value, sizeof(int));
        if (value != i) break;
    }
    TEST_ASSERT(i == 6, "Wrap-around order correct [0..5]");

    dkcFreeDeque(&deque);

    TEST_END();
}

/*
 * Test: dkcHashSet.c
 */
static int WINAPIV test_hashset_int_compare(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    return va - vb;
}

static BOOL WINAPI test_hashset_count_callback(const void *key, void *user)
{
    int *count = (int *)user;
    (void)key;
    (*count)++;
    return TRUE;
}

void Test_HashSet(void)
{
    DKC_HASHSET *hs;
    int key;
    int i;
    int count;
    void *found;

    TEST_BEGIN("dkcHashSet Test");

    /* Alloc/Free basic */
    hs = dkcAllocHashSet(sizeof(int), 0, NULL, test_hashset_int_compare);
    TEST_ASSERT(hs != NULL, "dkcAllocHashSet");
    TEST_ASSERT(dkcHashSetIsEmpty(hs) == TRUE, "New hash set is empty");
    TEST_ASSERT(dkcHashSetSize(hs) == 0, "Size is 0");
    TEST_ASSERT(dkcHashSetBucketCount(hs) == 16, "Default bucket count is 16");

    /* Insert */
    key = 42;
    TEST_ASSERT(dkcHashSetInsert(hs, &key) == edk_SUCCEEDED, "Insert 42");
    TEST_ASSERT(dkcHashSetSize(hs) == 1, "Size is 1 after insert");
    TEST_ASSERT(dkcHashSetIsEmpty(hs) == FALSE, "Not empty after insert");

    /* Duplicate rejection */
    key = 42;
    TEST_ASSERT(dkcHashSetInsert(hs, &key) == edk_FAILED, "Duplicate 42 rejected");
    TEST_ASSERT(dkcHashSetSize(hs) == 1, "Size still 1 after duplicate");

    /* Contains */
    key = 42;
    TEST_ASSERT(dkcHashSetContains(hs, &key) == TRUE, "Contains 42");
    key = 99;
    TEST_ASSERT(dkcHashSetContains(hs, &key) == FALSE, "Does not contain 99");

    /* Find */
    key = 42;
    found = dkcHashSetFind(hs, &key);
    TEST_ASSERT(found != NULL, "Find 42 returns non-NULL");
    TEST_ASSERT(*(int *)found == 42, "Find 42 returns correct value");
    key = 99;
    found = dkcHashSetFind(hs, &key);
    TEST_ASSERT(found == NULL, "Find 99 returns NULL");

    /* Insert more */
    key = 10;
    dkcHashSetInsert(hs, &key);
    key = 20;
    dkcHashSetInsert(hs, &key);
    key = 30;
    dkcHashSetInsert(hs, &key);
    TEST_ASSERT(dkcHashSetSize(hs) == 4, "Size is 4 after inserting 10,20,30");

    /* Erase */
    key = 20;
    TEST_ASSERT(dkcHashSetErase(hs, &key) == edk_SUCCEEDED, "Erase 20");
    TEST_ASSERT(dkcHashSetSize(hs) == 3, "Size is 3 after erase");
    TEST_ASSERT(dkcHashSetContains(hs, &key) == FALSE, "20 no longer contained");

    /* Erase non-existent */
    key = 999;
    TEST_ASSERT(dkcHashSetErase(hs, &key) == edk_Not_Found, "Erase non-existent returns Not_Found");

    /* Foreach */
    count = 0;
    dkcHashSetForeach(hs, test_hashset_count_callback, &count);
    TEST_ASSERT(count == 3, "Foreach visits 3 elements");

    /* Clear */
    dkcHashSetClear(hs);
    TEST_ASSERT(dkcHashSetSize(hs) == 0, "Size is 0 after clear");
    TEST_ASSERT(dkcHashSetIsEmpty(hs) == TRUE, "Empty after clear");

    /* Mass insert to trigger rehash */
    {
        size_t initial_buckets = dkcHashSetBucketCount(hs);
        int all_found = 1;
        for(i = 0; i < 100; i++){
            key = i * 7;
            dkcHashSetInsert(hs, &key);
        }
        TEST_ASSERT(dkcHashSetSize(hs) == 100, "100 elements after mass insert");
        TEST_ASSERT(dkcHashSetBucketCount(hs) > initial_buckets, "Rehash increased bucket count");

        /* verify all elements present */
        for(i = 0; i < 100; i++){
            key = i * 7;
            if(dkcHashSetContains(hs, &key) == FALSE){
                all_found = 0;
                break;
            }
        }
        TEST_ASSERT(all_found == 1, "All 100 elements found after rehash");

        /* mass erase */
        for(i = 0; i < 50; i++){
            key = i * 7;
            dkcHashSetErase(hs, &key);
        }
        TEST_ASSERT(dkcHashSetSize(hs) == 50, "50 elements after partial erase");
    }

    /* Error handling */
    TEST_ASSERT(dkcHashSetInsert(NULL, &key) == edk_FAILED, "Insert NULL ptr fails");
    TEST_ASSERT(dkcHashSetContains(NULL, &key) == FALSE, "Contains NULL ptr returns FALSE");
    TEST_ASSERT(dkcAllocHashSet(0, 0, NULL, test_hashset_int_compare) == NULL, "Alloc with key_size=0 fails");
    TEST_ASSERT(dkcAllocHashSet(sizeof(int), 0, NULL, NULL) == NULL, "Alloc with NULL compare fails");

    dkcFreeHashSet(&hs);
    TEST_ASSERT(hs == NULL, "Free hash set");

    TEST_END();
}

/*
 * Test: dkcHashMap.c
 */
static int WINAPIV test_hashmap_int_compare(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    return va - vb;
}

static BOOL WINAPI test_hashmap_sum_callback(
    const void *key, void *data, size_t data_size, void *user)
{
    int *sum = (int *)user;
    (void)key;
    (void)data_size;
    if(data != NULL){
        *sum += *(int *)data;
    }
    return TRUE;
}

void Test_HashMap(void)
{
    DKC_HASHMAP *hm;
    int key;
    int val;
    int result;
    int read_val;
    void *found;
    size_t found_size;
    int i;
    int sum;

    TEST_BEGIN("dkcHashMap Test");

    /* Alloc/Free basic */
    hm = dkcAllocHashMap(sizeof(int), 0, NULL, test_hashmap_int_compare);
    TEST_ASSERT(hm != NULL, "dkcAllocHashMap");
    TEST_ASSERT(dkcHashMapIsEmpty(hm) == TRUE, "New hash map is empty");
    TEST_ASSERT(dkcHashMapSize(hm) == 0, "Size is 0");
    TEST_ASSERT(dkcHashMapBucketCount(hm) == 16, "Default bucket count is 16");

    /* Insert key-value pair */
    key = 1; val = 100;
    result = dkcHashMapInsert(hm, &key, &val, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert key=1 val=100");
    TEST_ASSERT(dkcHashMapSize(hm) == 1, "Size is 1");

    /* Duplicate rejection */
    key = 1; val = 999;
    TEST_ASSERT(dkcHashMapInsert(hm, &key, &val, sizeof(int)) == edk_FAILED,
        "Duplicate key=1 rejected");

    /* Contains */
    key = 1;
    TEST_ASSERT(dkcHashMapContains(hm, &key) == TRUE, "Contains key=1");
    key = 2;
    TEST_ASSERT(dkcHashMapContains(hm, &key) == FALSE, "Does not contain key=2");

    /* GetBuffer */
    key = 1; read_val = 0;
    result = dkcHashMapGetBuffer(hm, &key, &read_val, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "GetBuffer key=1 succeeds");
    TEST_ASSERT(read_val == 100, "GetBuffer returns val=100");

    /* Find */
    key = 1; found_size = 0;
    found = dkcHashMapFind(hm, &key, &found_size);
    TEST_ASSERT(found != NULL, "Find key=1 returns non-NULL");
    TEST_ASSERT(*(int *)found == 100, "Find returns val=100");
    TEST_ASSERT(found_size == sizeof(int), "Find returns correct size");

    key = 999;
    found = dkcHashMapFind(hm, &key, NULL);
    TEST_ASSERT(found == NULL, "Find non-existent returns NULL");

    /* SetBuffer (overwrite value) */
    key = 1; val = 200;
    result = dkcHashMapSetBuffer(hm, &key, &val, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "SetBuffer key=1 val=200");
    read_val = 0;
    dkcHashMapGetBuffer(hm, &key, &read_val, sizeof(int));
    TEST_ASSERT(read_val == 200, "After SetBuffer, GetBuffer returns 200");

    /* SetBuffer non-existent */
    key = 999; val = 0;
    TEST_ASSERT(dkcHashMapSetBuffer(hm, &key, &val, sizeof(int)) == edk_Not_Found,
        "SetBuffer non-existent key returns Not_Found");

    /* Insert more pairs */
    key = 2; val = 20;
    dkcHashMapInsert(hm, &key, &val, sizeof(int));
    key = 3; val = 30;
    dkcHashMapInsert(hm, &key, &val, sizeof(int));
    key = 4; val = 40;
    dkcHashMapInsert(hm, &key, &val, sizeof(int));
    TEST_ASSERT(dkcHashMapSize(hm) == 4, "Size is 4");

    /* Erase */
    key = 3;
    TEST_ASSERT(dkcHashMapErase(hm, &key) == edk_SUCCEEDED, "Erase key=3");
    TEST_ASSERT(dkcHashMapSize(hm) == 3, "Size is 3 after erase");
    TEST_ASSERT(dkcHashMapContains(hm, &key) == FALSE, "key=3 no longer contained");

    /* Erase non-existent */
    key = 999;
    TEST_ASSERT(dkcHashMapErase(hm, &key) == edk_Not_Found, "Erase non-existent");

    /* Foreach - sum values (keys 1,2,4 => vals 200,20,40 => sum 260) */
    sum = 0;
    dkcHashMapForeach(hm, test_hashmap_sum_callback, &sum);
    TEST_ASSERT(sum == 260, "Foreach sum of values is 260");

    /* Clear */
    dkcHashMapClear(hm);
    TEST_ASSERT(dkcHashMapSize(hm) == 0, "Size is 0 after clear");
    TEST_ASSERT(dkcHashMapIsEmpty(hm) == TRUE, "Empty after clear");

    /* Mass insert to trigger rehash */
    {
        size_t initial_buckets = dkcHashMapBucketCount(hm);
        int all_ok = 1;
        for(i = 0; i < 100; i++){
            key = i;
            val = i * 10;
            dkcHashMapInsert(hm, &key, &val, sizeof(int));
        }
        TEST_ASSERT(dkcHashMapSize(hm) == 100, "100 entries after mass insert");
        TEST_ASSERT(dkcHashMapBucketCount(hm) > initial_buckets, "Rehash increased buckets");

        /* verify all entries */
        for(i = 0; i < 100; i++){
            key = i;
            read_val = -1;
            if(dkcHashMapGetBuffer(hm, &key, &read_val, sizeof(int)) != edk_SUCCEEDED
                || read_val != i * 10){
                all_ok = 0;
                break;
            }
        }
        TEST_ASSERT(all_ok == 1, "All 100 entries have correct values");

        /* mass erase */
        for(i = 0; i < 50; i++){
            key = i;
            dkcHashMapErase(hm, &key);
        }
        TEST_ASSERT(dkcHashMapSize(hm) == 50, "50 entries after partial erase");
    }

    /* Insert with NULL data */
    key = 1000;
    TEST_ASSERT(dkcHashMapInsert(hm, &key, NULL, 0) == edk_SUCCEEDED,
        "Insert with NULL data succeeds");
    TEST_ASSERT(dkcHashMapContains(hm, &key) == TRUE, "Contains key with NULL data");
    found = dkcHashMapFind(hm, &key, &found_size);
    TEST_ASSERT(found == NULL, "Find NULL-data entry returns NULL data ptr");
    TEST_ASSERT(found_size == 0, "Find NULL-data entry returns size=0");

    /* GetBuffer on NULL-data entry */
    read_val = 0;
    TEST_ASSERT(dkcHashMapGetBuffer(hm, &key, &read_val, sizeof(int)) == edk_NoValueToProcess,
        "GetBuffer on NULL-data returns NoValueToProcess");

    /* Error handling */
    TEST_ASSERT(dkcHashMapInsert(NULL, &key, &val, sizeof(int)) == edk_FAILED,
        "Insert NULL ptr fails");
    TEST_ASSERT(dkcAllocHashMap(0, 0, NULL, test_hashmap_int_compare) == NULL,
        "Alloc key_size=0 fails");
    TEST_ASSERT(dkcAllocHashMap(sizeof(int), 0, NULL, NULL) == NULL,
        "Alloc NULL compare fails");

    dkcFreeHashMap(&hm);
    TEST_ASSERT(hm == NULL, "Free hash map");

    TEST_END();
}

/*
 * Test: dkcSingleList.c
 */
void Test_SingleList(void)
{
    DKC_SINGLELIST_OBJECT *list;
    DKC_SINGLELIST *node;
    BYTE value;
    BYTE read_value;
    int count;

    TEST_BEGIN("dkcSingleList Test");

    value = 'a';
    list = dkcAllocSingleListObject(&value, sizeof(BYTE));
    TEST_ASSERT(list != NULL, "dkcAllocSingleListObject");

    value = 'b';
    list->push_back(list, &value, sizeof(BYTE));
    value = 'c';
    list->push_back(list, &value, sizeof(BYTE));

    count = 0;
    for (node = list->begin(list); !list->end(node); node = list->next(node)) {
        count++;
    }
    TEST_ASSERT(count == 3, "List has 3 nodes");

    node = list->begin(list);
    list->getbuffer(node, &read_value, sizeof(BYTE));
    TEST_ASSERT(read_value == 'a', "First node is 'a'");

    dkcFreeSingleListObject(&list);
    TEST_END();
}

/*
 * Test: dkcMemoryStream.c
 */
void Test_MemoryStream(void)
{
    DKC_MEMORYSTREAM *stream;
    char write_data[] = "Hello, dkutil_c!";
    char read_buffer[64];
    size_t read_size;

    TEST_BEGIN("dkcMemoryStream Test");

    stream = dkcAllocMemoryStream(256);
    TEST_ASSERT(stream != NULL, "dkcAllocMemoryStream");

    dkcMemoryStreamWrite(stream, write_data, strlen(write_data) + 1);
    dkcMemoryStreamSeek(stream, 0, edkcMemoryStreamSeekSet);

    memset(read_buffer, 0, sizeof(read_buffer));
    dkcMemoryStreamRead(stream, read_buffer, strlen(write_data) + 1, &read_size);
    TEST_ASSERT(strcmp(read_buffer, write_data) == 0, "Read matches write");

    TEST_ASSERT(dkcMemoryStreamTell(stream) == strlen(write_data) + 1, "Tell position");

    dkcFreeMemoryStream(&stream);
    TEST_END();
}

/*
 * Test: dkcCircularMemoryStream.c
 */
void Test_CircularMemoryStream(void)
{
    DKC_CIRCULAR_MEMORYSTREAM *cstream;
    char data1[] = "First";
    char data2[] = "Second";
    char buffer[32];
    size_t read_size;

    TEST_BEGIN("dkcCircularMemoryStream Test");

    cstream = dkcAllocCircularMemoryStream(64);
    TEST_ASSERT(cstream != NULL, "dkcAllocCircularMemoryStream");

    dkcCircularMemoryStreamWrite(cstream, data1, strlen(data1));
    dkcCircularMemoryStreamWrite(cstream, data2, strlen(data2));

    memset(buffer, 0, sizeof(buffer));
    dkcCircularMemoryStreamRead(cstream, buffer, 5, &read_size);
    TEST_ASSERT(memcmp(buffer, "First", 5) == 0, "Read 'First'");

    memset(buffer, 0, sizeof(buffer));
    dkcCircularMemoryStreamRead(cstream, buffer, 6, &read_size);
    TEST_ASSERT(memcmp(buffer, "Second", 6) == 0, "Read 'Second'");

    dkcFreeCircularMemoryStream(&cstream);
    TEST_END();
}

/*
 * Test: dkcBuffer.c
 */
void Test_Buffer(void)
{
    DKC_BUFFER *buf;
    char data[] = "Test Data";

    TEST_BEGIN("dkcBuffer Test");

    buf = dkcAllocBuffer(data, strlen(data) + 1);
    TEST_ASSERT(buf != NULL, "dkcAllocBuffer");
    TEST_ASSERT(buf->mSize == strlen(data) + 1, "Buffer size correct");
    TEST_ASSERT(strcmp((char*)buf->mBuff, data) == 0, "Buffer content correct");

    dkcFreeBuffer(&buf);
    TEST_ASSERT(buf == NULL, "Free buffer");

    TEST_END();
}

/*
 * Test: dkcMemoryPool.c
 */
void Test_MemoryPool(void)
{
    DKC_SAME_OBJECT_POOL *pool;
    void *obj1;
    void *obj2;

    TEST_BEGIN("dkcMemoryPool Test");

    pool = dkcAllocSameObjectPoolAuto(sizeof(int) * 4);
    TEST_ASSERT(pool != NULL, "dkcAllocSameObjectPoolAuto");

    obj1 = dkcSameObjectPoolAlloc(pool);
    TEST_ASSERT(obj1 != NULL, "Pool alloc 1");

    obj2 = dkcSameObjectPoolAlloc(pool);
    TEST_ASSERT(obj2 != NULL, "Pool alloc 2");
    TEST_ASSERT(obj1 != obj2, "Different allocations");

    dkcSameObjectPoolRecycle(pool, obj1);
    dkcSameObjectPoolRecycle(pool, obj2);

    dkcFreeSameObjectPool(&pool);
    TEST_END();
}

/* ========================================
 * HASH / DIGEST TESTS
 * ======================================== */

/*
 * Test: dkcCRC.c
 */
void Test_CRC(void)
{
    DKC_CRC *crc;
    const char *test_data = "123456789";
    ULONG crc_value;

    TEST_BEGIN("dkcCRC Test");

    crc = dkcAllocCRC_32right();
    TEST_ASSERT(crc != NULL, "dkcAllocCRC_32right");

    dkcCRCLoad(crc, (const BYTE*)test_data, strlen(test_data));
    crc_value = dkcCRCFinal(crc);
    TEST_ASSERT(crc_value == 0xCBF43926UL, "CRC32('123456789') = 0xCBF43926");

    dkcFreeCRC(&crc);

    /* Test CRC-CCITT */
    crc = dkcAllocCRC_ccitt_right();
    TEST_ASSERT(crc != NULL, "dkcAllocCRC_ccitt_right");
    dkcCRCLoad(crc, (const BYTE*)test_data, strlen(test_data));
    crc_value = dkcCRCFinal(crc);
    TEST_ASSERT(crc_value == 0x29B1, "CRC-CCITT('123456789') = 0x29B1");

    dkcFreeCRC(&crc);
    TEST_END();
}

/*
 * Test: dkcMD5.c
 */
void Test_MD5(void)
{
    DKC_MD5 *md5;
    char digest[MD5_STR_BUFFER_SIZE];

    TEST_BEGIN("dkcMD5 Test");

    md5 = dkcAllocMD5();
    TEST_ASSERT(md5 != NULL, "dkcAllocMD5");

    dkcMD5Load(md5, (const BYTE*)"", 0);
    dkcMD5FinalDigestStr(md5, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "d41d8cd98f00b204e9800998ecf8427e") == 0,
                "MD5('') correct");

    dkcFreeMD5(&md5);

    md5 = dkcAllocMD5();
    dkcMD5Load(md5, (const BYTE*)"abc", 3);
    dkcMD5FinalDigestStr(md5, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "900150983cd24fb0d6963f7d28e17f72") == 0,
                "MD5('abc') correct");

    dkcFreeMD5(&md5);
    TEST_END();
}

/*
 * Test: dkcMD4.c
 */
void Test_MD4(void)
{
    DKC_MD4 *md4;
    char digest[33];

    TEST_BEGIN("dkcMD4 Test");

    md4 = dkcAllocMD4();
    TEST_ASSERT(md4 != NULL, "dkcAllocMD4");

    dkcMD4Load(md4, (const BYTE*)"abc", 3);
    dkcMD4FinalDigestStr(md4, digest, sizeof(digest));
    /* MD4("abc") = a448017aaf21d8525fc10ae87aa6729d */
    TEST_ASSERT(strcmp(digest, "a448017aaf21d8525fc10ae87aa6729d") == 0,
                "MD4('abc') correct");

    dkcFreeMD4(&md4);
    TEST_END();
}

/*
 * Test: dkcMD2.c
 */
void Test_MD2(void)
{
    DKC_MD2 *md2;
    char digest[33];

    TEST_BEGIN("dkcMD2 Test");

    md2 = dkcAllocMD2();
    TEST_ASSERT(md2 != NULL, "dkcAllocMD2");

    dkcMD2Load(md2, (const BYTE*)"abc", 3);
    dkcMD2FinalDigestStr(md2, digest, sizeof(digest));
    /* MD2("abc") = da853b0d3f88d99b30283a69e6ded6bb */
    TEST_ASSERT(strcmp(digest, "da853b0d3f88d99b30283a69e6ded6bb") == 0,
                "MD2('abc') correct");

    dkcFreeMD2(&md2);
    TEST_END();
}

/*
 * Test: dkcSHA1.c
 */
void Test_SHA1(void)
{
    DKC_SHA1 *sha1;
    char digest[SHA1_STR_BUFFER_SIZE];

    TEST_BEGIN("dkcSHA1 Test");

    sha1 = dkcAllocSHA1();
    TEST_ASSERT(sha1 != NULL, "dkcAllocSHA1");

    dkcSHA1Load(sha1, (const BYTE*)"abc", 3);
    dkcSHA1FinalDigestStr(sha1, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "a9993e364706816aba3e25717850c26c9cd0d89d") == 0,
                "SHA1('abc') correct");

    dkcFreeSHA1(&sha1);
    TEST_END();
}

/*
 * Test: dkcSHA256.c via dkcHash.c SHO interface
 */
void Test_SHA256(void)
{
    DKC_SECURE_HASH_OBJECT *sho;
    char digest[128];

    TEST_BEGIN("dkcSHA256 Test");

    sho = dkcAllocSHO(edkcSH_SHA256);
    TEST_ASSERT(sho != NULL, "dkcAllocSHO(SHA256)");

    dkcSHOInit(sho);
    dkcSHOLoad(sho, (const BYTE*)"abc", 3);
    dkcSHOFinal(sho);
    dkcSHODigestStr(sho, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") == 0,
                "SHA256('abc') correct");

    dkcFreeSHO(&sho);
    TEST_END();
}

/*
 * Test: dkcSHA384.c
 */
void Test_SHA384(void)
{
    DKC_SECURE_HASH_OBJECT *sho;
    char digest[128];

    TEST_BEGIN("dkcSHA384 Test");

    sho = dkcAllocSHO(edkcSH_SHA384);
    TEST_ASSERT(sho != NULL, "dkcAllocSHO(SHA384)");

    dkcSHOInit(sho);
    dkcSHOLoad(sho, (const BYTE*)"abc", 3);
    dkcSHOFinal(sho);
    dkcSHODigestStr(sho, digest, sizeof(digest));
    /* SHA384("abc") = cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7 */
    TEST_ASSERT(strcmp(digest, "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7") == 0,
                "SHA384('abc') correct");

    dkcFreeSHO(&sho);
    TEST_END();
}

/*
 * Test: dkcSHA512.c
 */
void Test_SHA512(void)
{
    DKC_SECURE_HASH_OBJECT *sho;
    char digest[150];

    TEST_BEGIN("dkcSHA512 Test");

    sho = dkcAllocSHO(edkcSH_SHA512);
    TEST_ASSERT(sho != NULL, "dkcAllocSHO(SHA512)");

    dkcSHOInit(sho);
    dkcSHOLoad(sho, (const BYTE*)"abc", 3);
    dkcSHOFinal(sho);
    dkcSHODigestStr(sho, digest, sizeof(digest));
    /* SHA512("abc") starts with ddaf35a193617aba... */
    TEST_ASSERT(strncmp(digest, "ddaf35a193617aba", 16) == 0,
                "SHA512('abc') prefix correct");

    dkcFreeSHO(&sho);
    TEST_END();
}

/*
 * Test: dkcHMAC.c
 */
void Test_HMAC(void)
{
    DKC_HMAC *hmac;
    char digest[SHA1_STR_BUFFER_SIZE];
    const BYTE key_md5[] = {0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
                            0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b};
    const BYTE key_sha1[] = {0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
                             0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
                             0x0b,0x0b,0x0b,0x0b};
    const char *data = "Hi There";

    TEST_BEGIN("dkcHMAC Test");

    /* HMAC-MD5 */
    hmac = dkcAllocHMAC(edkcSH_MD5);
    TEST_ASSERT(hmac != NULL, "dkcAllocHMAC(MD5)");
    dkcHMACStr(hmac, (const BYTE*)data, strlen(data), key_md5, 16, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "9294727a3638bb1c13f48ef8158bfc9d") == 0,
                "HMAC-MD5 test vector");
    dkcFreeHMAC(&hmac);

    /* HMAC-SHA1 */
    hmac = dkcAllocHMAC(edkcSH_SHA1);
    TEST_ASSERT(hmac != NULL, "dkcAllocHMAC(SHA1)");
    dkcHMACStr(hmac, (const BYTE*)data, strlen(data), key_sha1, 20, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "b617318655057264e28bc0b6fb378c8ef146be00") == 0,
                "HMAC-SHA1 test vector");
    dkcFreeHMAC(&hmac);

    TEST_END();
}

/* ========================================
 * CIPHER TESTS
 * ======================================== */

/*
 * Test: dkcArcfour.c (RC4)
 */
void Test_Arcfour(void)
{
    DKC_ARCFOUR_STATE *rc4;
    BYTE plaintext[16] = "RC4 Test Data!!";
    BYTE ciphertext[16];
    BYTE decrypted[16];
    BYTE key[] = "Secret";

    TEST_BEGIN("dkcArcfour (RC4) Test");

    rc4 = dkcAllocArcfour(key, strlen((char*)key));
    TEST_ASSERT(rc4 != NULL, "dkcAllocArcfour");

    dkcArcfourEncrypt(rc4, ciphertext, 16, plaintext, 16);
    TEST_ASSERT(memcmp(plaintext, ciphertext, 16) != 0, "Ciphertext differs");

    dkcFreeArcfour(&rc4);

    /* Decrypt */
    rc4 = dkcAllocArcfour(key, strlen((char*)key));
    dkcArcfourDecrypt(rc4, decrypted, 16, ciphertext, 16);
    TEST_ASSERT(memcmp(plaintext, decrypted, 16) == 0, "Decrypt matches original");

    dkcFreeArcfour(&rc4);
    TEST_END();
}

/*
 * Test: dkcBlowfish.c
 */
void Test_Blowfish(void)
{
    DKC_BLOWFISH *bf;
    BYTE key[] = "TestKey123";
    BYTE plaintext[16] = "Hello Blowfish!";
    BYTE ciphertext[16];
    BYTE decrypted[16];

    TEST_BEGIN("dkcBlowfish Test");

    bf = dkcAllocBlowfish(key, (int)strlen((char*)key));
    TEST_ASSERT(bf != NULL, "dkcAllocBlowfish");

    dkcBlowfishEncrypt(bf, plaintext, ciphertext, 16);
    TEST_ASSERT(memcmp(plaintext, ciphertext, 16) != 0, "Ciphertext differs");

    dkcBlowfishDecrypt(bf, ciphertext, decrypted, 16);
    TEST_ASSERT(memcmp(plaintext, decrypted, 16) == 0, "Decrypt matches original");

    dkcFreeBlowfish(&bf);
    TEST_END();
}

/*
 * Test: dkcRijndael.c (AES)
 */
void Test_Rijndael(void)
{
    DKC_RIJNDAEL *enc;
    DKC_RIJNDAEL *dec;
    BYTE key[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    BYTE plaintext[16] = "AES Test Block!";
    BYTE ciphertext[32];
    BYTE decrypted[32];
    int enc_len;
    int dec_len;

    TEST_BEGIN("dkcRijndael (AES) Test");

    enc = dkcAllocRijndaelEncrypt(key, 16);
    TEST_ASSERT(enc != NULL, "dkcAllocRijndaelEncrypt");

    dec = dkcAllocRijndaelDecrypt(key, 16);
    TEST_ASSERT(dec != NULL, "dkcAllocRijndaelDecrypt");

    enc_len = dkcRijndaelPadEncrypt(enc, ciphertext, sizeof(ciphertext), plaintext, 16);
    TEST_ASSERT(enc_len > 0, "Encrypt returns positive length");

    dec_len = dkcRijndaelPadDecrypt(dec, decrypted, sizeof(decrypted), ciphertext, enc_len);
    TEST_ASSERT(dec_len == 16, "Decrypt returns original length");
    TEST_ASSERT(memcmp(plaintext, decrypted, 16) == 0, "Decrypt matches original");

    dkcFreeRijndael(&enc);
    dkcFreeRijndael(&dec);
    TEST_END();
}

/*
 * Test: dkcSNOW20.c
 */
void Test_SNOW20(void)
{
    DKC_SNOW2 *snow;
    BYTE key[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    BYTE plaintext[32];
    BYTE ciphertext[32];
    BYTE decrypted[32];
    size_t i;

    TEST_BEGIN("dkcSNOW20 Test");

    for (i = 0; i < 32; i++) {
        plaintext[i] = (BYTE)i;
    }
    memcpy(ciphertext, plaintext, 32);

    snow = dkcAllocSNOW2(key, 16, 0, 0, 0, 0);
    TEST_ASSERT(snow != NULL, "dkcAllocSNOW2");

    dkcSNOW2EncryptNoDestDOE(snow, ciphertext, 32);
    TEST_ASSERT(memcmp(plaintext, ciphertext, 32) != 0, "Ciphertext differs");

    dkcFreeSNOW2(&snow);

    snow = dkcAllocSNOW2(key, 16, 0, 0, 0, 0);
    memcpy(decrypted, ciphertext, 32);
    dkcSNOW2DecryptNoDestDOE(snow, decrypted, 32);
    TEST_ASSERT(memcmp(plaintext, decrypted, 32) == 0, "Decrypt matches original");

    dkcFreeSNOW2(&snow);
    TEST_END();
}

/*
 * Test: dkcHC256.c
 */
void Test_HC256(void)
{
    DKC_HC256 *hc;
    BYTE key[32];
    uint32 iv[8];
    BYTE plaintext[32];
    BYTE ciphertext[32];
    BYTE decrypted[32];
    size_t i;

    TEST_BEGIN("dkcHC256 Test");

    for (i = 0; i < 32; i++) {
        key[i] = (BYTE)i;
        plaintext[i] = (BYTE)(i * 2);
    }
    for (i = 0; i < 8; i++) {
        iv[i] = (uint32)i;
    }

    hc = dkcAllocHC256Const(key, 32, iv, 32);
    TEST_ASSERT(hc != NULL, "dkcAllocHC256Const");

    memcpy(ciphertext, plaintext, 32);
    for (i = 0; i < 32; i++) {
        ciphertext[i] ^= dkcHC256Process(hc);
    }
    TEST_ASSERT(memcmp(plaintext, ciphertext, 32) != 0, "Ciphertext differs");

    dkcFreeHC256(&hc);

    /* Decrypt */
    hc = dkcAllocHC256Const(key, 32, iv, 32);
    memcpy(decrypted, ciphertext, 32);
    for (i = 0; i < 32; i++) {
        decrypted[i] ^= dkcHC256Process(hc);
    }
    TEST_ASSERT(memcmp(plaintext, decrypted, 32) == 0, "Decrypt matches original");

    dkcFreeHC256(&hc);
    TEST_END();
}

/* ========================================
 * TREE TESTS
 * ======================================== */

/*
 * Compare function for tree int keys (shared by 2Tree and BTree)
 */
static int compare_2tree_int(const void *a, const void *b)
{
    int va = *(const int*)a;
    int vb = *(const int*)b;
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}

/*
 * Foreach callback for BTree: counts calls and verifies sorted order
 */
static int g_btree_foreach_prev;
static int g_btree_foreach_count;
static int g_btree_foreach_sorted;

static BOOL WINAPI btree_foreach_check_sorted(
    const void *key, void *data, size_t data_size, void *user)
{
    int k = *(const int *)key;
    (void)data; (void)data_size; (void)user;
    if (g_btree_foreach_count > 0 && k <= g_btree_foreach_prev) {
        g_btree_foreach_sorted = 0;
    }
    g_btree_foreach_prev = k;
    g_btree_foreach_count++;
    return TRUE;
}

/*
 * Test: dkcBTree.c (B-Tree)
 */
void Test_BTree(void)
{
    DKC_BTREE_ROOT *btree;
    DKC_BTREE_SEARCH_RESULT sr;
    int key, data_val, read_data;
    int result;
    int i;
    void *found_key;

    TEST_BEGIN("dkcBTree Test");

    /* Basic alloc/free */
    btree = dkcAllocBTreeRoot(sizeof(int), 2, compare_2tree_int, 0);
    TEST_ASSERT(btree != NULL, "dkcAllocBTreeRoot (t=2)");
    TEST_ASSERT(dkcBTreeIsEmpty(btree) == TRUE, "New BTree is empty");
    TEST_ASSERT(dkcBTreeSize(btree) == 0, "Size is 0");

    /* Invalid params */
    TEST_ASSERT(dkcAllocBTreeRoot(0, 2, compare_2tree_int, 0) == NULL,
        "key_size=0 returns NULL");
    TEST_ASSERT(dkcAllocBTreeRoot(sizeof(int), 1, compare_2tree_int, 0) == NULL,
        "min_degree=1 returns NULL");
    TEST_ASSERT(dkcAllocBTreeRoot(sizeof(int), 2, NULL, 0) == NULL,
        "compare=NULL returns NULL");

    /* Insert several keys */
    key = 50; data_val = 500;
    result = dkcBTreeInsert(btree, &key, &data_val, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert 50");

    key = 25; data_val = 250;
    result = dkcBTreeInsert(btree, &key, &data_val, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert 25");

    key = 75; data_val = 750;
    result = dkcBTreeInsert(btree, &key, &data_val, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert 75");

    key = 10; data_val = 100;
    dkcBTreeInsert(btree, &key, &data_val, sizeof(int));
    key = 30; data_val = 300;
    dkcBTreeInsert(btree, &key, &data_val, sizeof(int));

    TEST_ASSERT(dkcBTreeSize(btree) == 5, "Size is 5 after inserts");
    TEST_ASSERT(dkcBTreeIsEmpty(btree) == FALSE, "BTree is not empty");

    /* Duplicate rejection */
    key = 50; data_val = 999;
    result = dkcBTreeInsert(btree, &key, &data_val, sizeof(int));
    TEST_ASSERT(result == edk_FAILED, "Duplicate key 50 rejected");
    TEST_ASSERT(dkcBTreeSize(btree) == 5, "Size unchanged after dup");

    /* Find */
    key = 75;
    sr = dkcBTreeFind(btree, &key);
    TEST_ASSERT(sr.node != NULL, "Find key 75");

    key = 999;
    sr = dkcBTreeFind(btree, &key);
    TEST_ASSERT(sr.node == NULL, "Key 999 not found");

    /* GetBuffer */
    key = 50;
    result = dkcBTreeGetBuffer(btree, &key, &read_data, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED && read_data == 500,
        "GetBuffer key 50 = 500");

    /* SetBuffer */
    data_val = 5555;
    result = dkcBTreeSetBuffer(btree, &key, &data_val, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "SetBuffer key 50");
    result = dkcBTreeGetBuffer(btree, &key, &read_data, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED && read_data == 5555,
        "GetBuffer after Set = 5555");

    /* FindMin / FindMax */
    found_key = dkcBTreeFindMin(btree);
    TEST_ASSERT(found_key != NULL && *(int *)found_key == 10,
        "FindMin = 10");
    found_key = dkcBTreeFindMax(btree);
    TEST_ASSERT(found_key != NULL && *(int *)found_key == 75,
        "FindMax = 75");

    /* FindMinimalGreater / FindMaximumLess */
    key = 25;
    found_key = dkcBTreeFindMinimalGreater(btree, &key);
    TEST_ASSERT(found_key != NULL && *(int *)found_key == 30,
        "FindMinimalGreater(25) = 30");

    found_key = dkcBTreeFindMaximumLess(btree, &key);
    TEST_ASSERT(found_key != NULL && *(int *)found_key == 10,
        "FindMaximumLess(25) = 10");

    key = 10;
    found_key = dkcBTreeFindMaximumLess(btree, &key);
    TEST_ASSERT(found_key == NULL, "FindMaximumLess(10) = NULL (no smaller)");

    key = 75;
    found_key = dkcBTreeFindMinimalGreater(btree, &key);
    TEST_ASSERT(found_key == NULL, "FindMinimalGreater(75) = NULL (no larger)");

    /* Delete */
    key = 25;
    result = dkcBTreeErase(btree, &key);
    TEST_ASSERT(result == edk_SUCCEEDED, "Erase key 25");
    TEST_ASSERT(dkcBTreeSize(btree) == 4, "Size is 4 after erase");

    sr = dkcBTreeFind(btree, &key);
    TEST_ASSERT(sr.node == NULL, "Key 25 not found after erase");

    key = 999;
    result = dkcBTreeErase(btree, &key);
    TEST_ASSERT(result == edk_Not_Found, "Erase non-existent key");

    dkcFreeBTreeRoot(&btree);
    TEST_ASSERT(btree == NULL, "Free BTree");

    /* Mass insert test (triggers splits) */
    btree = dkcAllocBTreeRoot(sizeof(int), 3, compare_2tree_int, 0);
    TEST_ASSERT(btree != NULL, "Alloc BTree (t=3) for mass test");

    for (i = 0; i < 100; i++) {
        key = i;
        data_val = i * 10;
        result = dkcBTreeInsert(btree, &key, &data_val, sizeof(int));
        if (result != edk_SUCCEEDED) break;
    }
    TEST_ASSERT(dkcBTreeSize(btree) == 100, "Mass insert 100 keys");

    /* Verify all keys findable */
    for (i = 0; i < 100; i++) {
        key = i;
        sr = dkcBTreeFind(btree, &key);
        if (sr.node == NULL) break;
    }
    TEST_ASSERT(i == 100, "All 100 keys found");

    /* Foreach - verify sorted order */
    g_btree_foreach_count = 0;
    g_btree_foreach_sorted = 1;
    g_btree_foreach_prev = -1;
    dkcBTreeForeach(btree, btree_foreach_check_sorted, NULL);
    TEST_ASSERT(g_btree_foreach_count == 100, "Foreach visits 100 keys");
    TEST_ASSERT(g_btree_foreach_sorted == 1, "Foreach in sorted order");

    /* Mass delete */
    for (i = 0; i < 50; i++) {
        key = i * 2; /* delete even numbers */
        dkcBTreeErase(btree, &key);
    }
    TEST_ASSERT(dkcBTreeSize(btree) == 50, "50 keys after mass delete");

    /* Verify remaining keys */
    g_btree_foreach_count = 0;
    g_btree_foreach_sorted = 1;
    g_btree_foreach_prev = -1;
    dkcBTreeForeach(btree, btree_foreach_check_sorted, NULL);
    TEST_ASSERT(g_btree_foreach_count == 50, "Foreach visits 50 remaining");
    TEST_ASSERT(g_btree_foreach_sorted == 1, "Remaining still sorted");

    dkcFreeBTreeRoot(&btree);

    TEST_END();
}

/*
 * Test: dkc2Tree.c (Binary Tree)
 */
void Test_2Tree(void)
{
    DKC_2TREE_ROOT *tree;
    DKC_2TREE_NODE *node;
    int key1, key2, key3;
    int data1, data2, data3;
    int read_data;
    int result;

    TEST_BEGIN("dkc2Tree (Binary Tree) Test");

    tree = dkcAlloc2TreeRoot(sizeof(int), 10, compare_2tree_int, 100);
    TEST_ASSERT(tree != NULL, "dkcAlloc2TreeRoot");

    key1 = 50; data1 = 500;
    key2 = 25; data2 = 250;
    key3 = 75; data3 = 750;

    result = dkc2TreeInsert(tree, &key1, &data1, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert key 50");

    result = dkc2TreeInsert(tree, &key2, &data2, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert key 25");

    result = dkc2TreeInsert(tree, &key3, &data3, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert key 75");

    /* Find existing key */
    node = dkc2TreeFindEqual(tree, &key1);
    TEST_ASSERT(node != NULL, "Find key 50");

    if (node != NULL) {
        dkc2TreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 500, "Key 50 has data 500");
    }

    node = dkc2TreeFindEqual(tree, &key2);
    TEST_ASSERT(node != NULL, "Find key 25");

    /* Find non-existing key */
    key1 = 999;
    node = dkc2TreeFindEqual(tree, &key1);
    TEST_ASSERT(node == NULL, "Key 999 not found");

    dkcFree2TreeRoot(&tree);
    TEST_ASSERT(tree == NULL, "Free 2Tree");

    TEST_END();
}

/*
 * Test: dkcRedBlackTree.c
 */
void Test_RedBlackTree(void)
{
    DKC_RB_TREE_ROOT *rbtree;
    DKC_RED_BLACK_NODE *node;
    rb_tree_key_type key;
    rb_tree_data_type data_val;
    int test_data1, test_data2, test_data3;

    TEST_BEGIN("dkcRedBlackTree Test");

    rbtree = dkcAllocRedBlackTreeRoot(100, 50, NULL);
    TEST_ASSERT(rbtree != NULL, "dkcAllocRedBlackTreeRoot");

    test_data1 = 100;
    test_data2 = 200;
    test_data3 = 300;

    /* Insert nodes */
    node = dkcRedBlackTree_insertNode(rbtree, 10, &test_data1);
    TEST_ASSERT(node != NULL, "Insert key 10");

    node = dkcRedBlackTree_insertNode(rbtree, 5, &test_data2);
    TEST_ASSERT(node != NULL, "Insert key 5");

    node = dkcRedBlackTree_insertNode(rbtree, 15, &test_data3);
    TEST_ASSERT(node != NULL, "Insert key 15");

    TEST_ASSERT(rbtree->node_count == 3, "Node count is 3");

    /* Find nodes */
    node = dkcRedBlackTree_findNode(rbtree, 10);
    TEST_ASSERT(node != NULL, "Find key 10");
    if (node != NULL) {
        data_val = node->data;
        TEST_ASSERT(*(int*)data_val == 100, "Key 10 has data 100");
    }

    node = dkcRedBlackTree_findNode(rbtree, 5);
    TEST_ASSERT(node != NULL, "Find key 5");

    node = dkcRedBlackTree_findNode(rbtree, 999);
    TEST_ASSERT(node == NULL, "Key 999 not found");

    /* Delete a node */
    node = dkcRedBlackTree_findNode(rbtree, 10);
    if (node != NULL) {
        key = node->key;
        dkcRedBlackTree_deleteNode(rbtree, node, &data_val);
        TEST_ASSERT(rbtree->node_count == 2, "Node count is 2 after delete");
    }

    dkcFreeRedBlackTreeRoot(&rbtree);
    TEST_ASSERT(rbtree == NULL, "Free RedBlackTree");

    TEST_END();
}

/* ========================================
 * COMPRESSION TESTS
 * ======================================== */

/*
 * Test: dkcLZSS.c
 */
void Test_LZSS(void)
{
    DKC_LZSS *lzss;
    DKC_LZSS_HEADER header;
    BYTE original[] = "AAAAAAAAAAAABBBBBBBBBBBBCCCCCCCCCCCCDDDDDDDDDDDD";
    BYTE compressed[256];
    BYTE decompressed[256];
    int result;

    TEST_BEGIN("dkcLZSS Test");

    lzss = dkcAllocLZSS();
    TEST_ASSERT(lzss != NULL, "dkcAllocLZSS");

    result = dkcLZSSEncode(lzss, &header, compressed, sizeof(compressed),
                           original, sizeof(original), sizeof(compressed), dkcdLZSS_SIGNATURE);
    TEST_ASSERT(result == edk_SUCCEEDED || result == edk_NoValueToProcess, "LZSS encode");

    if (result == edk_SUCCEEDED) {
        TEST_ASSERT(header.mCompressedSize < header.mOriginSize, "Compressed smaller");

        memset(decompressed, 0, sizeof(decompressed));
        result = dkcLZSSDecode(lzss, &header, decompressed, sizeof(decompressed),
                               compressed, header.mCompressedSize, dkcdLZSS_SIGNATURE);
        TEST_ASSERT(result == edk_SUCCEEDED, "LZSS decode");
        TEST_ASSERT(memcmp(original, decompressed, sizeof(original)) == 0, "Matches original");
    }

    dkcFreeLZSS(&lzss);
    TEST_END();
}

/*
 * Test: dkcLZW.c
 */
void Test_LZW(void)
{
    DKC_LZW *lzw;
    DKC_LZW_HEADER header;
    BYTE original[] = "ABABABABABABABABABABABABABABABABABABABABABAB";
    BYTE compressed[256];
    BYTE decompressed[256];
    int result;

    TEST_BEGIN("dkcLZW Test");

    lzw = dkcAllocLZW(256);
    TEST_ASSERT(lzw != NULL, "dkcAllocLZW");

    result = dkcLZWEncode(lzw, &header, compressed, sizeof(compressed),
                          original, sizeof(original), sizeof(compressed),
                          dkcdLZW_SIGNATURE, edkcLZW_Default);
    TEST_ASSERT(result == edk_SUCCEEDED || result == edk_NoValueToProcess, "LZW encode");

    if (result == edk_SUCCEEDED) {
        TEST_ASSERT(header.mSignature == dkcdLZW_SIGNATURE, "Signature correct");

        memset(decompressed, 0, sizeof(decompressed));
        result = dkcLZWDecode(lzw, &header, decompressed, header.mOriginSize,
                              compressed, header.mCompressedSize, dkcdLZW_SIGNATURE);
        TEST_ASSERT(result == edk_SUCCEEDED, "LZW decode");
        TEST_ASSERT(memcmp(original, decompressed, sizeof(original)) == 0, "Matches original");
    }

    dkcFreeLZW(&lzw);
    TEST_END();
}

/*
 * Test: dkcRLE.c (PackBits algorithm)
 */
void Test_RLE_PackBits(void)
{
    DKC_RLE_PACKBITS_HEADER header;
    BYTE original[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55,
                       0x12, 0x34, 0x56, 0x78, 0x11, 0x11, 0x11, 0x11};
    BYTE compressed[64];
    BYTE decompressed[64];
    int result;

    TEST_BEGIN("dkcRLE (PackBits) Test");

    memset(&header, 0, sizeof(header));
    result = dkcRLEPackBitsEncode(&header, compressed, sizeof(compressed),
                                   original, sizeof(original), 2);
    TEST_ASSERT(result == edk_SUCCEEDED, "PackBits encode");

    if (result == edk_SUCCEEDED) {
        TEST_ASSERT(header.mOriginSize == sizeof(original), "Origin size correct");

        memset(decompressed, 0, sizeof(decompressed));
        result = dkcRLEPackBitsDecode(&header, decompressed, sizeof(decompressed),
                                       compressed, header.mCompressedSize);
        TEST_ASSERT(result == edk_SUCCEEDED, "PackBits decode");
        TEST_ASSERT(memcmp(original, decompressed, sizeof(original)) == 0, "Matches original");
    }

    TEST_END();
}

/*
 * Test: dkcRegex.c (Regular Expression)
 */
void Test_Regex(void)
{
    DKC_REGEX *regex;
    BOOL match_result;
    ULONG mbg, med;

    TEST_BEGIN("dkcRegex Test");

    /* Test simple match */
    match_result = dkcRegularExpressionMatch(L"abc", L"abc", FALSE);
    TEST_ASSERT(match_result == TRUE, "Match 'abc' with 'abc'");

    match_result = dkcRegularExpressionMatch(L"abc", L"ABC", FALSE);
    TEST_ASSERT(match_result == FALSE, "No match 'abc' with 'ABC' (case sensitive)");

    match_result = dkcRegularExpressionMatch(L"abc", L"ABC", TRUE);
    TEST_ASSERT(match_result == TRUE, "Match 'abc' with 'ABC' (case insensitive)");

    /* Test wildcard match */
    match_result = dkcRegularExpressionMatch(L"a.c", L"abc", FALSE);
    TEST_ASSERT(match_result == TRUE, "Match 'a.c' with 'abc'");

    match_result = dkcRegularExpressionMatch(L"a*", L"aaaa", FALSE);
    TEST_ASSERT(match_result == TRUE, "Match 'a*' with 'aaaa'");

    /* Test search with allocated object */
    regex = dkcAllocRegex(L"test", FALSE, FALSE);
    TEST_ASSERT(regex != NULL, "dkcAllocRegex");

    if (regex != NULL) {
        match_result = dkcRegexSearch(regex, L"This is a test string", 21, 0, &mbg, &med);
        TEST_ASSERT(match_result == TRUE, "Search finds 'test' in string");

        dkcFreeRegex(&regex);
    }

    TEST_END();
}

/*
 * Vernam cipher callback: simple XorShift RNG
 */
static int vernam_rng_callback(ULONG *seed, ULONG max)
{
    ULONG x = *seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *seed = x;
    return (int)(x % max);
}

/*
 * Test: dkcVernam.c (Vernam Cipher)
 */
/*
 * Test: dkcCamellia.c
 */
void Test_Camellia(void)
{
    DKC_CAMELLIA *cam;
    unsigned char key128[16] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,
                                0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10};
    unsigned char key256[32] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,
                                0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10,
                                0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
                                0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
    unsigned char plaintext[32];
    unsigned char ciphertext[32];
    unsigned char decrypted[32];
    unsigned char inplace[32];
    int result;
    size_t i;

    TEST_BEGIN("dkcCamellia Test");

    /* Fill plaintext with test data */
    for (i = 0; i < 32; i++) {
        plaintext[i] = (unsigned char)i;
    }

    /* --- 128-bit key test --- */
    cam = dkcAllocCamellia(key128, 16);
    TEST_ASSERT(cam != NULL, "dkcAllocCamellia (128-bit key)");

    result = dkcCamelliaEncrypt(cam, ciphertext, 32, plaintext, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Encrypt 128-bit succeeds");
    TEST_ASSERT(memcmp(plaintext, ciphertext, 32) != 0, "Ciphertext differs from plaintext (128-bit)");

    result = dkcCamelliaDecrypt(cam, decrypted, 32, ciphertext, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Decrypt 128-bit succeeds");
    TEST_ASSERT(memcmp(plaintext, decrypted, 32) == 0, "Decrypt matches original (128-bit)");

    /* In-place encrypt/decrypt */
    memcpy(inplace, plaintext, 32);
    dkcCamelliaEncryptNoDest(cam, inplace, 32);
    TEST_ASSERT(memcmp(inplace, ciphertext, 32) == 0, "In-place encrypt matches (128-bit)");
    dkcCamelliaDecryptNoDest(cam, inplace, 32);
    TEST_ASSERT(memcmp(inplace, plaintext, 32) == 0, "In-place decrypt matches (128-bit)");

    dkcFreeCamellia(&cam);
    TEST_ASSERT(cam == NULL, "Free camellia (128-bit)");

    /* --- 256-bit key test --- */
    cam = dkcAllocCamellia(key256, 32);
    TEST_ASSERT(cam != NULL, "dkcAllocCamellia (256-bit key)");

    result = dkcCamelliaEncrypt(cam, ciphertext, 32, plaintext, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Encrypt 256-bit succeeds");
    TEST_ASSERT(memcmp(plaintext, ciphertext, 32) != 0, "Ciphertext differs from plaintext (256-bit)");

    result = dkcCamelliaDecrypt(cam, decrypted, 32, ciphertext, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Decrypt 256-bit succeeds");
    TEST_ASSERT(memcmp(plaintext, decrypted, 32) == 0, "Decrypt matches original (256-bit)");

    dkcFreeCamellia(&cam);

    /* --- Invalid key size test --- */
    cam = dkcAllocCamellia(key128, 15);
    TEST_ASSERT(cam == NULL, "Invalid key size (15) returns NULL");

    /* --- Size not multiple of 16 test --- */
    cam = dkcAllocCamellia(key128, 16);
    result = dkcCamelliaEncrypt(cam, ciphertext, 32, plaintext, 17);
    TEST_ASSERT(result == edk_ArgumentException, "Non-block-aligned size rejected");
    dkcFreeCamellia(&cam);

    TEST_END();
}

/*
 * Test: dkcVernam.c
 */
void Test_Vernam(void)
{
    BYTE plaintext[32];
    BYTE ciphertext[32];
    BYTE decrypted[32];
    BYTE key[] = "SecretKey12345";
    ULONG seed;
    size_t i;

    TEST_BEGIN("dkcVernam Test");

    /* Initialize plaintext */
    for (i = 0; i < 32; i++) {
        plaintext[i] = (BYTE)('A' + (i % 26));
    }
    memcpy(ciphertext, plaintext, 32);

    /* Encrypt */
    seed = 0x12345678;
    dkcVernamEncrypt(ciphertext, 32, key, sizeof(key) - 1, seed, vernam_rng_callback);
    TEST_ASSERT(memcmp(plaintext, ciphertext, 32) != 0, "Ciphertext differs from plaintext");

    /* Decrypt */
    memcpy(decrypted, ciphertext, 32);
    seed = 0x12345678;
    dkcVernamDecrypt(decrypted, 32, key, sizeof(key) - 1, seed, vernam_rng_callback);
    TEST_ASSERT(memcmp(plaintext, decrypted, 32) == 0, "Decrypted matches original");

    /* Test easy vernam (uses RNG directly as key) */
    memcpy(ciphertext, plaintext, 32);
    seed = 0xABCDEF01;
    dkcEasyVernamEncrypt(ciphertext, 32, seed, vernam_rng_callback);
    TEST_ASSERT(memcmp(plaintext, ciphertext, 32) != 0, "EasyVernam ciphertext differs");

    memcpy(decrypted, ciphertext, 32);
    seed = 0xABCDEF01;
    dkcEasyVernamDecrypt(decrypted, 32, seed, vernam_rng_callback);
    TEST_ASSERT(memcmp(plaintext, decrypted, 32) == 0, "EasyVernam decrypted matches");

    TEST_END();
}

/* ========================================
 * UTILITY TESTS
 * ======================================== */

/*
 * Test: dkcOSIndependent.c (Memory)
 */
void Test_Memory(void)
{
    void *ptr;

    TEST_BEGIN("dkcOSIndependent (Memory) Test");

    ptr = dkcAllocate(1024);
    TEST_ASSERT(ptr != NULL, "dkcAllocate");

    memset(ptr, 0xAA, 1024);
    TEST_ASSERT(((BYTE*)ptr)[0] == 0xAA, "Memory writable (start)");
    TEST_ASSERT(((BYTE*)ptr)[1023] == 0xAA, "Memory writable (end)");

    dkcFree(&ptr);
    TEST_ASSERT(ptr == NULL, "dkcFree sets NULL");

    TEST_END();
}

/*
 * Test: dkcSort.c
 */
static int compare_int(const void *a, const void *b)
{
    return (*(int*)a) - (*(int*)b);
}

void Test_Sort(void)
{
    int arr1[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr2[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr3[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int sorted[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    TEST_BEGIN("dkcSort Test");

    dkcShellSort(arr1, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr1, sorted, sizeof(sorted)) == 0, "ShellSort");

    dkcCombSort(arr2, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr2, sorted, sizeof(sorted)) == 0, "CombSort");

    dkcBubbleSort(arr3, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr3, sorted, sizeof(sorted)) == 0, "BubbleSort");

    TEST_END();
}

/*
 * Test: dkcBit.c
 */
void Test_Bit(void)
{
    TEST_BEGIN("dkcBit Test");

    /* Population count */
    TEST_ASSERT(dkcPOP(0x00000000) == 0, "POP(0) = 0");
    TEST_ASSERT(dkcPOP(0x00000001) == 1, "POP(1) = 1");
    TEST_ASSERT(dkcPOP(0x0000000F) == 4, "POP(0xF) = 4");
    TEST_ASSERT(dkcPOP(0xFFFFFFFF) == 32, "POP(0xFFFFFFFF) = 32");

    /* Number of leading zeros */
    TEST_ASSERT(dkcNLZ(0x80000000) == 0, "NLZ(0x80000000) = 0");
    TEST_ASSERT(dkcNLZ(0x00000001) == 31, "NLZ(1) = 31");

    TEST_END();
}

/*
 * Test: dkcMath.c (XorShift RNG)
 */
void Test_Math(void)
{
    DKC_XORSHIFT_RNG rng;
    uint32 r1, r2;

    TEST_BEGIN("dkcMath Test");

    dkcXorShiftRNG128DefaultInit(&rng);
    r1 = dkcXorShiftRNG128(&rng);
    r2 = dkcXorShiftRNG128(&rng);
    TEST_ASSERT(r1 != r2, "XorShift produces different values");
    TEST_ASSERT(r1 != 0 && r2 != 0, "XorShift non-zero output");

    TEST_END();
}

/*
 * Test: dkcString.c (SJIS functions)
 */
void Test_String(void)
{
    const char *test_str = "test\\path";
    size_t pos;

    TEST_BEGIN("dkcString Test");

    /* Test SJIS character search */
    pos = dkcSJIS_StrChrSearch(test_str, '\\');
    TEST_ASSERT(pos == 4, "SJIS_StrChrSearch finds backslash");

    pos = dkcSJIS_StrChrSearch(test_str, 'z');
    TEST_ASSERT(pos == strlen(test_str), "SJIS_StrChrSearch returns length for not found");

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
    printf("  dkutil_c Comprehensive Test Suite\n");
    printf("========================================\n");

    /* Container Tests */
    Test_Stack();
    Test_Queue();
    Test_Deque();
    Test_HashSet();
    Test_HashMap();
    Test_SingleList();
    Test_MemoryStream();
    Test_CircularMemoryStream();
    Test_Buffer();
    Test_MemoryPool();

    /* Tree Tests */
    Test_BTree();
    Test_2Tree();
    Test_RedBlackTree();

    /* Hash/Digest Tests */
    Test_CRC();
    Test_MD5();
    Test_MD4();
    Test_MD2();
    Test_SHA1();
    Test_SHA256();
    Test_SHA384();
    Test_SHA512();
    Test_HMAC();

    /* Cipher Tests */
    Test_Arcfour();
    Test_Blowfish();
    Test_Rijndael();
    Test_SNOW20();
    Test_HC256();

    /* Compression Tests */
    Test_LZSS();
    Test_LZW();
    Test_RLE_PackBits();

    Test_Camellia();

    /* Cipher Tests (additional) */
    Test_Vernam();

    /* Regex Tests */
    Test_Regex();

    /* Utility Tests */
    Test_Memory();
    Test_Sort();
    Test_Bit();
    Test_Math();
    Test_String();

    /* Print summary */
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
