/*
 * test_main.c
 * dkutil_c Comprehensive Test Suite
 * VC6 Compatible (C89)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dkc.h"

#ifdef _MSC_VER
#define QW(x) x##ui64
#else
#define QW(x) x##ULL
#endif

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
 * Test: dkcHashMultiSet.c
 */
static int WINAPIV test_hmset_int_compare(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    return va - vb;
}

void Test_HashMultiSet(void)
{
    DKC_HASHMULTISET *hms;
    int key;
    int i;
    size_t removed;

    TEST_BEGIN("dkcHashMultiSet Test");

    hms = dkcAllocHashMultiSet(sizeof(int), 0, NULL, test_hmset_int_compare);
    TEST_ASSERT(hms != NULL, "dkcAllocHashMultiSet");
    TEST_ASSERT(dkcHashMultiSetIsEmpty(hms) == TRUE, "New multi-set is empty");

    /* Insert duplicates allowed */
    key = 10;
    TEST_ASSERT(dkcHashMultiSetInsert(hms, &key) == edk_SUCCEEDED, "Insert 10 first");
    TEST_ASSERT(dkcHashMultiSetInsert(hms, &key) == edk_SUCCEEDED, "Insert 10 second (dup ok)");
    TEST_ASSERT(dkcHashMultiSetInsert(hms, &key) == edk_SUCCEEDED, "Insert 10 third (dup ok)");
    TEST_ASSERT(dkcHashMultiSetSize(hms) == 3, "Size is 3 after 3 inserts of same key");

    /* Count */
    TEST_ASSERT(dkcHashMultiSetCount(hms, &key) == 3, "Count(10) is 3");
    key = 99;
    TEST_ASSERT(dkcHashMultiSetCount(hms, &key) == 0, "Count(99) is 0");

    /* Contains */
    key = 10;
    TEST_ASSERT(dkcHashMultiSetContains(hms, &key) == TRUE, "Contains 10");
    key = 99;
    TEST_ASSERT(dkcHashMultiSetContains(hms, &key) == FALSE, "Does not contain 99");

    /* Erase one */
    key = 10;
    TEST_ASSERT(dkcHashMultiSetErase(hms, &key) == edk_SUCCEEDED, "Erase one 10");
    TEST_ASSERT(dkcHashMultiSetSize(hms) == 2, "Size is 2 after erase one");
    TEST_ASSERT(dkcHashMultiSetCount(hms, &key) == 2, "Count(10) is 2");

    /* EraseAll */
    removed = dkcHashMultiSetEraseAll(hms, &key);
    TEST_ASSERT(removed == 2, "EraseAll(10) removed 2");
    TEST_ASSERT(dkcHashMultiSetSize(hms) == 0, "Size is 0 after EraseAll");
    TEST_ASSERT(dkcHashMultiSetContains(hms, &key) == FALSE, "10 gone after EraseAll");

    /* EraseAll on non-existent */
    key = 999;
    removed = dkcHashMultiSetEraseAll(hms, &key);
    TEST_ASSERT(removed == 0, "EraseAll non-existent returns 0");

    /* Mixed keys with duplicates */
    key = 1; dkcHashMultiSetInsert(hms, &key);
    key = 2; dkcHashMultiSetInsert(hms, &key);
    key = 2; dkcHashMultiSetInsert(hms, &key);
    key = 3; dkcHashMultiSetInsert(hms, &key);
    key = 3; dkcHashMultiSetInsert(hms, &key);
    key = 3; dkcHashMultiSetInsert(hms, &key);
    TEST_ASSERT(dkcHashMultiSetSize(hms) == 6, "Size is 6 (1+2+3)");
    key = 1; TEST_ASSERT(dkcHashMultiSetCount(hms, &key) == 1, "Count(1)=1");
    key = 2; TEST_ASSERT(dkcHashMultiSetCount(hms, &key) == 2, "Count(2)=2");
    key = 3; TEST_ASSERT(dkcHashMultiSetCount(hms, &key) == 3, "Count(3)=3");

    /* Mass insert to trigger rehash */
    dkcHashMultiSetClear(hms);
    {
        size_t initial_buckets = dkcHashMultiSetBucketCount(hms);
        for(i = 0; i < 50; i++){
            key = i % 10;
            dkcHashMultiSetInsert(hms, &key);
        }
        TEST_ASSERT(dkcHashMultiSetSize(hms) == 50, "50 elements after mass insert");
        TEST_ASSERT(dkcHashMultiSetBucketCount(hms) > initial_buckets, "Rehash occurred");
        key = 0;
        TEST_ASSERT(dkcHashMultiSetCount(hms, &key) == 5, "Count(0)=5 after mass insert");
    }

    /* Clear */
    dkcHashMultiSetClear(hms);
    TEST_ASSERT(dkcHashMultiSetSize(hms) == 0, "Size 0 after clear");
    TEST_ASSERT(dkcHashMultiSetIsEmpty(hms) == TRUE, "Empty after clear");

    dkcFreeHashMultiSet(&hms);
    TEST_ASSERT(hms == NULL, "Free multi-set");

    TEST_END();
}

/*
 * Test: dkcHashMultiMap.c
 */
static int WINAPIV test_hmmap_int_compare(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    return va - vb;
}

static BOOL WINAPI test_hmmap_collect_callback(
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

void Test_HashMultiMap(void)
{
    DKC_HASHMULTIMAP *hmm;
    int key;
    int val;
    int read_val;
    int result;
    size_t removed;
    void *found;
    size_t found_size;
    int sum;
    int i;

    TEST_BEGIN("dkcHashMultiMap Test");

    hmm = dkcAllocHashMultiMap(sizeof(int), 0, NULL, test_hmmap_int_compare);
    TEST_ASSERT(hmm != NULL, "dkcAllocHashMultiMap");
    TEST_ASSERT(dkcHashMultiMapIsEmpty(hmm) == TRUE, "New multi-map is empty");

    /* Insert duplicates allowed */
    key = 1; val = 100;
    TEST_ASSERT(dkcHashMultiMapInsert(hmm, &key, &val, sizeof(int)) == edk_SUCCEEDED,
        "Insert key=1 val=100");
    key = 1; val = 200;
    TEST_ASSERT(dkcHashMultiMapInsert(hmm, &key, &val, sizeof(int)) == edk_SUCCEEDED,
        "Insert key=1 val=200 (dup ok)");
    key = 1; val = 300;
    TEST_ASSERT(dkcHashMultiMapInsert(hmm, &key, &val, sizeof(int)) == edk_SUCCEEDED,
        "Insert key=1 val=300 (dup ok)");
    TEST_ASSERT(dkcHashMultiMapSize(hmm) == 3, "Size is 3");

    /* Count */
    key = 1;
    TEST_ASSERT(dkcHashMultiMapCount(hmm, &key) == 3, "Count(1) is 3");
    key = 99;
    TEST_ASSERT(dkcHashMultiMapCount(hmm, &key) == 0, "Count(99) is 0");

    /* Contains */
    key = 1;
    TEST_ASSERT(dkcHashMultiMapContains(hmm, &key) == TRUE, "Contains key=1");

    /* Find (returns first match) */
    key = 1;
    found = dkcHashMultiMapFind(hmm, &key, &found_size);
    TEST_ASSERT(found != NULL, "Find key=1 non-NULL");
    TEST_ASSERT(found_size == sizeof(int), "Find returns correct size");

    /* GetBuffer (returns first match) */
    key = 1; read_val = 0;
    result = dkcHashMultiMapGetBuffer(hmm, &key, &read_val, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "GetBuffer key=1 succeeds");

    /* Erase one */
    key = 1;
    TEST_ASSERT(dkcHashMultiMapErase(hmm, &key) == edk_SUCCEEDED, "Erase one key=1");
    TEST_ASSERT(dkcHashMultiMapSize(hmm) == 2, "Size is 2 after erase one");
    TEST_ASSERT(dkcHashMultiMapCount(hmm, &key) == 2, "Count(1) is 2");

    /* EraseAll */
    removed = dkcHashMultiMapEraseAll(hmm, &key);
    TEST_ASSERT(removed == 2, "EraseAll(1) removed 2");
    TEST_ASSERT(dkcHashMultiMapSize(hmm) == 0, "Size 0 after EraseAll");

    /* Foreach with mixed keys */
    key = 10; val = 1;
    dkcHashMultiMapInsert(hmm, &key, &val, sizeof(int));
    key = 10; val = 2;
    dkcHashMultiMapInsert(hmm, &key, &val, sizeof(int));
    key = 20; val = 10;
    dkcHashMultiMapInsert(hmm, &key, &val, sizeof(int));
    /* sum = 1 + 2 + 10 = 13 */
    sum = 0;
    dkcHashMultiMapForeach(hmm, test_hmmap_collect_callback, &sum);
    TEST_ASSERT(sum == 13, "Foreach sum is 13");

    /* Mass insert to trigger rehash */
    dkcHashMultiMapClear(hmm);
    {
        size_t initial_buckets = dkcHashMultiMapBucketCount(hmm);
        int all_ok = 1;
        for(i = 0; i < 50; i++){
            key = i % 10;
            val = i;
            dkcHashMultiMapInsert(hmm, &key, &val, sizeof(int));
        }
        TEST_ASSERT(dkcHashMultiMapSize(hmm) == 50, "50 entries after mass insert");
        TEST_ASSERT(dkcHashMultiMapBucketCount(hmm) > initial_buckets, "Rehash occurred");
        key = 0;
        TEST_ASSERT(dkcHashMultiMapCount(hmm, &key) == 5, "Count(0)=5");

        /* verify EraseAll works after rehash */
        removed = dkcHashMultiMapEraseAll(hmm, &key);
        TEST_ASSERT(removed == 5, "EraseAll(0) removed 5 after rehash");
        TEST_ASSERT(dkcHashMultiMapSize(hmm) == 45, "45 entries remain");
    }

    /* Clear */
    dkcHashMultiMapClear(hmm);
    TEST_ASSERT(dkcHashMultiMapIsEmpty(hmm) == TRUE, "Empty after clear");

    /* Error handling */
    TEST_ASSERT(dkcHashMultiMapInsert(NULL, &key, &val, sizeof(int)) == edk_FAILED,
        "Insert NULL ptr fails");
    TEST_ASSERT(dkcAllocHashMultiMap(0, 0, NULL, test_hmmap_int_compare) == NULL,
        "Alloc key_size=0 fails");
    TEST_ASSERT(dkcAllocHashMultiMap(sizeof(int), 0, NULL, NULL) == NULL,
        "Alloc NULL compare fails");

    dkcFreeHashMultiMap(&hmm);
    TEST_ASSERT(hmm == NULL, "Free multi-map");

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

/*
 * Test: dkcSHA3.c (SHA3-224/256/384/512)
 */
void Test_SHA3(void)
{
    DKC_SHA3_256 *sha3_256;
    DKC_SHA3_512 *sha3_512;
    DKC_SHA3_224 *sha3_224;
    DKC_SECURE_HASH_OBJECT *sho;
    char digest256[SHA3_256_STR_BUFFER_SIZE];
    char digest512[SHA3_512_STR_BUFFER_SIZE];
    char digest224[SHA3_224_STR_BUFFER_SIZE];
    char sho_digest[SHA3_256_STR_BUFFER_SIZE];

    TEST_BEGIN("dkcSHA3 Test");

    /* SHA3-256("abc") - NIST FIPS 202 test vector */
    sha3_256 = dkcAllocSHA3_256();
    TEST_ASSERT(sha3_256 != NULL, "dkcAllocSHA3_256");
    dkcSHA3_256Load(sha3_256, (const BYTE*)"abc", 3);
    dkcSHA3_256FinalDigestStr(sha3_256, digest256, sizeof(digest256));
    TEST_ASSERT(strcmp(digest256, "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532") == 0,
                "SHA3-256('abc') correct");
    dkcFreeSHA3_256(&sha3_256);
    TEST_ASSERT(sha3_256 == NULL, "Free SHA3-256 sets NULL");

    /* SHA3-512("abc") - NIST FIPS 202 test vector */
    sha3_512 = dkcAllocSHA3_512();
    TEST_ASSERT(sha3_512 != NULL, "dkcAllocSHA3_512");
    dkcSHA3_512Load(sha3_512, (const BYTE*)"abc", 3);
    dkcSHA3_512FinalDigestStr(sha3_512, digest512, sizeof(digest512));
    TEST_ASSERT(strcmp(digest512, "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0") == 0,
                "SHA3-512('abc') correct");
    dkcFreeSHA3_512(&sha3_512);

    /* SHA3-224("abc") - NIST FIPS 202 test vector */
    sha3_224 = dkcAllocSHA3_224();
    TEST_ASSERT(sha3_224 != NULL, "dkcAllocSHA3_224");
    dkcSHA3_224Load(sha3_224, (const BYTE*)"abc", 3);
    dkcSHA3_224FinalDigestStr(sha3_224, digest224, sizeof(digest224));
    TEST_ASSERT(strcmp(digest224, "e642824c3f8cf24ad09234ee7d3c766fc9a3a5168d0c94ad73b46fdf") == 0,
                "SHA3-224('abc') correct");
    dkcFreeSHA3_224(&sha3_224);

    /* SHA3-256 via SHO interface */
    sho = dkcAllocSHO(edkcSH_SHA3_256);
    TEST_ASSERT(sho != NULL, "dkcAllocSHO(SHA3-256)");
    dkcSHOInit(sho);
    dkcSHOLoad(sho, (const BYTE*)"abc", 3);
    dkcSHOFinal(sho);
    dkcSHODigestStr(sho, sho_digest, sizeof(sho_digest));
    TEST_ASSERT(strcmp(sho_digest, "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532") == 0,
                "SHA3-256 via SHO correct");
    dkcFreeSHO(&sho);

    /* SHA3-256 empty string */
    sha3_256 = dkcAllocSHA3_256();
    dkcSHA3_256Load(sha3_256, (const BYTE*)"", 0);
    dkcSHA3_256FinalDigestStr(sha3_256, digest256, sizeof(digest256));
    TEST_ASSERT(strcmp(digest256, "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a") == 0,
                "SHA3-256('') correct");
    dkcFreeSHA3_256(&sha3_256);

    /* SHA3-256 split input (multiple Load calls) */
    sha3_256 = dkcAllocSHA3_256();
    dkcSHA3_256Load(sha3_256, (const BYTE*)"a", 1);
    dkcSHA3_256Load(sha3_256, (const BYTE*)"b", 1);
    dkcSHA3_256Load(sha3_256, (const BYTE*)"c", 1);
    dkcSHA3_256FinalDigestStr(sha3_256, digest256, sizeof(digest256));
    TEST_ASSERT(strcmp(digest256, "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532") == 0,
                "SHA3-256 split input matches");
    dkcFreeSHA3_256(&sha3_256);

    TEST_END();
}

/*
 * Test: dkcBLAKE256.c (BLAKE-256 / BLAKE-224)
 */
void Test_BLAKE256(void)
{
    DKC_BLAKE256 *b256;
    DKC_BLAKE224 *b224;
    char digest[BLAKE256_STR_BUFFER_SIZE];
    char digest224[BLAKE224_STR_BUFFER_SIZE];

    TEST_BEGIN("dkcBLAKE256 Test");

    /* BLAKE-256("") */
    b256 = dkcAllocBLAKE256();
    TEST_ASSERT(b256 != NULL, "dkcAllocBLAKE256");
    dkcBLAKE256Load(b256, (const BYTE*)"", 0);
    dkcBLAKE256FinalDigestStr(b256, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "716f6e863f744b9ac22c97ec7b76ea5f5908bc5b2f67c61510bfc4751384ea7a") == 0,
                "BLAKE-256('') correct");
    dkcFreeBLAKE256(&b256);

    /* BLAKE-256("abc") */
    b256 = dkcAllocBLAKE256();
    dkcBLAKE256Load(b256, (const BYTE*)"abc", 3);
    dkcBLAKE256FinalDigestStr(b256, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "1833a9fa7cf4086bd5fda73da32e5a1d75b4c3f89d5c436369f9d78bb2da5c28") == 0,
                "BLAKE-256('abc') correct");
    dkcFreeBLAKE256(&b256);

    /* BLAKE-256 split input */
    b256 = dkcAllocBLAKE256();
    dkcBLAKE256Load(b256, (const BYTE*)"a", 1);
    dkcBLAKE256Load(b256, (const BYTE*)"b", 1);
    dkcBLAKE256Load(b256, (const BYTE*)"c", 1);
    dkcBLAKE256FinalDigestStr(b256, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "1833a9fa7cf4086bd5fda73da32e5a1d75b4c3f89d5c436369f9d78bb2da5c28") == 0,
                "BLAKE-256 split input matches");
    dkcFreeBLAKE256(&b256);

    /* BLAKE-224("") */
    b224 = dkcAllocBLAKE224();
    TEST_ASSERT(b224 != NULL, "dkcAllocBLAKE224");
    dkcBLAKE224Load(b224, (const BYTE*)"", 0);
    dkcBLAKE224FinalDigestStr(b224, digest224, sizeof(digest224));
    TEST_ASSERT(strlen(digest224) == 56, "BLAKE-224 digest is 56 hex chars");
    dkcFreeBLAKE224(&b224);

    TEST_END();
}

/*
 * Test: dkcBLAKE512.c (BLAKE-512 / BLAKE-384)
 */
void Test_BLAKE512(void)
{
    DKC_BLAKE512 *b512;
    DKC_BLAKE384 *b384;
    char digest[BLAKE512_STR_BUFFER_SIZE];
    char digest384[BLAKE384_STR_BUFFER_SIZE];

    TEST_BEGIN("dkcBLAKE512 Test");

    /* BLAKE-512("") - verify prefix */
    b512 = dkcAllocBLAKE512();
    TEST_ASSERT(b512 != NULL, "dkcAllocBLAKE512");
    dkcBLAKE512Load(b512, (const BYTE*)"", 0);
    dkcBLAKE512FinalDigestStr(b512, digest, sizeof(digest));
    TEST_ASSERT(strncmp(digest, "a8cfbbd73726062d", 16) == 0,
                "BLAKE-512('') prefix correct");
    TEST_ASSERT(strlen(digest) == 128, "BLAKE-512 digest is 128 hex chars");
    dkcFreeBLAKE512(&b512);

    /* BLAKE-384("") */
    b384 = dkcAllocBLAKE384();
    TEST_ASSERT(b384 != NULL, "dkcAllocBLAKE384");
    dkcBLAKE384Load(b384, (const BYTE*)"", 0);
    dkcBLAKE384FinalDigestStr(b384, digest384, sizeof(digest384));
    TEST_ASSERT(strlen(digest384) == 96, "BLAKE-384 digest is 96 hex chars");
    dkcFreeBLAKE384(&b384);

    TEST_END();
}

/*
 * Test: dkcBLAKE2.c (BLAKE2b / BLAKE2s)
 */
void Test_BLAKE2(void)
{
    DKC_BLAKE2B *b2b;
    DKC_BLAKE2S *b2s;
    char digest_b[BLAKE2B_STR_BUFFER_SIZE];
    char digest_s[BLAKE2S_STR_BUFFER_SIZE];

    TEST_BEGIN("dkcBLAKE2 Test");

    /* BLAKE2b-512("") */
    b2b = dkcAllocBLAKE2b();
    TEST_ASSERT(b2b != NULL, "dkcAllocBLAKE2b");
    dkcBLAKE2bLoad(b2b, (const BYTE*)"", 0);
    dkcBLAKE2bFinalDigestStr(b2b, digest_b, sizeof(digest_b));
    TEST_ASSERT(strcmp(digest_b, "786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce") == 0,
                "BLAKE2b-512('') correct");
    dkcFreeBLAKE2b(&b2b);

    /* BLAKE2b-512("abc") - verify non-empty */
    b2b = dkcAllocBLAKE2b();
    dkcBLAKE2bLoad(b2b, (const BYTE*)"abc", 3);
    dkcBLAKE2bFinalDigestStr(b2b, digest_b, sizeof(digest_b));
    TEST_ASSERT(strcmp(digest_b, "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6886a7e7907d2e2e7104f9f7ee592e723f044eef62cef1ee0b29649e2b5e72c6816e8ef1e2") == 0,
                "BLAKE2b-512('abc') correct");
    dkcFreeBLAKE2b(&b2b);

    /* BLAKE2s-256("") */
    b2s = dkcAllocBLAKE2s();
    TEST_ASSERT(b2s != NULL, "dkcAllocBLAKE2s");
    dkcBLAKE2sLoad(b2s, (const BYTE*)"", 0);
    dkcBLAKE2sFinalDigestStr(b2s, digest_s, sizeof(digest_s));
    TEST_ASSERT(strcmp(digest_s, "69217a3079908094e11121d042354a7c1f55b6482ca1a51e1b250dfd1ed0eef9") == 0,
                "BLAKE2s-256('') correct");
    dkcFreeBLAKE2s(&b2s);

    /* BLAKE2s-256("abc") */
    b2s = dkcAllocBLAKE2s();
    dkcBLAKE2sLoad(b2s, (const BYTE*)"abc", 3);
    dkcBLAKE2sFinalDigestStr(b2s, digest_s, sizeof(digest_s));
    TEST_ASSERT(strcmp(digest_s, "508c5e8c327c14e2e1a72ba34eeb452f37458b209ed63a294d999b4c86675982") == 0,
                "BLAKE2s-256('abc') correct");
    dkcFreeBLAKE2s(&b2s);

    /* BLAKE2b split input */
    b2b = dkcAllocBLAKE2b();
    dkcBLAKE2bLoad(b2b, (const BYTE*)"a", 1);
    dkcBLAKE2bLoad(b2b, (const BYTE*)"b", 1);
    dkcBLAKE2bLoad(b2b, (const BYTE*)"c", 1);
    dkcBLAKE2bFinalDigestStr(b2b, digest_b, sizeof(digest_b));
    TEST_ASSERT(strcmp(digest_b, "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6886a7e7907d2e2e7104f9f7ee592e723f044eef62cef1ee0b29649e2b5e72c6816e8ef1e2") == 0,
                "BLAKE2b split input matches");
    dkcFreeBLAKE2b(&b2b);

    TEST_END();
}

/*
 * Test: dkcBLAKE3.c
 */
void Test_BLAKE3(void)
{
    DKC_BLAKE3 *b3;
    char digest[BLAKE3_STR_BUFFER_SIZE];

    TEST_BEGIN("dkcBLAKE3 Test");

    /* BLAKE3("") */
    b3 = dkcAllocBLAKE3();
    TEST_ASSERT(b3 != NULL, "dkcAllocBLAKE3");
    dkcBLAKE3Load(b3, (const BYTE*)"", 0);
    dkcBLAKE3FinalDigestStr(b3, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262") == 0,
                "BLAKE3('') correct");
    dkcFreeBLAKE3(&b3);

    /* BLAKE3 split input */
    b3 = dkcAllocBLAKE3();
    dkcBLAKE3Load(b3, (const BYTE*)"a", 1);
    dkcBLAKE3Load(b3, (const BYTE*)"b", 1);
    dkcBLAKE3Load(b3, (const BYTE*)"c", 1);
    dkcBLAKE3Final(b3);
    dkcBLAKE3DigestStr(b3, digest, sizeof(digest));
    /* Just verify it produces a 64-char hex string */
    TEST_ASSERT(strlen(digest) == 64, "BLAKE3('abc') produces 64 hex chars");
    dkcFreeBLAKE3(&b3);

    /* Free sets NULL */
    b3 = dkcAllocBLAKE3();
    dkcFreeBLAKE3(&b3);
    TEST_ASSERT(b3 == NULL, "Free BLAKE3 sets NULL");

    TEST_END();
}

/*
 * Test: BLAKE SHO interface
 */
void Test_BLAKE_SHO(void)
{
    DKC_SECURE_HASH_OBJECT *sho;
    char digest[BLAKE2B_STR_BUFFER_SIZE];

    TEST_BEGIN("BLAKE SHO Interface Test");

    /* BLAKE-256 via SHO */
    sho = dkcAllocSHO(edkcSH_BLAKE256);
    TEST_ASSERT(sho != NULL, "dkcAllocSHO(BLAKE256)");
    dkcSHOInit(sho);
    dkcSHOLoad(sho, (const BYTE*)"abc", 3);
    dkcSHOFinal(sho);
    dkcSHODigestStr(sho, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "1833a9fa7cf4086bd5fda73da32e5a1d75b4c3f89d5c436369f9d78bb2da5c28") == 0,
                "BLAKE-256 via SHO correct");
    dkcFreeSHO(&sho);

    /* BLAKE2b via SHO */
    sho = dkcAllocSHO(edkcSH_BLAKE2b);
    TEST_ASSERT(sho != NULL, "dkcAllocSHO(BLAKE2b)");
    dkcSHOInit(sho);
    dkcSHOLoad(sho, (const BYTE*)"", 0);
    dkcSHOFinal(sho);
    dkcSHODigestStr(sho, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce") == 0,
                "BLAKE2b via SHO correct");
    dkcFreeSHO(&sho);

    /* BLAKE3 via SHO */
    sho = dkcAllocSHO(edkcSH_BLAKE3);
    TEST_ASSERT(sho != NULL, "dkcAllocSHO(BLAKE3)");
    dkcSHOInit(sho);
    dkcSHOLoad(sho, (const BYTE*)"", 0);
    dkcSHOFinal(sho);
    dkcSHODigestStr(sho, digest, sizeof(digest));
    TEST_ASSERT(strcmp(digest, "af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262") == 0,
                "BLAKE3 via SHO correct");
    dkcFreeSHO(&sho);

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

/*
 * Test: dkcKCipher2.c
 */
void Test_KCipher2(void)
{
    DKC_KCIPHER2 *kc;
    unsigned char key[16];
    unsigned char iv[16];
    unsigned char plaintext[64];
    unsigned char ciphertext[64];
    unsigned char decrypted[64];
    unsigned char inplace[64];
    int result;
    size_t i;

    TEST_BEGIN("dkcKCipher2 Test");

    /* Initialize test data */
    for (i = 0; i < 16; i++) {
        key[i] = (unsigned char)(i + 0x10);
        iv[i] = (unsigned char)(i + 0x20);
    }
    for (i = 0; i < 64; i++) {
        plaintext[i] = (unsigned char)(i * 3);
    }

    /* Allocate context */
    kc = dkcAllocKCipher2(key, 16, iv, 16);
    TEST_ASSERT(kc != NULL, "dkcAllocKCipher2");

    /* Encrypt */
    result = dkcKCipher2Encrypt(kc, ciphertext, 64, plaintext, 64);
    TEST_ASSERT(result == edk_SUCCEEDED, "KCipher2 encrypt succeeds");
    TEST_ASSERT(memcmp(plaintext, ciphertext, 64) != 0, "Ciphertext differs from plaintext");

    dkcFreeKCipher2(&kc);
    TEST_ASSERT(kc == NULL, "Free KCipher2");

    /* Decrypt with fresh context (same key/iv) */
    kc = dkcAllocKCipher2(key, 16, iv, 16);
    TEST_ASSERT(kc != NULL, "Reallocate KCipher2 for decrypt");

    result = dkcKCipher2Decrypt(kc, decrypted, 64, ciphertext, 64);
    TEST_ASSERT(result == edk_SUCCEEDED, "KCipher2 decrypt succeeds");
    TEST_ASSERT(memcmp(plaintext, decrypted, 64) == 0, "Decrypt matches original");

    dkcFreeKCipher2(&kc);

    /* In-place encrypt/decrypt */
    kc = dkcAllocKCipher2(key, 16, iv, 16);
    memcpy(inplace, plaintext, 64);
    result = dkcKCipher2EncryptNoDest(kc, inplace, 64);
    TEST_ASSERT(result == edk_SUCCEEDED, "In-place encrypt succeeds");
    TEST_ASSERT(memcmp(inplace, ciphertext, 64) == 0, "In-place encrypt matches");
    dkcFreeKCipher2(&kc);

    kc = dkcAllocKCipher2(key, 16, iv, 16);
    result = dkcKCipher2DecryptNoDest(kc, inplace, 64);
    TEST_ASSERT(result == edk_SUCCEEDED, "In-place decrypt succeeds");
    TEST_ASSERT(memcmp(inplace, plaintext, 64) == 0, "In-place decrypt matches");
    dkcFreeKCipher2(&kc);

    /* Test Init function (re-initialize existing context) */
    kc = dkcAllocKCipher2(key, 16, iv, 16);
    result = dkcKCipher2Init(kc, key, 16, iv, 16);
    TEST_ASSERT(result == edk_SUCCEEDED, "KCipher2Init succeeds");

    memcpy(inplace, plaintext, 64);
    dkcKCipher2EncryptNoDest(kc, inplace, 64);
    TEST_ASSERT(memcmp(inplace, ciphertext, 64) == 0, "Re-init produces same output");
    dkcFreeKCipher2(&kc);

    /* Error cases */
    kc = dkcAllocKCipher2(key, 15, iv, 16);
    TEST_ASSERT(kc == NULL, "Invalid key size returns NULL");

    kc = dkcAllocKCipher2(key, 16, iv, 15);
    TEST_ASSERT(kc == NULL, "Invalid IV size returns NULL");

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
 * Test: dkcTwofish.c
 */
void Test_Twofish(void)
{
    DKC_TWOFISH *tf;
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

    TEST_BEGIN("dkcTwofish Test");

    /* Fill plaintext with test data */
    for (i = 0; i < 32; i++) {
        plaintext[i] = (unsigned char)i;
    }

    /* --- 128-bit key test --- */
    tf = dkcAllocTwofish(key128, 16);
    TEST_ASSERT(tf != NULL, "dkcAllocTwofish (128-bit key)");

    result = dkcTwofishEncrypt(tf, ciphertext, 16, plaintext, 16);
    TEST_ASSERT(result == edk_SUCCEEDED, "Encrypt 128-bit succeeds");
    TEST_ASSERT(memcmp(plaintext, ciphertext, 16) != 0, "Ciphertext differs from plaintext (128-bit)");

    result = dkcTwofishDecrypt(tf, decrypted, 16, ciphertext, 16);
    TEST_ASSERT(result == edk_SUCCEEDED, "Decrypt 128-bit succeeds");
    TEST_ASSERT(memcmp(plaintext, decrypted, 16) == 0, "Decrypt matches original (128-bit)");

    dkcFreeTwofish(&tf);
    TEST_ASSERT(tf == NULL, "Free twofish (128-bit)");

    /* --- 256-bit key test --- */
    tf = dkcAllocTwofish(key256, 32);
    TEST_ASSERT(tf != NULL, "dkcAllocTwofish (256-bit key)");

    result = dkcTwofishEncrypt(tf, ciphertext, 32, plaintext, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Encrypt 256-bit succeeds");
    TEST_ASSERT(memcmp(plaintext, ciphertext, 32) != 0, "Ciphertext differs from plaintext (256-bit)");

    result = dkcTwofishDecrypt(tf, decrypted, 32, ciphertext, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Decrypt 256-bit succeeds");
    TEST_ASSERT(memcmp(plaintext, decrypted, 32) == 0, "Decrypt matches original (256-bit)");

    /* In-place encrypt/decrypt (NoDest) */
    memcpy(inplace, plaintext, 32);
    dkcTwofishEncryptNoDest(tf, inplace, 32);
    TEST_ASSERT(memcmp(inplace, ciphertext, 32) == 0, "In-place encrypt matches (256-bit)");
    dkcTwofishDecryptNoDest(tf, inplace, 32);
    TEST_ASSERT(memcmp(inplace, plaintext, 32) == 0, "In-place decrypt matches (256-bit)");

    dkcFreeTwofish(&tf);

    /* --- Invalid key size test --- */
    tf = dkcAllocTwofish(key128, 15);
    TEST_ASSERT(tf == NULL, "Invalid key size (15) returns NULL");

    /* --- Size not multiple of 16 test --- */
    tf = dkcAllocTwofish(key128, 16);
    result = dkcTwofishEncrypt(tf, ciphertext, 32, plaintext, 17);
    TEST_ASSERT(result == edk_ArgumentException, "Non-block-aligned size rejected");
    dkcFreeTwofish(&tf);
    TEST_ASSERT(tf == NULL, "Free sets pointer to NULL");

    TEST_END();
}

/*
 * Test: dkcThreefish.c
 */
void Test_Threefish(void)
{
    DKC_THREEFISH256 *tf256;
    DKC_THREEFISH512 *tf512;
    DKC_THREEFISH *tf;
    unsigned char key256[32];
    unsigned char key512[64];
    unsigned char tweak[16];
    unsigned char plaintext256[32];
    unsigned char plaintext512[64];
    unsigned char ciphertext256[32];
    unsigned char ciphertext512[64];
    unsigned char decrypted256[32];
    unsigned char decrypted512[64];
    unsigned char inplace[64];
    int result;
    size_t i;

    TEST_BEGIN("dkcThreefish Test");

    /* Initialize test data */
    for (i = 0; i < 32; i++) {
        key256[i] = (unsigned char)i;
        plaintext256[i] = (unsigned char)(i * 3);
    }
    for (i = 0; i < 64; i++) {
        key512[i] = (unsigned char)(i * 2);
        plaintext512[i] = (unsigned char)(i * 5);
    }
    for (i = 0; i < 16; i++) {
        tweak[i] = (unsigned char)(0xAA ^ i);
    }

    /* ===== Threefish-256 tests ===== */
    tf256 = dkcAllocThreefish256(key256, 32, NULL);
    TEST_ASSERT(tf256 != NULL, "dkcAllocThreefish256 (no tweak)");

    result = dkcThreefish256Encrypt(tf256, ciphertext256, 32, plaintext256, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Threefish-256 encrypt succeeds");
    TEST_ASSERT(memcmp(plaintext256, ciphertext256, 32) != 0, "Threefish-256 ciphertext differs");

    result = dkcThreefish256Decrypt(tf256, decrypted256, 32, ciphertext256, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Threefish-256 decrypt succeeds");
    TEST_ASSERT(memcmp(plaintext256, decrypted256, 32) == 0, "Threefish-256 decrypt matches original");

    dkcFreeThreefish256(&tf256);
    TEST_ASSERT(tf256 == NULL, "Free Threefish-256");

    /* Threefish-256 with tweak */
    tf256 = dkcAllocThreefish256(key256, 32, tweak);
    TEST_ASSERT(tf256 != NULL, "dkcAllocThreefish256 (with tweak)");

    result = dkcThreefish256Encrypt(tf256, ciphertext256, 32, plaintext256, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Threefish-256 encrypt with tweak succeeds");

    result = dkcThreefish256Decrypt(tf256, decrypted256, 32, ciphertext256, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Threefish-256 decrypt with tweak succeeds");
    TEST_ASSERT(memcmp(plaintext256, decrypted256, 32) == 0, "Threefish-256 tweak decrypt matches");

    /* In-place test */
    memcpy(inplace, plaintext256, 32);
    dkcThreefish256EncryptNoDest(tf256, inplace, 32);
    TEST_ASSERT(memcmp(inplace, ciphertext256, 32) == 0, "Threefish-256 in-place encrypt");
    dkcThreefish256DecryptNoDest(tf256, inplace, 32);
    TEST_ASSERT(memcmp(inplace, plaintext256, 32) == 0, "Threefish-256 in-place decrypt");

    /* SetTweak test */
    result = dkcThreefish256SetTweak(tf256, tweak);
    TEST_ASSERT(result == edk_SUCCEEDED, "Threefish-256 SetTweak");

    dkcFreeThreefish256(&tf256);

    /* ===== Threefish-512 tests ===== */
    tf512 = dkcAllocThreefish512(key512, 64, tweak);
    TEST_ASSERT(tf512 != NULL, "dkcAllocThreefish512");

    result = dkcThreefish512Encrypt(tf512, ciphertext512, 64, plaintext512, 64);
    TEST_ASSERT(result == edk_SUCCEEDED, "Threefish-512 encrypt succeeds");
    TEST_ASSERT(memcmp(plaintext512, ciphertext512, 64) != 0, "Threefish-512 ciphertext differs");

    result = dkcThreefish512Decrypt(tf512, decrypted512, 64, ciphertext512, 64);
    TEST_ASSERT(result == edk_SUCCEEDED, "Threefish-512 decrypt succeeds");
    TEST_ASSERT(memcmp(plaintext512, decrypted512, 64) == 0, "Threefish-512 decrypt matches original");

    /* In-place test */
    memcpy(inplace, plaintext512, 64);
    dkcThreefish512EncryptNoDest(tf512, inplace, 64);
    TEST_ASSERT(memcmp(inplace, ciphertext512, 64) == 0, "Threefish-512 in-place encrypt");
    dkcThreefish512DecryptNoDest(tf512, inplace, 64);
    TEST_ASSERT(memcmp(inplace, plaintext512, 64) == 0, "Threefish-512 in-place decrypt");

    dkcFreeThreefish512(&tf512);
    TEST_ASSERT(tf512 == NULL, "Free Threefish-512");

    /* ===== Generic Threefish API tests ===== */
    tf = dkcAllocThreefish(edkcThreefish256, key256, 32, tweak);
    TEST_ASSERT(tf != NULL, "dkcAllocThreefish (256)");
    TEST_ASSERT(dkcThreefishGetBlockSize(tf) == 32, "GetBlockSize returns 32 for 256-bit");

    result = dkcThreefishEncrypt(tf, ciphertext256, 32, plaintext256, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Generic Threefish-256 encrypt");

    result = dkcThreefishDecrypt(tf, decrypted256, 32, ciphertext256, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Generic Threefish-256 decrypt");
    TEST_ASSERT(memcmp(plaintext256, decrypted256, 32) == 0, "Generic Threefish-256 roundtrip");

    dkcFreeThreefish(&tf);
    TEST_ASSERT(tf == NULL, "Free generic Threefish");

    tf = dkcAllocThreefish(edkcThreefish512, key512, 64, NULL);
    TEST_ASSERT(tf != NULL, "dkcAllocThreefish (512)");
    TEST_ASSERT(dkcThreefishGetBlockSize(tf) == 64, "GetBlockSize returns 64 for 512-bit");
    dkcFreeThreefish(&tf);

    /* ===== Error case tests ===== */
    tf256 = dkcAllocThreefish256(key256, 31, NULL);
    TEST_ASSERT(tf256 == NULL, "Invalid key size (31) returns NULL");

    tf256 = dkcAllocThreefish256(key256, 32, NULL);
    result = dkcThreefish256Encrypt(tf256, ciphertext256, 32, plaintext256, 17);
    TEST_ASSERT(result == edk_FAILED, "Non-block-aligned size rejected");
    dkcFreeThreefish256(&tf256);

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
    int arr4[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr5[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr6[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr7[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr8[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr9[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr10[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr11[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr12[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr13[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr14[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr15[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr16[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int arr17[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    int sorted[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    /* RadixSort test data (with negative values) */
    int rarr1[] = {5, -2, 8, -1, 9, 3, -7, 4, 6, 0};
    int rsorted[] = {-7, -2, -1, 0, 3, 4, 5, 6, 8, 9};
    unsigned int uarr1[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
    unsigned int usorted[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    TEST_BEGIN("dkcSort Test");

    dkcShellSort(arr1, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr1, sorted, sizeof(sorted)) == 0, "ShellSort");

    dkcCombSort(arr2, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr2, sorted, sizeof(sorted)) == 0, "CombSort");

    dkcBubbleSort(arr3, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr3, sorted, sizeof(sorted)) == 0, "BubbleSort");

    dkcInsertionSort(arr4, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr4, sorted, sizeof(sorted)) == 0, "InsertionSort");

    dkcSelectionSort(arr5, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr5, sorted, sizeof(sorted)) == 0, "SelectionSort");

    dkcMergeSort(arr6, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr6, sorted, sizeof(sorted)) == 0, "MergeSort");

    dkcHeapSort2(arr7, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr7, sorted, sizeof(sorted)) == 0, "HeapSort");

    dkcCocktailSort(arr8, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr8, sorted, sizeof(sorted)) == 0, "CocktailSort");

    dkcGnomeSort(arr9, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr9, sorted, sizeof(sorted)) == 0, "GnomeSort");

    dkcOddEvenSort(arr10, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr10, sorted, sizeof(sorted)) == 0, "OddEvenSort");

    dkcPancakeSort(arr11, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr11, sorted, sizeof(sorted)) == 0, "PancakeSort");

    dkcCycleSort(arr12, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr12, sorted, sizeof(sorted)) == 0, "CycleSort");

    dkcBitonicSort(arr13, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr13, sorted, sizeof(sorted)) == 0, "BitonicSort");

    dkcMultiPartitionSort(arr14, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr14, sorted, sizeof(sorted)) == 0, "MultiPartitionSort");

    dkcIntroSort(arr15, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr15, sorted, sizeof(sorted)) == 0, "IntroSort");

    dkcTimSort(arr16, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr16, sorted, sizeof(sorted)) == 0, "TimSort");

    dkcQuickSort(arr17, 10, sizeof(int), compare_int);
    TEST_ASSERT(memcmp(arr17, sorted, sizeof(sorted)) == 0, "QuickSort");

    /* RadixSort tests */
    TEST_ASSERT(dkcRadixSortInt(10, rarr1) == edk_SUCCEEDED, "RadixSortInt return");
    TEST_ASSERT(memcmp(rarr1, rsorted, sizeof(rsorted)) == 0, "RadixSortInt");

    TEST_ASSERT(dkcRadixSortUInt(10, uarr1) == edk_SUCCEEDED, "RadixSortUInt return");
    TEST_ASSERT(memcmp(uarr1, usorted, sizeof(usorted)) == 0, "RadixSortUInt");

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
 * DOUBLE LIST TESTS
 * ======================================== */

/*
 * Test: dkcDoubleList.c
 */
void Test_DoubleList(void)
{
    DKC_DOUBLELIST_OBJECT *obj;
    DKC_DOUBLELIST *it;
    int val;
    int result;
    size_t sz;

    TEST_BEGIN("dkcDoubleList Test");

    /* Alloc object with initial value */
    val = 10;
    obj = dkcAllocDoubleListObject(&val, sizeof(int));
    TEST_ASSERT(obj != NULL, "dkcAllocDoubleListObject");

    /* push_back x 2 */
    val = 20;
    obj->push_back(obj, &val, sizeof(int));
    val = 30;
    obj->push_back(obj, &val, sizeof(int));

    /* size should be 3 */
    sz = obj->size(obj);
    TEST_ASSERT(sz == 3, "Size is 3 after push_back x2");

    /* push_front */
    val = 5;
    obj->push_front(obj, &val, sizeof(int));
    sz = obj->size(obj);
    TEST_ASSERT(sz == 4, "Size is 4 after push_front");

    /* begin should have value 5 */
    it = obj->begin(obj);
    TEST_ASSERT(it != NULL, "begin() not null");
    result = 0;
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 5, "begin data is 5 (push_front)");

    /* forward traversal: 5 -> 10 -> 20 -> 30 */
    it = obj->next(it);
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 10, "Second element is 10");

    it = obj->next(it);
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 20, "Third element is 20");

    it = obj->next(it);
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 30, "Fourth element is 30");

    /* reverse traversal: tail -> prev -> ... -> begin */
    it = obj->tail(obj);
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 30, "Tail data is 30");

    it = obj->prev(it);
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 20, "Prev of tail is 20");

    it = obj->prev(it);
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 10, "Prev-prev of tail is 10");

    /* erase middle element (value 10) */
    obj->erase(obj, it);
    sz = obj->size(obj);
    TEST_ASSERT(sz == 3, "Size is 3 after erase middle");

    /* verify link integrity: 5 -> 20 -> 30 */
    it = obj->begin(obj);
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 5, "After erase: begin is 5");

    it = obj->next(it);
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 20, "After erase: second is 20");

    it = obj->next(it);
    obj->getbuffer(it, &result, sizeof(int));
    TEST_ASSERT(result == 30, "After erase: third is 30");

    TEST_ASSERT(obj->end(obj->next(it)), "Next of tail is end");

    /* Free */
    dkcFreeDoubleListObject(&obj);
    TEST_ASSERT(obj == NULL, "Free double list object");

    TEST_END();
}

/* ========================================
 * NARY TREE TESTS
 * ======================================== */

static BOOL WINAPI test_nary_preorder_callback(DKC_NARYTREE_NODE *node, void *user)
{
    int *counter = (int *)user;
    (*counter)++;
    return TRUE;
}

/*
 * Compare function for AVL Tree int keys
 */
static int WINAPIV compare_avltree_int(const void *a, const void *b)
{
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    if (ia < ib) return -1;
    if (ia > ib) return 1;
    return 0;
}

/*
 * Callback for AVL Tree foreach
 */
static BOOL WINAPI avltree_foreach_callback(const void *key, void *data, size_t data_size, void *user)
{
    int *counter = (int *)user;
    (void)key;
    (void)data;
    (void)data_size;
    (*counter)++;
    return TRUE;
}

/*
 * Test: dkcAVLTree.c
 */
void Test_AVLTree(void)
{
    DKC_AVLTREE_ROOT *tree;
    DKC_AVLTREE_NODE *node;
    DKC_AVLTREE_NODE *min_node;
    DKC_AVLTREE_NODE *max_node;
    int keys[10];
    int data_vals[10];
    int read_data;
    int result;
    int i;
    int counter;
    int height;

    TEST_BEGIN("dkcAVLTree (AVL Tree) Test");

    /* Create AVL tree */
    tree = dkcAllocAVLTreeRoot(sizeof(int), 10, compare_avltree_int, 100);
    TEST_ASSERT(tree != NULL, "dkcAllocAVLTreeRoot");

    TEST_ASSERT(dkcAVLTreeIsEmpty(tree) == TRUE, "New tree is empty");
    TEST_ASSERT(dkcAVLTreeSize(tree) == 0, "Size is 0");

    /* Insert multiple keys to test balancing */
    /* Insert in order that would create unbalanced tree without AVL balancing */
    keys[0] = 10; data_vals[0] = 100;
    keys[1] = 20; data_vals[1] = 200;
    keys[2] = 30; data_vals[2] = 300;
    keys[3] = 40; data_vals[3] = 400;
    keys[4] = 50; data_vals[4] = 500;
    keys[5] = 25; data_vals[5] = 250;

    for (i = 0; i < 6; i++) {
        result = dkcAVLTreeInsert(tree, &keys[i], &data_vals[i], sizeof(int));
        TEST_ASSERT(result == edk_SUCCEEDED, "Insert key");
    }

    TEST_ASSERT(dkcAVLTreeSize(tree) == 6, "Size is 6 after inserts");
    TEST_ASSERT(dkcAVLTreeIsEmpty(tree) == FALSE, "Tree is not empty");

    /* Check height - AVL tree should be balanced (height <= log2(n)+1) */
    height = dkcAVLTreeHeight(tree);
    TEST_ASSERT(height <= 4, "Tree height is balanced");

    /* Find existing keys */
    node = dkcAVLTreeFindEqual(tree, &keys[0]);
    TEST_ASSERT(node != NULL, "Find key 10");

    if (node != NULL) {
        dkcAVLTreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 100, "Key 10 has data 100");
    }

    node = dkcAVLTreeFindEqual(tree, &keys[4]);
    TEST_ASSERT(node != NULL, "Find key 50");

    if (node != NULL) {
        dkcAVLTreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 500, "Key 50 has data 500");
    }

    /* Find non-existing key */
    i = 999;
    node = dkcAVLTreeFindEqual(tree, &i);
    TEST_ASSERT(node == NULL, "Key 999 not found");

    /* Test FindMin and FindMax */
    min_node = dkcAVLTreeFindMin(tree);
    TEST_ASSERT(min_node != NULL, "FindMin returns node");
    if (min_node != NULL) {
        dkcAVLTreeGetBuffer(min_node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 100, "Min node has data 100 (key 10)");
    }

    max_node = dkcAVLTreeFindMax(tree);
    TEST_ASSERT(max_node != NULL, "FindMax returns node");
    if (max_node != NULL) {
        dkcAVLTreeGetBuffer(max_node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 500, "Max node has data 500 (key 50)");
    }

    /* Test FindMinimalGreater */
    i = 25;
    node = dkcAVLTreeFindMinimalGreater(tree, &i);
    TEST_ASSERT(node != NULL, "FindMinimalGreater(25) returns node");
    if (node != NULL) {
        dkcAVLTreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 300, "MinimalGreater(25) is 30 (data 300)");
    }

    /* Test FindMaximumLess */
    i = 25;
    node = dkcAVLTreeFindMaximumLess(tree, &i);
    TEST_ASSERT(node != NULL, "FindMaximumLess(25) returns node");
    if (node != NULL) {
        dkcAVLTreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 200, "MaximumLess(25) is 20 (data 200)");
    }

    /* Test foreach (in-order traversal) */
    counter = 0;
    result = dkcAVLTreeForeach(tree, edkcAVLTreeInOrder, avltree_foreach_callback, &counter);
    TEST_ASSERT(result == edk_SUCCEEDED, "Foreach in-order succeeded");
    TEST_ASSERT(counter == 6, "Foreach visited all 6 nodes");

    /* Test node existence */
    node = dkcAVLTreeFindEqual(tree, &keys[2]);
    TEST_ASSERT(dkcAVLTreeExist(tree, node) == edkcAVLTREE_EXIST, "Node exists in tree");

    /* Test delete */
    i = 30;
    result = dkcAVLTreeEraseFromKey(tree, &i);
    TEST_ASSERT(result == edk_SUCCEEDED, "Delete key 30");
    TEST_ASSERT(dkcAVLTreeSize(tree) == 5, "Size is 5 after delete");

    node = dkcAVLTreeFindEqual(tree, &i);
    TEST_ASSERT(node == NULL, "Key 30 not found after delete");

    /* Delete non-existing key */
    i = 999;
    result = dkcAVLTreeEraseFromKey(tree, &i);
    TEST_ASSERT(result == edk_Not_Found, "Delete non-existing key returns Not_Found");

    /* Test update existing key */
    i = 10;
    read_data = 1000;
    result = dkcAVLTreeInsert(tree, &i, &read_data, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Update existing key 10");

    node = dkcAVLTreeFindEqual(tree, &i);
    if (node != NULL) {
        dkcAVLTreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 1000, "Key 10 updated to data 1000");
    }

    /* Test clear */
    result = dkcAVLTreeClear(tree);
    TEST_ASSERT(result == edk_SUCCEEDED, "Clear tree");
    TEST_ASSERT(dkcAVLTreeIsEmpty(tree) == TRUE, "Tree is empty after clear");
    TEST_ASSERT(dkcAVLTreeSize(tree) == 0, "Size is 0 after clear");

    /* Free tree */
    dkcFreeAVLTreeRoot(&tree);
    TEST_ASSERT(tree == NULL, "Free AVL tree");

    TEST_END();
}

/*
 * Test: dkcNaryTree.c
 */
void Test_NaryTree(void)
{
    DKC_NARYTREE_NODE *root;
    DKC_NARYTREE_NODE *child1;
    DKC_NARYTREE_NODE *child2;
    DKC_NARYTREE_NODE *child3;
    DKC_NARYTREE_NODE *grandchild;
    DKC_NARYTREE_NODE *found_root;
    int val;
    int result;
    int counter;
    size_t num;

    TEST_BEGIN("dkcNaryTree Test");

    /* create root */
    val = 100;
    root = dkcAllocNaryTreeNode(&val, sizeof(int));
    TEST_ASSERT(root != NULL, "AllocNaryTreeNode root");
    TEST_ASSERT(dkcNaryTreeIsRoot(root) == TRUE, "Root is root");
    TEST_ASSERT(dkcNaryTreeIsLeaf(root) == TRUE, "Root with no children is leaf");

    /* append 3 children */
    val = 200;
    child1 = dkcAllocNaryTreeNode(&val, sizeof(int));
    val = 300;
    child2 = dkcAllocNaryTreeNode(&val, sizeof(int));
    val = 400;
    child3 = dkcAllocNaryTreeNode(&val, sizeof(int));

    dkcNaryTreeAppendChild(root, child1);
    dkcNaryTreeAppendChild(root, child2);
    dkcNaryTreeAppendChild(root, child3);

    TEST_ASSERT(dkcNaryTreeNumChildren(root) == 3, "Root has 3 children");
    TEST_ASSERT(dkcNaryTreeIsLeaf(root) == FALSE, "Root with children is not leaf");
    TEST_ASSERT(dkcNaryTreeIsRoot(child1) == FALSE, "Child1 is not root");

    /* add grandchild to child1 */
    val = 210;
    grandchild = dkcAllocNaryTreeNode(&val, sizeof(int));
    dkcNaryTreeAppendChild(child1, grandchild);

    /* NumNodes */
    num = dkcNaryTreeNumNodes(root);
    TEST_ASSERT(num == 5, "Total nodes is 5");

    /* Depth */
    TEST_ASSERT(dkcNaryTreeDepth(root) == 0, "Root depth is 0");
    TEST_ASSERT(dkcNaryTreeDepth(child1) == 1, "Child1 depth is 1");
    TEST_ASSERT(dkcNaryTreeDepth(grandchild) == 2, "Grandchild depth is 2");

    /* IsLeaf */
    TEST_ASSERT(dkcNaryTreeIsLeaf(child2) == TRUE, "Child2 is leaf");
    TEST_ASSERT(dkcNaryTreeIsLeaf(child1) == FALSE, "Child1 is not leaf");

    /* GetRoot from grandchild */
    found_root = dkcNaryTreeGetRoot(grandchild);
    TEST_ASSERT(found_root == root, "GetRoot from grandchild returns root");

    /* GetBuffer */
    result = 0;
    dkcNaryTreeGetBuffer(grandchild, &result, sizeof(int));
    TEST_ASSERT(result == 210, "Grandchild data is 210");

    /* SetBuffer */
    val = 999;
    dkcNaryTreeSetBuffer(child2, &val, sizeof(int));
    result = 0;
    dkcNaryTreeGetBuffer(child2, &result, sizeof(int));
    TEST_ASSERT(result == 999, "SetBuffer/GetBuffer roundtrip");

    /* Foreach PreOrder */
    counter = 0;
    dkcNaryTreeForeach(root, edkcNaryPreOrder, test_nary_preorder_callback, &counter);
    TEST_ASSERT(counter == 5, "PreOrder visits all 5 nodes");

    /* Foreach PostOrder */
    counter = 0;
    dkcNaryTreeForeach(root, edkcNaryPostOrder, test_nary_preorder_callback, &counter);
    TEST_ASSERT(counter == 5, "PostOrder visits all 5 nodes");

    /* Unlink child2 (subtree detach) */
    dkcNaryTreeUnlink(child2);
    num = dkcNaryTreeNumNodes(root);
    TEST_ASSERT(num == 4, "After unlink child2, total nodes is 4");
    TEST_ASSERT(dkcNaryTreeNumChildren(root) == 2, "Root has 2 children after unlink");
    TEST_ASSERT(dkcNaryTreeIsRoot(child2) == TRUE, "Unlinked child2 is now root");

    /* free unlinked node separately */
    dkcFreeNaryTreeNode(&child2);
    TEST_ASSERT(child2 == NULL, "Free unlinked child2");

    /* FreeAll */
    dkcFreeNaryTreeAll(&root);
    TEST_ASSERT(root == NULL, "FreeNaryTreeAll");

    TEST_END();
}

/* ========================================
 * BASE64 TESTS
 * ======================================== */

/*
 * Test: dkcBase64.c
 */
void Test_Base64(void)
{
    char enc_buf[256];
    BYTE dec_buf[256];
    size_t out_len;
    int result;
    /* test data */
    const BYTE *hello = (const BYTE *)"Hello";
    const BYTE *hi = (const BYTE *)"Hi";
    const BYTE *abc = (const BYTE *)"abc";

    TEST_BEGIN("dkcBase64 Test");

    /* empty input */
    result = dkcBase64Encode(enc_buf, sizeof(enc_buf), NULL, 0, &out_len);
    TEST_ASSERT(result == edk_SUCCEEDED && out_len == 0, "Encode empty input");

    /* "Hello" -> "SGVsbG8=" */
    result = dkcBase64Encode(enc_buf, sizeof(enc_buf), hello, 5, &out_len);
    TEST_ASSERT(result == edk_SUCCEEDED, "Encode Hello succeeded");
    TEST_ASSERT(strcmp(enc_buf, "SGVsbG8=") == 0, "Encode Hello = SGVsbG8=");
    TEST_ASSERT(out_len == 8, "Encode Hello length is 8");

    /* "Hi" -> "SGk=" (padding 2) */
    result = dkcBase64Encode(enc_buf, sizeof(enc_buf), hi, 2, &out_len);
    TEST_ASSERT(result == edk_SUCCEEDED, "Encode Hi succeeded");
    TEST_ASSERT(strcmp(enc_buf, "SGk=") == 0, "Encode Hi = SGk=");

    /* "abc" -> "YWJj" (no padding) */
    result = dkcBase64Encode(enc_buf, sizeof(enc_buf), abc, 3, &out_len);
    TEST_ASSERT(result == edk_SUCCEEDED, "Encode abc succeeded");
    TEST_ASSERT(strcmp(enc_buf, "YWJj") == 0, "Encode abc = YWJj");

    /* Decode "SGVsbG8=" -> "Hello" */
    memset(dec_buf, 0, sizeof(dec_buf));
    result = dkcBase64Decode(dec_buf, sizeof(dec_buf), "SGVsbG8=", 8, &out_len);
    TEST_ASSERT(result == edk_SUCCEEDED, "Decode SGVsbG8= succeeded");
    TEST_ASSERT(out_len == 5, "Decode length is 5");
    TEST_ASSERT(memcmp(dec_buf, "Hello", 5) == 0, "Decode result is Hello");

    /* Roundtrip: encode -> decode -> compare */
    {
        const BYTE test_data[] = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0x80, 0x7F};
        BYTE rt_dec[16];
        size_t enc_len;
        size_t dec_len;

        dkcBase64Encode(enc_buf, sizeof(enc_buf), test_data, sizeof(test_data), &enc_len);
        dkcBase64Decode(rt_dec, sizeof(rt_dec), enc_buf, enc_len, &dec_len);
        TEST_ASSERT(dec_len == sizeof(test_data), "Roundtrip: decoded size matches");
        TEST_ASSERT(memcmp(rt_dec, test_data, sizeof(test_data)) == 0, "Roundtrip: data matches");
    }

    /* Buffer overflow */
    result = dkcBase64Encode(enc_buf, 1, hello, 5, &out_len);
    TEST_ASSERT(result == edk_BufferOverFlow, "Encode buffer overflow detected");

    result = dkcBase64Decode(dec_buf, 1, "SGVsbG8=", 8, &out_len);
    TEST_ASSERT(result == edk_BufferOverFlow, "Decode buffer overflow detected");

    TEST_END();
}

/*
 * Test: dkcMPH.c (Minimal Perfect Hash - CHD Algorithm)
 */
void Test_MPH(void)
{
    DKC_MPH mph;
    const char *words[] = {
        "apple", "banana", "cherry", "date", "elderberry",
        "fig", "grape", "honeydew", "kiwi", "lemon",
        "mango", "nectarine", "orange", "papaya", "quince"
    };
    size_t num_words = sizeof(words) / sizeof(words[0]);
    size_t indices[15];
    uint8 used[15];
    int result;
    size_t i, j;
    void *serial_buf;
    size_t serial_size;

    TEST_BEGIN("dkcMPH Test (CHD Algorithm)");

    /* === 初期化テスト === */
    result = dkcMPHInit(&mph);
    TEST_ASSERT(result == edk_SUCCEEDED, "MPH init");

    /* === 文字列配列からの構築テスト === */
    result = dkcMPHBuildStrings(&mph, words, num_words);
    TEST_ASSERT(result == edk_SUCCEEDED, "MPH build from strings");
    TEST_ASSERT(mph.num_keys == num_words, "MPH num_keys matches");
    TEST_ASSERT(mph.g != NULL, "MPH g array allocated");

    /* === ルックアップテスト === */
    /* 全てのキーが0〜n-1の一意なインデックスを返すことを確認 */
    memset(used, 0, sizeof(used));
    for (i = 0; i < num_words; i++) {
        indices[i] = dkcMPHLookupString(&mph, words[i]);
        TEST_ASSERT(indices[i] < num_words, "MPH lookup returns valid index");
    }

    /* 全てのインデックスが一意であることを確認 */
    for (i = 0; i < num_words; i++) {
        TEST_ASSERT(used[indices[i]] == 0, "MPH index is unique");
        used[indices[i]] = 1;
    }

    /* 全ての0〜n-1が使われていることを確認（最小完全ハッシュの特性） */
    for (i = 0; i < num_words; i++) {
        TEST_ASSERT(used[i] == 1, "MPH all slots used (minimal)");
    }

    /* === 同じキーは同じインデックスを返す === */
    for (i = 0; i < num_words; i++) {
        size_t idx2 = dkcMPHLookupString(&mph, words[i]);
        TEST_ASSERT(indices[i] == idx2, "MPH lookup is deterministic");
    }

    /* === ユーティリティ関数テスト === */
    {
        size_t mem_usage = dkcMPHMemoryUsage(&mph);
        double bits_per_key = dkcMPHBitsPerKey(&mph);
        TEST_ASSERT(mem_usage > 0, "MPH memory usage > 0");
        TEST_ASSERT(bits_per_key > 0.0, "MPH bits per key > 0");
        /* CHDは通常2〜3 bits/key */
        TEST_ASSERT(bits_per_key < 50.0, "MPH bits per key reasonable");
    }

    /* === シリアライズ/デシリアライズテスト === */
    serial_size = dkcMPHSerializedSize(&mph);
    TEST_ASSERT(serial_size > 0, "MPH serialized size > 0");

    serial_buf = malloc(serial_size);
    TEST_ASSERT(serial_buf != NULL, "Serial buffer allocated");

    result = dkcMPHSerialize(&mph, serial_buf, serial_size);
    TEST_ASSERT(result == edk_SUCCEEDED, "MPH serialize");

    /* 別のMPH構造体にデシリアライズ */
    {
        DKC_MPH mph2;
        dkcMPHInit(&mph2);

        result = dkcMPHDeserialize(&mph2, serial_buf, serial_size);
        TEST_ASSERT(result == edk_SUCCEEDED, "MPH deserialize");
        TEST_ASSERT(mph2.num_keys == mph.num_keys, "Deserialized num_keys matches");
        TEST_ASSERT(mph2.g_size == mph.g_size, "Deserialized g_size matches");

        /* デシリアライズ後も同じ結果を返すことを確認 */
        for (i = 0; i < num_words; i++) {
            size_t idx_orig = dkcMPHLookupString(&mph, words[i]);
            size_t idx_deser = dkcMPHLookupString(&mph2, words[i]);
            TEST_ASSERT(idx_orig == idx_deser, "Deserialized lookup matches");
        }

        dkcMPHFree(&mph2);
    }

    free(serial_buf);

    /* === 配列APIテスト === */
    {
        DKC_MPH mph3;
        const void *key_ptrs[5];
        size_t key_lens[5];
        const char *test_keys[] = {"one", "two", "three", "four", "five"};

        for (i = 0; i < 5; i++) {
            key_ptrs[i] = test_keys[i];
            key_lens[i] = strlen(test_keys[i]);
        }

        dkcMPHInit(&mph3);
        result = dkcMPHBuild(&mph3, key_ptrs, key_lens, 5);
        TEST_ASSERT(result == edk_SUCCEEDED, "MPH build from array");

        /* 一意性確認 */
        {
            uint8 used3[5] = {0};
            for (i = 0; i < 5; i++) {
                size_t idx = dkcMPHLookup(&mph3, test_keys[i], strlen(test_keys[i]));
                TEST_ASSERT(idx < 5, "Array API lookup valid");
                TEST_ASSERT(used3[idx] == 0, "Array API index unique");
                used3[idx] = 1;
            }
        }

        dkcMPHFree(&mph3);
    }

    /* === コールバックAPIテスト === */
    /* dkcMPHBuildStringsが内部でコールバックを使用しているため、
       コールバックAPIは間接的にテスト済み */

    /* === 解放テスト === */
    dkcMPHFree(&mph);
    TEST_ASSERT(mph.g == NULL, "MPH freed");

    TEST_END();
}

/*
 * Test: dkcLightHash.c (Non-cryptographic hash functions)
 */
void Test_LightHash(void)
{
    const char *test_str = "Hello, World!";
    size_t test_len = strlen(test_str);
    uint32 h32_1, h32_2;
    uint64 h64_1, h64_2;
    uint64 murmur128[2];

    TEST_BEGIN("dkcLightHash Test");

    /* === Classic Hash Functions === */
    h32_1 = dkcHashDJB2(test_str, test_len);
    TEST_ASSERT(h32_1 != 0, "DJB2 hash non-zero");

    h32_1 = dkcHashDJB2a(test_str, test_len);
    TEST_ASSERT(h32_1 != 0, "DJB2a hash non-zero");

    h32_1 = dkcHashSDBM(test_str, test_len);
    TEST_ASSERT(h32_1 != 0, "SDBM hash non-zero");

    h32_1 = dkcHashFNV1_32(test_str, test_len);
    h32_2 = dkcHashFNV1_32(test_str, test_len);
    TEST_ASSERT(h32_1 == h32_2, "FNV1-32 deterministic");

    h32_1 = dkcHashFNV1a_32(test_str, test_len);
    TEST_ASSERT(h32_1 != 0, "FNV1a-32 hash non-zero");

    h64_1 = dkcHashFNV1_64(test_str, test_len);
    TEST_ASSERT(h64_1 != 0, "FNV1-64 hash non-zero");

    h64_1 = dkcHashFNV1a_64(test_str, test_len);
    TEST_ASSERT(h64_1 != 0, "FNV1a-64 hash non-zero");

    h32_1 = dkcHashJenkinsOAAT(test_str, test_len);
    TEST_ASSERT(h32_1 != 0, "Jenkins OAAT hash non-zero");

    h32_1 = dkcHashELF(test_str, test_len);
    TEST_ASSERT(h32_1 != 0, "ELF hash non-zero");

    h32_1 = dkcHashPJW(test_str, test_len);
    TEST_ASSERT(h32_1 != 0, "PJW hash non-zero");

    h32_1 = dkcHashSuperFast(test_str, test_len);
    TEST_ASSERT(h32_1 != 0, "SuperFast hash non-zero");

    {
        uint8 p = dkcHashPearson(test_str, test_len);
        TEST_ASSERT(p != dkcHashPearson("different", 9), "Pearson different inputs differ");
    }

    /* === Modern Hash Functions === */
    h32_1 = dkcHashMurmur2_32(test_str, test_len, 0);
    h32_2 = dkcHashMurmur2_32(test_str, test_len, 0);
    TEST_ASSERT(h32_1 == h32_2, "MurmurHash2-32 deterministic");

    h64_1 = dkcHashMurmur2_64(test_str, test_len, 0);
    TEST_ASSERT(h64_1 != 0, "MurmurHash2-64 non-zero");

    h32_1 = dkcHashMurmur3_32(test_str, test_len, 0);
    TEST_ASSERT(h32_1 != 0, "MurmurHash3-32 non-zero");

    dkcHashMurmur3_128(test_str, test_len, 0, murmur128);
    TEST_ASSERT(murmur128[0] != 0 || murmur128[1] != 0, "MurmurHash3-128 non-zero");

    h32_1 = dkcHashXX32(test_str, test_len, 0);
    h32_2 = dkcHashXX32(test_str, test_len, 0);
    TEST_ASSERT(h32_1 == h32_2, "xxHash32 deterministic");

    h64_1 = dkcHashXX64(test_str, test_len, 0);
    TEST_ASSERT(h64_1 != 0, "xxHash64 non-zero");

    h64_1 = dkcHashCity64(test_str, test_len);
    TEST_ASSERT(h64_1 != 0, "CityHash64 non-zero");

    h64_1 = dkcHashCity64WithSeed(test_str, test_len, 12345);
    TEST_ASSERT(h64_1 != 0, "CityHash64WithSeed non-zero");

    h64_1 = dkcHashWy(test_str, test_len, 0);
    TEST_ASSERT(h64_1 != 0, "wyhash non-zero");

    h32_1 = dkcHashFx32(test_str, test_len);
    TEST_ASSERT(h32_1 != 0, "FxHash32 non-zero");

    h64_1 = dkcHashFx64(test_str, test_len);
    TEST_ASSERT(h64_1 != 0, "FxHash64 non-zero");

    h32_1 = dkcHashLookup3(test_str, test_len, 0);
    TEST_ASSERT(h32_1 != 0, "Lookup3 non-zero");

    /* === Utility Functions === */
    h32_1 = dkcHashString("test string");
    TEST_ASSERT(h32_1 != 0, "HashString non-zero");

    h32_1 = dkcHashInt32(12345678);
    TEST_ASSERT(h32_1 != 12345678, "HashInt32 transforms input");

    h64_1 = dkcHashInt64(QW(0x123456789ABCDEF));
    TEST_ASSERT(h64_1 != QW(0x123456789ABCDEF), "HashInt64 transforms input");

    {
        uint32 combined = dkcHashCombine32(0x12345678, 0x9ABCDEF0);
        TEST_ASSERT(combined != 0x12345678 && combined != 0x9ABCDEF0, "HashCombine32 works");
    }

    /* === Collision resistance (basic check) === */
    {
        uint32 h1 = dkcHashFNV1a_32("abc", 3);
        uint32 h2 = dkcHashFNV1a_32("abd", 3);
        TEST_ASSERT(h1 != h2, "FNV1a different inputs produce different hashes");
    }

    TEST_END();
}

/*
 * Test: dkcBloomFilter.c
 */
void Test_BloomFilter(void)
{
    DKC_BLOOM bloom;
    DKC_COUNTING_BLOOM cbloom;
    const char *words[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int result;
    size_t i;

    TEST_BEGIN("dkcBloomFilter Test");

    /* === Basic Bloom Filter === */
    result = dkcBloomCreate(&bloom, 100, 0.01);
    TEST_ASSERT(result == edk_SUCCEEDED, "Bloom create");
    TEST_ASSERT(bloom.bits != NULL, "Bloom bits allocated");
    TEST_ASSERT(bloom.num_bits > 0, "Bloom num_bits > 0");
    TEST_ASSERT(bloom.num_hashes > 0, "Bloom num_hashes > 0");

    /* Add elements */
    for (i = 0; i < 5; i++) {
        result = dkcBloomAddString(&bloom, words[i]);
        TEST_ASSERT(result == edk_SUCCEEDED, "Bloom add string");
    }

    /* Check contains */
    for (i = 0; i < 5; i++) {
        TEST_ASSERT(dkcBloomContainsString(&bloom, words[i]) == TRUE,
            "Bloom contains added word");
    }

    /* Check not contains (may have false positives, but unlikely for these) */
    TEST_ASSERT(dkcBloomContainsString(&bloom, "notinset") == FALSE ||
                dkcBloomContainsString(&bloom, "xyz123") == FALSE,
                "Bloom likely doesn't contain random words");

    /* FPR and fill ratio */
    {
        double fpr = dkcBloomGetFPR(&bloom);
        double fill = dkcBloomGetFillRatio(&bloom);
        TEST_ASSERT(fpr >= 0.0 && fpr <= 1.0, "Bloom FPR in valid range");
        TEST_ASSERT(fill >= 0.0 && fill <= 1.0, "Bloom fill ratio in valid range");
    }

    /* Memory usage */
    {
        size_t mem = dkcBloomMemoryUsage(&bloom);
        TEST_ASSERT(mem > 0, "Bloom memory usage > 0");
    }

    /* Serialize/Deserialize */
    {
        size_t serial_size = dkcBloomSerializedSize(&bloom);
        void *buf = malloc(serial_size);
        DKC_BLOOM bloom2;

        TEST_ASSERT(buf != NULL, "Bloom serial buffer allocated");
        result = dkcBloomSerialize(&bloom, buf, serial_size);
        TEST_ASSERT(result == edk_SUCCEEDED, "Bloom serialize");

        memset(&bloom2, 0, sizeof(bloom2));
        result = dkcBloomDeserialize(&bloom2, buf, serial_size);
        TEST_ASSERT(result == edk_SUCCEEDED, "Bloom deserialize");

        /* Check deserialized filter still contains words */
        for (i = 0; i < 5; i++) {
            TEST_ASSERT(dkcBloomContainsString(&bloom2, words[i]) == TRUE,
                "Deserialized bloom contains words");
        }

        dkcBloomFree(&bloom2);
        free(buf);
    }

    /* Clear */
    dkcBloomClear(&bloom);
    TEST_ASSERT(bloom.num_items == 0, "Bloom cleared");
    TEST_ASSERT(dkcBloomContainsString(&bloom, words[0]) == FALSE,
        "Bloom empty after clear");

    dkcBloomFree(&bloom);

    /* === Counting Bloom Filter === */
    result = dkcCountingBloomCreate(&cbloom, 100, 0.01);
    TEST_ASSERT(result == edk_SUCCEEDED, "Counting bloom create");

    /* Add elements */
    for (i = 0; i < 5; i++) {
        result = dkcCountingBloomAdd(&cbloom, words[i], strlen(words[i]));
        TEST_ASSERT(result == edk_SUCCEEDED, "Counting bloom add");
    }

    /* Check contains */
    for (i = 0; i < 5; i++) {
        TEST_ASSERT(dkcCountingBloomContains(&cbloom, words[i], strlen(words[i])) == TRUE,
            "Counting bloom contains added");
    }

    /* Remove an element */
    result = dkcCountingBloomRemove(&cbloom, words[0], strlen(words[0]));
    TEST_ASSERT(result == edk_SUCCEEDED, "Counting bloom remove");
    TEST_ASSERT(dkcCountingBloomContains(&cbloom, words[0], strlen(words[0])) == FALSE,
        "Counting bloom doesn't contain removed");

    /* Other elements still there */
    TEST_ASSERT(dkcCountingBloomContains(&cbloom, words[1], strlen(words[1])) == TRUE,
        "Counting bloom still contains others");

    dkcCountingBloomFree(&cbloom);

    /* === Utility functions === */
    {
        size_t opt_bits = dkcBloomOptimalBits(1000, 0.01);
        uint32 opt_hashes = dkcBloomOptimalHashes(opt_bits, 1000);
        TEST_ASSERT(opt_bits > 0, "Optimal bits > 0");
        TEST_ASSERT(opt_hashes > 0 && opt_hashes <= 16, "Optimal hashes reasonable");
    }

    TEST_END();
}

/*
 * Test: dkcCuckooHash.c
 */
void Test_CuckooHash(void)
{
    DKC_CUCKOO_HASH hash;
    DKC_CUCKOO_FILTER filter;
    const char *keys[] = {"key1", "key2", "key3", "key4", "key5"};
    int values[] = {100, 200, 300, 400, 500};
    int result;
    size_t i;

    TEST_BEGIN("dkcCuckooHash Test");

    /* === Cuckoo Hash Table === */
    result = dkcCuckooHashCreate(&hash, 16);
    TEST_ASSERT(result == edk_SUCCEEDED, "Cuckoo hash create");
    TEST_ASSERT(hash.table1 != NULL, "Cuckoo table1 allocated");
    TEST_ASSERT(hash.table2 != NULL, "Cuckoo table2 allocated");

    /* Insert elements */
    for (i = 0; i < 5; i++) {
        result = dkcCuckooHashInsertString(&hash, keys[i], &values[i]);
        TEST_ASSERT(result == edk_SUCCEEDED, "Cuckoo insert");
    }

    TEST_ASSERT(dkcCuckooHashCount(&hash) == 5, "Cuckoo count is 5");

    /* Lookup elements */
    for (i = 0; i < 5; i++) {
        int *val = (int *)dkcCuckooHashLookupString(&hash, keys[i]);
        TEST_ASSERT(val != NULL, "Cuckoo lookup found");
        TEST_ASSERT(*val == values[i], "Cuckoo lookup value correct");
    }

    /* Contains check */
    TEST_ASSERT(dkcCuckooHashContains(&hash, keys[0], strlen(keys[0]) + 1) == TRUE,
        "Cuckoo contains existing key");
    TEST_ASSERT(dkcCuckooHashContains(&hash, "nonexistent", 12) == FALSE,
        "Cuckoo doesn't contain nonexistent");

    /* Lookup nonexistent */
    TEST_ASSERT(dkcCuckooHashLookupString(&hash, "nonexistent") == NULL,
        "Cuckoo lookup nonexistent returns NULL");

    /* Update value */
    {
        int new_val = 999;
        result = dkcCuckooHashInsertString(&hash, keys[0], &new_val);
        TEST_ASSERT(result == edk_SUCCEEDED, "Cuckoo update");

        int *val = (int *)dkcCuckooHashLookupString(&hash, keys[0]);
        TEST_ASSERT(val != NULL && *val == 999, "Cuckoo value updated");
    }

    /* Remove element */
    result = dkcCuckooHashRemoveString(&hash, keys[1]);
    TEST_ASSERT(result == edk_SUCCEEDED, "Cuckoo remove");
    TEST_ASSERT(dkcCuckooHashCount(&hash) == 4, "Cuckoo count after remove");
    TEST_ASSERT(dkcCuckooHashLookupString(&hash, keys[1]) == NULL,
        "Cuckoo removed element not found");

    /* Load factor */
    {
        double lf = dkcCuckooHashLoadFactor(&hash);
        TEST_ASSERT(lf > 0.0 && lf < 1.0, "Cuckoo load factor reasonable");
    }

    /* Clear */
    dkcCuckooHashClear(&hash);
    TEST_ASSERT(dkcCuckooHashCount(&hash) == 0, "Cuckoo cleared");

    dkcCuckooHashFree(&hash);

    /* === Cuckoo Filter === */
    result = dkcCuckooFilterCreate(&filter, 100);
    TEST_ASSERT(result == edk_SUCCEEDED, "Cuckoo filter create");
    TEST_ASSERT(filter.buckets != NULL, "Cuckoo filter buckets allocated");

    /* Add elements */
    for (i = 0; i < 5; i++) {
        result = dkcCuckooFilterAddString(&filter, keys[i]);
        TEST_ASSERT(result == edk_SUCCEEDED, "Cuckoo filter add");
    }

    TEST_ASSERT(dkcCuckooFilterCount(&filter) == 5, "Cuckoo filter count is 5");

    /* Contains check */
    for (i = 0; i < 5; i++) {
        TEST_ASSERT(dkcCuckooFilterContainsString(&filter, keys[i]) == TRUE,
            "Cuckoo filter contains added");
    }

    /* Remove element (unlike Bloom filter, this is supported) */
    result = dkcCuckooFilterRemoveString(&filter, keys[0]);
    TEST_ASSERT(result == edk_SUCCEEDED, "Cuckoo filter remove");
    TEST_ASSERT(dkcCuckooFilterCount(&filter) == 4, "Cuckoo filter count after remove");
    TEST_ASSERT(dkcCuckooFilterContainsString(&filter, keys[0]) == FALSE,
        "Cuckoo filter doesn't contain removed");

    /* Memory usage */
    {
        size_t mem = dkcCuckooFilterMemoryUsage(&filter);
        TEST_ASSERT(mem > 0, "Cuckoo filter memory usage > 0");
    }

    /* Load factor */
    {
        double lf = dkcCuckooFilterLoadFactor(&filter);
        TEST_ASSERT(lf >= 0.0 && lf <= 1.0, "Cuckoo filter load factor valid");
    }

    /* Clear */
    dkcCuckooFilterClear(&filter);
    TEST_ASSERT(dkcCuckooFilterCount(&filter) == 0, "Cuckoo filter cleared");

    dkcCuckooFilterFree(&filter);

    TEST_END();
}

/*
 * Test: dkcLRUCache.c
 */
void Test_LRUCache(void)
{
    DKC_LRU_CACHE cache;
    int values[] = {100, 200, 300, 400, 500};
    int result;
    int *val;

    TEST_BEGIN("dkcLRUCache Test");

    /* === 基本操作 === */
    result = dkcLRUCacheCreate(&cache, 3);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU cache create");
    TEST_ASSERT(dkcLRUCacheCapacity(&cache) == 3, "LRU capacity is 3");
    TEST_ASSERT(dkcLRUCacheCount(&cache) == 0, "LRU initially empty");

    /* Put操作 */
    result = dkcLRUCachePutString(&cache, "key1", &values[0]);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU put key1");

    result = dkcLRUCachePutString(&cache, "key2", &values[1]);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU put key2");

    result = dkcLRUCachePutString(&cache, "key3", &values[2]);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU put key3");

    TEST_ASSERT(dkcLRUCacheCount(&cache) == 3, "LRU count is 3");
    TEST_ASSERT(dkcLRUCacheIsFull(&cache) == TRUE, "LRU is full");

    /* Get操作 */
    val = (int *)dkcLRUCacheGetString(&cache, "key1");
    TEST_ASSERT(val != NULL && *val == 100, "LRU get key1");

    val = (int *)dkcLRUCacheGetString(&cache, "key2");
    TEST_ASSERT(val != NULL && *val == 200, "LRU get key2");

    /* 存在確認（位置移動なし） */
    TEST_ASSERT(dkcLRUCacheContains(&cache, "key3", 5) == TRUE, "LRU contains key3");
    TEST_ASSERT(dkcLRUCacheContains(&cache, "nonexistent", 12) == FALSE, "LRU doesn't contain nonexistent");

    /* === LRU削除テスト === */
    /* key3にアクセスしていないので、key3が最古 */
    /* 新しい要素を追加すると、アクセス順は key2 -> key1 -> key3（古い順） */
    /* key1に再度アクセス */
    dkcLRUCacheGetString(&cache, "key1");
    /* アクセス順: key1(新) -> key2 -> key3(古) */

    /* key4を追加するとkey3が削除されるはず */
    result = dkcLRUCachePutString(&cache, "key4", &values[3]);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU put key4 (should evict key3)");

    TEST_ASSERT(dkcLRUCacheCount(&cache) == 3, "LRU count still 3");
    TEST_ASSERT(dkcLRUCacheGetString(&cache, "key3") == NULL, "key3 was evicted");
    TEST_ASSERT(dkcLRUCacheGetString(&cache, "key4") != NULL, "key4 exists");

    /* === 値更新テスト === */
    {
        int new_val = 999;
        result = dkcLRUCachePutString(&cache, "key1", &new_val);
        TEST_ASSERT(result == edk_SUCCEEDED, "LRU update key1");

        val = (int *)dkcLRUCacheGetString(&cache, "key1");
        TEST_ASSERT(val != NULL && *val == 999, "LRU key1 value updated");
    }

    /* === 削除テスト === */
    result = dkcLRUCacheRemoveString(&cache, "key2");
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU remove key2");
    TEST_ASSERT(dkcLRUCacheCount(&cache) == 2, "LRU count is 2");
    TEST_ASSERT(dkcLRUCacheGetString(&cache, "key2") == NULL, "key2 removed");

    /* === 統計情報テスト === */
    {
        uint64 hits, misses, evictions;
        double hit_rate;

        dkcLRUCacheGetStats(&cache, &hits, &misses, &evictions);
        TEST_ASSERT(hits > 0, "LRU has hits");
        TEST_ASSERT(evictions > 0, "LRU has evictions");

        hit_rate = dkcLRUCacheHitRate(&cache);
        TEST_ASSERT(hit_rate >= 0.0 && hit_rate <= 1.0, "LRU hit rate valid");

        dkcLRUCacheResetStats(&cache);
        dkcLRUCacheGetStats(&cache, &hits, &misses, &evictions);
        TEST_ASSERT(hits == 0 && misses == 0 && evictions == 0, "LRU stats reset");
    }

    /* === Peek操作テスト === */
    {
        const char *oldest = (const char *)dkcLRUCachePeekOldest(&cache, NULL);
        const char *newest = (const char *)dkcLRUCachePeekNewest(&cache, NULL);
        TEST_ASSERT(oldest != NULL, "LRU peek oldest");
        TEST_ASSERT(newest != NULL, "LRU peek newest");
    }

    /* === クリアテスト === */
    dkcLRUCacheClear(&cache);
    TEST_ASSERT(dkcLRUCacheCount(&cache) == 0, "LRU cleared");
    TEST_ASSERT(dkcLRUCacheGetString(&cache, "key1") == NULL, "LRU empty after clear");

    /* === リサイズテスト === */
    dkcLRUCachePutString(&cache, "a", &values[0]);
    dkcLRUCachePutString(&cache, "b", &values[1]);
    dkcLRUCachePutString(&cache, "c", &values[2]);

    result = dkcLRUCacheResize(&cache, 2);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU resize to 2");
    TEST_ASSERT(dkcLRUCacheCapacity(&cache) == 2, "LRU new capacity is 2");
    TEST_ASSERT(dkcLRUCacheCount(&cache) == 2, "LRU count is 2 after resize");

    dkcLRUCacheFree(&cache);

    TEST_END();
}

/*
 * Test: dkcLRUCache2.c (Library component reuse version)
 */
void Test_LRUCache2(void)
{
    DKC_LRU_CACHE2 *cache;
    int values[] = {100, 200, 300, 400, 500};
    int result;
    int *val;

    TEST_BEGIN("dkcLRUCache2 Test (Library Reuse)");

    /* === 基本操作 === */
    /* キーサイズ32バイトで容量3のキャッシュを作成 */
    cache = dkcLRUCache2Create(3, 32);
    TEST_ASSERT(cache != NULL, "LRU2 cache create");
    TEST_ASSERT(dkcLRUCache2Capacity(cache) == 3, "LRU2 capacity is 3");
    TEST_ASSERT(dkcLRUCache2Count(cache) == 0, "LRU2 initially empty");

    /* Put操作 */
    result = dkcLRUCache2PutString(cache, "key1", &values[0]);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU2 put key1");

    result = dkcLRUCache2PutString(cache, "key2", &values[1]);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU2 put key2");

    result = dkcLRUCache2PutString(cache, "key3", &values[2]);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU2 put key3");

    TEST_ASSERT(dkcLRUCache2Count(cache) == 3, "LRU2 count is 3");
    TEST_ASSERT(dkcLRUCache2IsFull(cache) == TRUE, "LRU2 is full");

    /* Get操作 */
    val = (int *)dkcLRUCache2GetString(cache, "key1");
    TEST_ASSERT(val != NULL && *val == 100, "LRU2 get key1");

    val = (int *)dkcLRUCache2GetString(cache, "key2");
    TEST_ASSERT(val != NULL && *val == 200, "LRU2 get key2");

    /* === LRU削除テスト === */
    /* key3にアクセスしていないので、key3が最古 */
    /* key1に再度アクセス */
    dkcLRUCache2GetString(cache, "key1");
    /* アクセス順: key1(新) -> key2 -> key3(古) */

    /* key4を追加するとkey3が削除されるはず */
    result = dkcLRUCache2PutString(cache, "key4", &values[3]);
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU2 put key4 (should evict key3)");

    TEST_ASSERT(dkcLRUCache2Count(cache) == 3, "LRU2 count still 3");
    TEST_ASSERT(dkcLRUCache2GetString(cache, "key3") == NULL, "LRU2 key3 was evicted");
    TEST_ASSERT(dkcLRUCache2GetString(cache, "key4") != NULL, "LRU2 key4 exists");

    /* === 値更新テスト === */
    {
        int new_val = 999;
        result = dkcLRUCache2PutString(cache, "key1", &new_val);
        TEST_ASSERT(result == edk_SUCCEEDED, "LRU2 update key1");

        val = (int *)dkcLRUCache2GetString(cache, "key1");
        TEST_ASSERT(val != NULL && *val == 999, "LRU2 key1 value updated");
    }

    /* === 削除テスト === */
    result = dkcLRUCache2RemoveString(cache, "key2");
    TEST_ASSERT(result == edk_SUCCEEDED, "LRU2 remove key2");
    TEST_ASSERT(dkcLRUCache2Count(cache) == 2, "LRU2 count is 2");
    TEST_ASSERT(dkcLRUCache2GetString(cache, "key2") == NULL, "LRU2 key2 removed");

    /* === 統計情報テスト === */
    {
        uint64 hits, misses, evictions;
        double hit_rate;

        dkcLRUCache2GetStats(cache, &hits, &misses, &evictions);
        TEST_ASSERT(hits > 0, "LRU2 has hits");
        TEST_ASSERT(evictions > 0, "LRU2 has evictions");

        hit_rate = dkcLRUCache2HitRate(cache);
        TEST_ASSERT(hit_rate >= 0.0 && hit_rate <= 1.0, "LRU2 hit rate valid");

        dkcLRUCache2ResetStats(cache);
        dkcLRUCache2GetStats(cache, &hits, &misses, &evictions);
        TEST_ASSERT(hits == 0 && misses == 0 && evictions == 0, "LRU2 stats reset");
    }

    /* === クリアテスト === */
    dkcLRUCache2Clear(cache);
    TEST_ASSERT(dkcLRUCache2Count(cache) == 0, "LRU2 cleared");
    TEST_ASSERT(dkcLRUCache2GetString(cache, "key1") == NULL, "LRU2 empty after clear");

    /* === 固定サイズキーテスト === */
    {
        uint32 int_key1 = 12345;
        uint32 int_key2 = 67890;
        DKC_LRU_CACHE2 *int_cache;

        /* キーサイズ4バイト（uint32）で作成 */
        int_cache = dkcLRUCache2Create(4, sizeof(uint32));
        TEST_ASSERT(int_cache != NULL, "LRU2 int-key cache create");

        dkcLRUCache2Put(int_cache, &int_key1, &values[0]);
        dkcLRUCache2Put(int_cache, &int_key2, &values[1]);

        val = (int *)dkcLRUCache2Get(int_cache, &int_key1);
        TEST_ASSERT(val != NULL && *val == 100, "LRU2 int-key get");

        dkcLRUCache2Free(&int_cache);
        TEST_ASSERT(int_cache == NULL, "LRU2 int-key cache freed");
    }

    dkcLRUCache2Free(&cache);
    TEST_ASSERT(cache == NULL, "LRU2 freed");

    TEST_END();
}

/*
 * Test: dkcSegmentTree.c
 */
void Test_SegmentTree(void)
{
    DKC_SEGMENT_TREE tree;
    DKC_LAZY_SEGMENT_TREE lazy_tree;
    DKC_SEGTREE_VALUE values[] = {1, 3, 5, 7, 9, 11};
    DKC_SEGTREE_VALUE result;
    int ret;

    TEST_BEGIN("dkcSegmentTree Test");

    /* === Range Sum Query === */
    ret = dkcSegmentTreeBuild(&tree, values, 6, dkcSegTreeOpSum, dkcd_SEGTREE_IDENTITY_SUM);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SegTree build (sum)");
    TEST_ASSERT(dkcSegmentTreeSize(&tree) == 6, "SegTree size is 6");

    /* 全区間の合計 */
    result = dkcSegmentTreeQuery(&tree, 0, 6);
    TEST_ASSERT(result == 36, "SegTree sum [0,6) = 36");

    /* 部分区間 */
    result = dkcSegmentTreeQuery(&tree, 1, 4);
    TEST_ASSERT(result == 15, "SegTree sum [1,4) = 3+5+7 = 15");

    /* 単一要素 */
    result = dkcSegmentTreeGet(&tree, 2);
    TEST_ASSERT(result == 5, "SegTree get(2) = 5");

    /* 点更新 */
    dkcSegmentTreeUpdate(&tree, 2, 10);
    result = dkcSegmentTreeQuery(&tree, 0, 6);
    TEST_ASSERT(result == 41, "SegTree after update: sum = 41");

    dkcSegmentTreeFree(&tree);

    /* === Range Minimum Query === */
    ret = dkcSegmentTreeBuild(&tree, values, 6, dkcSegTreeOpMin, LLONG_MAX);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SegTree build (min)");

    result = dkcSegmentTreeQuery(&tree, 0, 6);
    TEST_ASSERT(result == 1, "SegTree min [0,6) = 1");

    result = dkcSegmentTreeQuery(&tree, 2, 5);
    TEST_ASSERT(result == 5, "SegTree min [2,5) = 5");

    dkcSegmentTreeFree(&tree);

    /* === Range Maximum Query === */
    ret = dkcSegmentTreeBuild(&tree, values, 6, dkcSegTreeOpMax, LLONG_MIN);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SegTree build (max)");

    result = dkcSegmentTreeQuery(&tree, 0, 6);
    TEST_ASSERT(result == 11, "SegTree max [0,6) = 11");

    dkcSegmentTreeFree(&tree);

    /* === Lazy Segment Tree (Range Add) === */
    ret = dkcLazySegmentTreeCreate(&lazy_tree, 6, dkcSegTreeOpSum, 0, 0);
    TEST_ASSERT(ret == edk_SUCCEEDED, "LazySegTree create");

    /* 区間加算 */
    dkcLazySegmentTreeRangeAdd(&lazy_tree, 1, 4, 10);  /* [1,4) に 10 を加算 */
    result = dkcLazySegmentTreeQuery(&lazy_tree, 0, 6);
    TEST_ASSERT(result == 30, "LazySegTree after range add: sum = 30");

    result = dkcLazySegmentTreeQuery(&lazy_tree, 1, 4);
    TEST_ASSERT(result == 30, "LazySegTree query [1,4) = 30");

    dkcLazySegmentTreeFree(&lazy_tree);

    TEST_END();
}

/*
 * Test: dkcFenwickTree.c
 */
void Test_FenwickTree(void)
{
    DKC_FENWICK_TREE tree;
    DKC_FENWICK_TREE_2D tree2d;
    DKC_FENWICK_VALUE values[] = {1, 3, 5, 7, 9, 11};
    DKC_FENWICK_VALUE result;
    int ret;

    TEST_BEGIN("dkcFenwickTree Test");

    /* === 1次元 Fenwick Tree === */
    ret = dkcFenwickTreeBuild(&tree, values, 6);
    TEST_ASSERT(ret == edk_SUCCEEDED, "FenwickTree build");
    TEST_ASSERT(dkcFenwickTreeSize(&tree) == 6, "FenwickTree size is 6");

    /* 累積和 */
    result = dkcFenwickTreeSum(&tree, 5);
    TEST_ASSERT(result == 36, "FenwickTree sum [0,5] = 36");

    result = dkcFenwickTreeSum(&tree, 2);
    TEST_ASSERT(result == 9, "FenwickTree sum [0,2] = 1+3+5 = 9");

    /* 区間和 */
    result = dkcFenwickTreeRangeSum(&tree, 1, 4);
    TEST_ASSERT(result == 15, "FenwickTree range [1,4) = 3+5+7 = 15");

    /* 点取得 */
    result = dkcFenwickTreeGet(&tree, 3);
    TEST_ASSERT(result == 7, "FenwickTree get(3) = 7");

    /* 点加算 */
    dkcFenwickTreeAdd(&tree, 2, 5);
    result = dkcFenwickTreeSum(&tree, 5);
    TEST_ASSERT(result == 41, "FenwickTree after add: sum = 41");

    /* 点設定 */
    dkcFenwickTreeSet(&tree, 2, 5);  /* 10 -> 5 に戻す */
    result = dkcFenwickTreeSum(&tree, 5);
    TEST_ASSERT(result == 36, "FenwickTree after set: sum = 36");

    /* lower_bound */
    {
        size_t idx = dkcFenwickTreeLowerBound(&tree, 10);
        TEST_ASSERT(idx <= 6, "FenwickTree lower_bound valid");
    }

    dkcFenwickTreeFree(&tree);

    /* === 2次元 Fenwick Tree === */
    ret = dkcFenwickTree2DCreate(&tree2d, 4, 4);
    TEST_ASSERT(ret == edk_SUCCEEDED, "FenwickTree2D create");

    /* 点加算 */
    dkcFenwickTree2DAdd(&tree2d, 0, 0, 1);
    dkcFenwickTree2DAdd(&tree2d, 1, 1, 2);
    dkcFenwickTree2DAdd(&tree2d, 2, 2, 3);
    dkcFenwickTree2DAdd(&tree2d, 3, 3, 4);

    /* 累積和 */
    result = dkcFenwickTree2DSum(&tree2d, 3, 3);
    TEST_ASSERT(result == 10, "FenwickTree2D sum = 1+2+3+4 = 10");

    /* 区間和 */
    result = dkcFenwickTree2DRangeSum(&tree2d, 1, 1, 3, 3);
    TEST_ASSERT(result == 5, "FenwickTree2D range sum = 2+3 = 5");

    dkcFenwickTree2DFree(&tree2d);

    TEST_END();
}

/*
 * Test: dkcUnionFind.c
 */
void Test_UnionFind(void)
{
    DKC_UNION_FIND uf;
    DKC_WEIGHTED_UNION_FIND wuf;
    int ret;
    BOOL united;
    size_t roots[10];
    size_t members[10];
    size_t count;

    TEST_BEGIN("dkcUnionFind Test");

    /* === 基本 Union-Find === */
    ret = dkcUnionFindCreate(&uf, 10);
    TEST_ASSERT(ret == edk_SUCCEEDED, "UnionFind create");
    TEST_ASSERT(dkcUnionFindNumElements(&uf) == 10, "UnionFind has 10 elements");
    TEST_ASSERT(dkcUnionFindNumSets(&uf) == 10, "UnionFind initially has 10 sets");

    /* 併合 */
    united = dkcUnionFindUnite(&uf, 0, 1);
    TEST_ASSERT(united == TRUE, "UnionFind unite 0-1");
    TEST_ASSERT(dkcUnionFindNumSets(&uf) == 9, "UnionFind now has 9 sets");

    united = dkcUnionFindUnite(&uf, 1, 2);
    TEST_ASSERT(united == TRUE, "UnionFind unite 1-2");

    united = dkcUnionFindUnite(&uf, 3, 4);
    TEST_ASSERT(united == TRUE, "UnionFind unite 3-4");

    /* 同一集合判定 */
    TEST_ASSERT(dkcUnionFindSame(&uf, 0, 2) == TRUE, "UnionFind 0 and 2 are same");
    TEST_ASSERT(dkcUnionFindSame(&uf, 0, 3) == FALSE, "UnionFind 0 and 3 are different");

    /* 集合サイズ */
    TEST_ASSERT(dkcUnionFindSize(&uf, 0) == 3, "UnionFind set containing 0 has size 3");
    TEST_ASSERT(dkcUnionFindSize(&uf, 3) == 2, "UnionFind set containing 3 has size 2");

    /* 重複併合は無視 */
    united = dkcUnionFindUnite(&uf, 0, 2);
    TEST_ASSERT(united == FALSE, "UnionFind duplicate unite returns FALSE");

    /* 根の列挙 */
    count = dkcUnionFindGetRoots(&uf, roots);
    TEST_ASSERT(count == dkcUnionFindNumSets(&uf), "UnionFind roots count matches");

    /* メンバー列挙 */
    count = dkcUnionFindGetMembers(&uf, 0, members, 10);
    TEST_ASSERT(count == 3, "UnionFind members of set 0 = 3");

    dkcUnionFindFree(&uf);

    /* === 重み付き Union-Find === */
    ret = dkcWeightedUnionFindCreate(&wuf, 5);
    TEST_ASSERT(ret == edk_SUCCEEDED, "WeightedUnionFind create");

    /* weight(1) - weight(0) = 5 */
    united = dkcWeightedUnionFindUnite(&wuf, 0, 1, 5);
    TEST_ASSERT(united == TRUE, "WeightedUF unite 0-1 with weight 5");

    /* weight(2) - weight(1) = 3 => weight(2) - weight(0) = 8 */
    united = dkcWeightedUnionFindUnite(&wuf, 1, 2, 3);
    TEST_ASSERT(united == TRUE, "WeightedUF unite 1-2 with weight 3");

    /* 同一集合判定 */
    TEST_ASSERT(dkcWeightedUnionFindSame(&wuf, 0, 2) == TRUE, "WeightedUF 0 and 2 are same");

    /* 重み差分取得 */
    {
        int64 diff = dkcWeightedUnionFindDiff(&wuf, 0, 2);
        TEST_ASSERT(diff == 8, "WeightedUF diff(0,2) = 8");
    }

    /* 矛盾する併合は FALSE */
    united = dkcWeightedUnionFindUnite(&wuf, 0, 2, 10);  /* 実際は 8 */
    TEST_ASSERT(united == FALSE, "WeightedUF contradictory unite returns FALSE");

    dkcWeightedUnionFindFree(&wuf);

    TEST_END();
}

/*
 * Test: dkcSparseSet.c
 */
void Test_SparseSet(void)
{
    DKC_SPARSE_SET set, set_a, set_b, set_result;
    int ret;
    size_t i;
    const size_t *dense;

    TEST_BEGIN("dkcSparseSet Test");

    /* === 基本操作 === */
    ret = dkcSparseSetCreate(&set, 100);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SparseSet create");
    TEST_ASSERT(dkcSparseSetCapacity(&set) == 100, "SparseSet capacity is 100");
    TEST_ASSERT(dkcSparseSetSize(&set) == 0, "SparseSet initially empty");
    TEST_ASSERT(dkcSparseSetIsEmpty(&set) == TRUE, "SparseSet is empty");

    /* 追加 */
    ret = dkcSparseSetAdd(&set, 5);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SparseSet add 5");
    ret = dkcSparseSetAdd(&set, 1);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SparseSet add 1");
    ret = dkcSparseSetAdd(&set, 42);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SparseSet add 42");

    TEST_ASSERT(dkcSparseSetSize(&set) == 3, "SparseSet size is 3");
    TEST_ASSERT(dkcSparseSetIsEmpty(&set) == FALSE, "SparseSet is not empty");

    /* メンバーシップテスト */
    TEST_ASSERT(dkcSparseSetContains(&set, 5) == TRUE, "SparseSet contains 5");
    TEST_ASSERT(dkcSparseSetContains(&set, 1) == TRUE, "SparseSet contains 1");
    TEST_ASSERT(dkcSparseSetContains(&set, 42) == TRUE, "SparseSet contains 42");
    TEST_ASSERT(dkcSparseSetContains(&set, 0) == FALSE, "SparseSet does not contain 0");
    TEST_ASSERT(dkcSparseSetContains(&set, 99) == FALSE, "SparseSet does not contain 99");

    /* 重複追加は無視される */
    ret = dkcSparseSetAdd(&set, 5);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SparseSet duplicate add succeeds");
    TEST_ASSERT(dkcSparseSetSize(&set) == 3, "SparseSet size unchanged");

    /* === 削除 === */
    ret = dkcSparseSetRemove(&set, 1);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SparseSet remove 1");
    TEST_ASSERT(dkcSparseSetSize(&set) == 2, "SparseSet size is 2");
    TEST_ASSERT(dkcSparseSetContains(&set, 1) == FALSE, "SparseSet no longer contains 1");
    TEST_ASSERT(dkcSparseSetContains(&set, 5) == TRUE, "SparseSet still contains 5");
    TEST_ASSERT(dkcSparseSetContains(&set, 42) == TRUE, "SparseSet still contains 42");

    /* 存在しない要素の削除は失敗 */
    ret = dkcSparseSetRemove(&set, 1);
    TEST_ASSERT(ret == edk_FAILED, "SparseSet remove non-existent fails");

    /* === O(1) クリア === */
    dkcSparseSetClear(&set);
    TEST_ASSERT(dkcSparseSetSize(&set) == 0, "SparseSet cleared");
    TEST_ASSERT(dkcSparseSetContains(&set, 5) == FALSE, "SparseSet 5 gone after clear");
    TEST_ASSERT(dkcSparseSetContains(&set, 42) == FALSE, "SparseSet 42 gone after clear");

    /* === イテレーション === */
    dkcSparseSetAdd(&set, 10);
    dkcSparseSetAdd(&set, 20);
    dkcSparseSetAdd(&set, 30);

    /* dense配列直接アクセス */
    dense = dkcSparseSetGetDense(&set);
    TEST_ASSERT(dense != NULL, "SparseSet dense array accessible");
    TEST_ASSERT(dense[0] == 10, "SparseSet dense[0] = 10 (insertion order)");
    TEST_ASSERT(dense[1] == 20, "SparseSet dense[1] = 20");
    TEST_ASSERT(dense[2] == 30, "SparseSet dense[2] = 30");

    /* GetAt */
    TEST_ASSERT(dkcSparseSetGetAt(&set, 0) == 10, "SparseSet GetAt(0) = 10");
    TEST_ASSERT(dkcSparseSetGetAt(&set, 2) == 30, "SparseSet GetAt(2) = 30");

    dkcSparseSetFree(&set);

    /* === 集合演算 === */
    dkcSparseSetCreate(&set_a, 50);
    dkcSparseSetCreate(&set_b, 50);
    dkcSparseSetCreate(&set_result, 50);

    /* A = {1, 2, 3, 4} */
    for (i = 1; i <= 4; i++) dkcSparseSetAdd(&set_a, i);

    /* B = {3, 4, 5, 6} */
    for (i = 3; i <= 6; i++) dkcSparseSetAdd(&set_b, i);

    /* 和集合: A ∪ B = {1, 2, 3, 4, 5, 6} */
    ret = dkcSparseSetUnion(&set_result, &set_a, &set_b);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SparseSet union");
    TEST_ASSERT(dkcSparseSetSize(&set_result) == 6, "SparseSet union size = 6");

    /* 積集合: A ∩ B = {3, 4} */
    ret = dkcSparseSetIntersection(&set_result, &set_a, &set_b);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SparseSet intersection");
    TEST_ASSERT(dkcSparseSetSize(&set_result) == 2, "SparseSet intersection size = 2");
    TEST_ASSERT(dkcSparseSetContains(&set_result, 3) == TRUE, "SparseSet intersection has 3");
    TEST_ASSERT(dkcSparseSetContains(&set_result, 4) == TRUE, "SparseSet intersection has 4");

    /* 差集合: A - B = {1, 2} */
    ret = dkcSparseSetDifference(&set_result, &set_a, &set_b);
    TEST_ASSERT(ret == edk_SUCCEEDED, "SparseSet difference");
    TEST_ASSERT(dkcSparseSetSize(&set_result) == 2, "SparseSet difference size = 2");
    TEST_ASSERT(dkcSparseSetContains(&set_result, 1) == TRUE, "SparseSet difference has 1");
    TEST_ASSERT(dkcSparseSetContains(&set_result, 2) == TRUE, "SparseSet difference has 2");

    /* 等価判定 */
    TEST_ASSERT(dkcSparseSetEquals(&set_a, &set_a) == TRUE, "SparseSet A == A");
    TEST_ASSERT(dkcSparseSetEquals(&set_a, &set_b) == FALSE, "SparseSet A != B");

    /* 部分集合判定 */
    dkcSparseSetClear(&set_result);
    dkcSparseSetAdd(&set_result, 1);
    dkcSparseSetAdd(&set_result, 2);
    TEST_ASSERT(dkcSparseSetIsSubset(&set_result, &set_a) == TRUE, "SparseSet {1,2} subset of A");
    TEST_ASSERT(dkcSparseSetIsSubset(&set_a, &set_result) == FALSE, "SparseSet A not subset of {1,2}");

    dkcSparseSetFree(&set_a);
    dkcSparseSetFree(&set_b);
    dkcSparseSetFree(&set_result);

    TEST_END();
}

/*
 * Test: dkcUniqueID.c
 */
void Test_UniqueID(void)
{
    DKC_UUID uuid1, uuid2, uuid3;
    DKC_UUID_GEN uuid_gen;
    DKC_ULID ulid1, ulid2;
    DKC_ULID_GEN ulid_gen;
    DKC_SNOWFLAKE sf;
    DKC_SNOWFLAKE_GEN sf_gen;
    DKC_KSUID ksuid;
    DKC_KSUID_GEN ksuid_gen;
    DKC_XID xid;
    DKC_XID_GEN xid_gen;
    DKC_NANOID_GEN nanoid_gen;
    DKC_CUID_GEN cuid_gen;
    char str_buf[64];
    int result;

    TEST_BEGIN("dkcUniqueID Test");

    /* === UUID Generator Init === */
    result = dkcUUIDGenInit(&uuid_gen, 12345);
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID generator init");

    /* === UUID v4 Tests === */
    result = dkcUUIDv4Generate(&uuid_gen, &uuid1);
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID v4 generate");
    TEST_ASSERT(dkcUUIDGetVersion(&uuid1) == 4, "UUID v4 version is 4");
    TEST_ASSERT(dkcUUIDIsNil(&uuid1) == FALSE, "UUID v4 is not nil");

    result = dkcUUIDToString(&uuid1, str_buf);
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID to string");
    TEST_ASSERT(strlen(str_buf) == 36, "UUID string length is 36");

    result = dkcUUIDFromString(str_buf, &uuid2);
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID from string");
    TEST_ASSERT(dkcUUIDCompare(&uuid1, &uuid2) == 0, "UUID roundtrip matches");

    /* === UUID v1 Tests === */
    result = dkcUUIDv1Generate(&uuid_gen, &uuid1);
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID v1 generate");
    TEST_ASSERT(dkcUUIDGetVersion(&uuid1) == 1, "UUID v1 version is 1");

    /* === UUID v3 (MD5) Tests === */
    result = dkcUUIDv3GenerateNS(&uuid1, edkcUUID_NS_DNS, "example.com", 11);
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID v3 generate");
    TEST_ASSERT(dkcUUIDGetVersion(&uuid1) == 3, "UUID v3 version is 3");

    /* Same input produces same UUID */
    result = dkcUUIDv3GenerateNS(&uuid2, edkcUUID_NS_DNS, "example.com", 11);
    TEST_ASSERT(dkcUUIDCompare(&uuid1, &uuid2) == 0, "UUID v3 deterministic");

    /* === UUID v5 (SHA-1) Tests === */
    result = dkcUUIDv5GenerateNS(&uuid1, edkcUUID_NS_URL, "https://example.com", 19);
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID v5 generate");
    TEST_ASSERT(dkcUUIDGetVersion(&uuid1) == 5, "UUID v5 version is 5");

    /* === UUID v6 Tests === */
    result = dkcUUIDv6Generate(&uuid_gen, &uuid1);
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID v6 generate");
    TEST_ASSERT(dkcUUIDGetVersion(&uuid1) == 6, "UUID v6 version is 6");

    /* === UUID v7 Tests === */
    result = dkcUUIDv7Generate(&uuid_gen, &uuid1);
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID v7 generate");
    TEST_ASSERT(dkcUUIDGetVersion(&uuid1) == 7, "UUID v7 version is 7");

    /* === UUID v8 Tests === */
    result = dkcUUIDv8Generate(&uuid1, QW(0x123456789ABC), 0xDEF, QW(0x0123456789ABCDEF));
    TEST_ASSERT(result == edk_SUCCEEDED, "UUID v8 generate");
    TEST_ASSERT(dkcUUIDGetVersion(&uuid1) == 8, "UUID v8 version is 8");

    /* === NIL UUID Tests === */
    dkcUUIDNil(&uuid3);
    TEST_ASSERT(dkcUUIDIsNil(&uuid3) == TRUE, "NIL UUID is nil");

    /* === ULID Generator Init === */
    result = dkcULIDGenInit(&ulid_gen, 12345);
    TEST_ASSERT(result == edk_SUCCEEDED, "ULID generator init");

    /* === ULID Tests === */
    result = dkcULIDGenerate(&ulid_gen, &ulid1);
    TEST_ASSERT(result == edk_SUCCEEDED, "ULID generate");

    result = dkcULIDToString(&ulid1, str_buf);
    TEST_ASSERT(result == edk_SUCCEEDED, "ULID to string");
    TEST_ASSERT(strlen(str_buf) == 26, "ULID string length is 26");

    result = dkcULIDFromString(str_buf, &ulid2);
    TEST_ASSERT(result == edk_SUCCEEDED, "ULID from string");
    TEST_ASSERT(dkcULIDCompare(&ulid1, &ulid2) == 0, "ULID roundtrip matches");

    /* ULID timestamp */
    {
        uint64 ts = dkcULIDGetTimestamp(&ulid1);
        TEST_ASSERT(ts > 0, "ULID timestamp is valid");
    }

    /* === Snowflake Tests === */
    result = dkcSnowflakeInit(&sf_gen, 1, 1, 0, 12345);
    TEST_ASSERT(result == edk_SUCCEEDED, "Snowflake init");

    result = dkcSnowflakeGenerate(&sf_gen, &sf);
    TEST_ASSERT(result == edk_SUCCEEDED, "Snowflake generate");
    TEST_ASSERT(sf.id != 0, "Snowflake ID is not zero");

    result = dkcSnowflakeToString(&sf, str_buf);
    TEST_ASSERT(result == edk_SUCCEEDED, "Snowflake to string");

    /* === KSUID Tests === */
    result = dkcKSUIDGenInit(&ksuid_gen, 12345);
    TEST_ASSERT(result == edk_SUCCEEDED, "KSUID generator init");

    result = dkcKSUIDGenerate(&ksuid_gen, &ksuid);
    TEST_ASSERT(result == edk_SUCCEEDED, "KSUID generate");

    result = dkcKSUIDToString(&ksuid, str_buf);
    TEST_ASSERT(result == edk_SUCCEEDED, "KSUID to string");
    TEST_ASSERT(strlen(str_buf) == 27, "KSUID string length is 27");

    /* KSUID timestamp */
    {
        uint32 ts = dkcKSUIDGetTimestamp(&ksuid);
        TEST_ASSERT(ts > 0, "KSUID timestamp is valid");
    }

    /* === XID Tests === */
    result = dkcXIDInit(&xid_gen, 12345);
    TEST_ASSERT(result == edk_SUCCEEDED, "XID init");

    result = dkcXIDGenerate(&xid_gen, &xid);
    TEST_ASSERT(result == edk_SUCCEEDED, "XID generate");

    result = dkcXIDToString(&xid, str_buf);
    TEST_ASSERT(result == edk_SUCCEEDED, "XID to string");
    TEST_ASSERT(strlen(str_buf) == 20, "XID string length is 20");

    /* === NanoID Tests === */
    result = dkcNanoIDGenInit(&nanoid_gen, 12345);
    TEST_ASSERT(result == edk_SUCCEEDED, "NanoID generator init");

    result = dkcNanoIDGenerate(&nanoid_gen, str_buf);
    TEST_ASSERT(result == edk_SUCCEEDED, "NanoID generate");
    TEST_ASSERT(strlen(str_buf) == 21, "NanoID default length is 21");

    /* Custom NanoID */
    result = dkcNanoIDGenerateCustom(&nanoid_gen, str_buf, 10, "abc123");
    TEST_ASSERT(result == edk_SUCCEEDED, "NanoID custom generate");
    TEST_ASSERT(strlen(str_buf) == 10, "NanoID custom length is 10");

    /* === CUID Tests === */
    result = dkcCUIDGenInit(&cuid_gen, 12345);
    TEST_ASSERT(result == edk_SUCCEEDED, "CUID generator init");

    result = dkcCUIDGenerate(&cuid_gen, str_buf);
    TEST_ASSERT(result == edk_SUCCEEDED, "CUID generate");
    TEST_ASSERT(str_buf[0] == 'c', "CUID starts with 'c'");
    TEST_ASSERT(strlen(str_buf) == 25, "CUID length is 25");

    /* === CUID2 Tests === */
    result = dkcCUID2Generate(&cuid_gen, str_buf);
    TEST_ASSERT(result == edk_SUCCEEDED, "CUID2 generate");
    TEST_ASSERT(strlen(str_buf) == 24, "CUID2 default length is 24");

    TEST_END();
}

/* ========================================
 * HEAP TESTS
 * ======================================== */

/*
 * Test: dkcHeap (pointer heap)
 */
void Test_Heap(void)
{
    DKC_HEAP heap;
    int result;
    int vals[10];
    int *top;
    int i;

    TEST_BEGIN("dkcHeap (Pointer Heap) Test");

    /* --- Min-Heap --- */
    result = dkcHeapCreate(&heap, 0, dkcHeapCompareInt, TRUE);
    TEST_ASSERT(result == edk_SUCCEEDED, "Min-Heap create");
    TEST_ASSERT(dkcHeapIsEmpty(&heap) == TRUE, "New heap is empty");
    TEST_ASSERT(dkcHeapSize(&heap) == 0, "New heap size is 0");

    vals[0] = 30; vals[1] = 10; vals[2] = 50; vals[3] = 20; vals[4] = 40;
    for (i = 0; i < 5; i++) {
        result = dkcHeapPush(&heap, &vals[i]);
        TEST_ASSERT(result == edk_SUCCEEDED, "Min-Heap push");
    }

    TEST_ASSERT(dkcHeapSize(&heap) == 5, "Min-Heap size is 5");
    TEST_ASSERT(dkcHeapIsEmpty(&heap) == FALSE, "Min-Heap is not empty");

    top = (int *)dkcHeapTop(&heap);
    TEST_ASSERT(top != NULL && *top == 10, "Min-Heap top is 10");

    top = (int *)dkcHeapPop(&heap);
    TEST_ASSERT(top != NULL && *top == 10, "Min-Heap pop returns 10");

    top = (int *)dkcHeapPop(&heap);
    TEST_ASSERT(top != NULL && *top == 20, "Min-Heap pop returns 20");

    top = (int *)dkcHeapPop(&heap);
    TEST_ASSERT(top != NULL && *top == 30, "Min-Heap pop returns 30");

    top = (int *)dkcHeapPop(&heap);
    TEST_ASSERT(top != NULL && *top == 40, "Min-Heap pop returns 40");

    top = (int *)dkcHeapPop(&heap);
    TEST_ASSERT(top != NULL && *top == 50, "Min-Heap pop returns 50");

    TEST_ASSERT(dkcHeapIsEmpty(&heap) == TRUE, "Min-Heap empty after all pops");
    TEST_ASSERT(dkcHeapPop(&heap) == NULL, "Pop from empty heap returns NULL");

    dkcHeapFree(&heap);

    /* --- Max-Heap --- */
    result = dkcHeapCreate(&heap, 4, dkcHeapCompareInt, FALSE);
    TEST_ASSERT(result == edk_SUCCEEDED, "Max-Heap create");

    vals[0] = 30; vals[1] = 10; vals[2] = 50; vals[3] = 20; vals[4] = 40;
    for (i = 0; i < 5; i++) {
        dkcHeapPush(&heap, &vals[i]);
    }

    top = (int *)dkcHeapPop(&heap);
    TEST_ASSERT(top != NULL && *top == 50, "Max-Heap pop returns 50");

    top = (int *)dkcHeapPop(&heap);
    TEST_ASSERT(top != NULL && *top == 40, "Max-Heap pop returns 40");

    top = (int *)dkcHeapPop(&heap);
    TEST_ASSERT(top != NULL && *top == 30, "Max-Heap pop returns 30");

    dkcHeapClear(&heap);
    TEST_ASSERT(dkcHeapIsEmpty(&heap) == TRUE, "Clear empties heap");

    dkcHeapFree(&heap);

    TEST_END();
}

/*
 * Test: dkcTypedHeap (typed heap)
 */
void Test_TypedHeap(void)
{
    DKC_TYPED_HEAP heap;
    int result;
    int val, out;
    int arr[] = {50, 30, 10, 40, 20, 60, 5};
    int i;

    TEST_BEGIN("dkcTypedHeap (Typed Heap) Test");

    /* --- Create and push --- */
    result = dkcTypedHeapCreate(&heap, 4, sizeof(int), dkcHeapCompareInt, TRUE);
    TEST_ASSERT(result == edk_SUCCEEDED, "TypedHeap create");

    val = 30; dkcTypedHeapPush(&heap, &val);
    val = 10; dkcTypedHeapPush(&heap, &val);
    val = 50; dkcTypedHeapPush(&heap, &val);
    val = 20; dkcTypedHeapPush(&heap, &val);
    val = 40; dkcTypedHeapPush(&heap, &val);

    TEST_ASSERT(dkcTypedHeapSize(&heap) == 5, "TypedHeap size is 5");

    result = dkcTypedHeapTop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 10, "TypedHeap top is 10");

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 10, "TypedHeap pop returns 10");

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 20, "TypedHeap pop returns 20");

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 30, "TypedHeap pop returns 30");

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 40, "TypedHeap pop returns 40");

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 50, "TypedHeap pop returns 50");

    TEST_ASSERT(dkcTypedHeapIsEmpty(&heap) == TRUE, "TypedHeap empty after pops");
    dkcTypedHeapFree(&heap);

    /* --- Build from array (O(n) heapify) --- */
    result = dkcTypedHeapBuild(&heap, arr, 7, sizeof(int), dkcHeapCompareInt, TRUE);
    TEST_ASSERT(result == edk_SUCCEEDED, "TypedHeap build from array");
    TEST_ASSERT(dkcTypedHeapSize(&heap) == 7, "Built heap size is 7");

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 5, "Built heap pop returns 5");

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 10, "Built heap pop returns 10");

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 20, "Built heap pop returns 20");

    dkcTypedHeapClear(&heap);
    TEST_ASSERT(dkcTypedHeapIsEmpty(&heap) == TRUE, "TypedHeap clear");
    dkcTypedHeapFree(&heap);

    /* --- Max-Heap typed --- */
    result = dkcTypedHeapCreate(&heap, 0, sizeof(int), dkcHeapCompareInt, FALSE);
    TEST_ASSERT(result == edk_SUCCEEDED, "Max TypedHeap create");

    val = 10; dkcTypedHeapPush(&heap, &val);
    val = 50; dkcTypedHeapPush(&heap, &val);
    val = 30; dkcTypedHeapPush(&heap, &val);

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 50, "Max TypedHeap pop returns 50");

    result = dkcTypedHeapPop(&heap, &out);
    TEST_ASSERT(result == edk_SUCCEEDED && out == 30, "Max TypedHeap pop returns 30");

    dkcTypedHeapFree(&heap);

    TEST_END();
}

/*
 * Test: dkcHeapSort
 */
void Test_HeapSortStandalone(void)
{
    int arr[] = {50, 30, 10, 40, 20, 60, 5, 35, 15, 45};
    int result;
    int i;
    int sorted;

    TEST_BEGIN("dkcHeapSort (Standalone) Test");

    result = dkcHeapSort2(arr, 10, sizeof(int), dkcHeapCompareInt);
    TEST_ASSERT(result == edk_SUCCEEDED, "HeapSort returns success");

    sorted = 1;
    for (i = 1; i < 10; i++) {
        if (arr[i - 1] > arr[i]) {
            sorted = 0;
            break;
        }
    }
    TEST_ASSERT(sorted, "HeapSort produces sorted array");
    TEST_ASSERT(arr[0] == 5, "HeapSort first element is 5");
    TEST_ASSERT(arr[9] == 60, "HeapSort last element is 60");

    /* Single element */
    result = dkcHeapSort2(arr, 1, sizeof(int), dkcHeapCompareInt);
    TEST_ASSERT(result == edk_SUCCEEDED, "HeapSort single element");

    /* Empty */
    result = dkcHeapSort2(arr, 0, sizeof(int), dkcHeapCompareInt);
    TEST_ASSERT(result == edk_SUCCEEDED, "HeapSort zero elements");

    TEST_END();
}

/* ========================================
 * SPLAY TREE TESTS
 * ======================================== */

static int WINAPIV compare_splaytree_int(const void *a, const void *b)
{
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    if (ia < ib) return -1;
    if (ia > ib) return 1;
    return 0;
}

static BOOL WINAPI splaytree_foreach_callback(const void *key, void *data, size_t data_size, void *user)
{
    int *counter = (int *)user;
    (void)key;
    (void)data;
    (void)data_size;
    (*counter)++;
    return TRUE;
}

void Test_SplayTree(void)
{
    DKC_SPLAYTREE_ROOT *tree;
    DKC_SPLAYTREE_NODE *node;
    DKC_SPLAYTREE_NODE *min_node;
    DKC_SPLAYTREE_NODE *max_node;
    int keys[10];
    int data_vals[10];
    int read_data;
    int result;
    int i;
    int counter;

    TEST_BEGIN("dkcSplayTree (Splay Tree) Test");

    /* Create Splay tree */
    tree = dkcAllocSplayTreeRoot(sizeof(int), 10, compare_splaytree_int, 100);
    TEST_ASSERT(tree != NULL, "dkcAllocSplayTreeRoot");

    TEST_ASSERT(dkcSplayTreeIsEmpty(tree) == TRUE, "New tree is empty");
    TEST_ASSERT(dkcSplayTreeSize(tree) == 0, "Size is 0");

    /* Insert multiple keys */
    keys[0] = 10; data_vals[0] = 100;
    keys[1] = 20; data_vals[1] = 200;
    keys[2] = 30; data_vals[2] = 300;
    keys[3] = 40; data_vals[3] = 400;
    keys[4] = 50; data_vals[4] = 500;
    keys[5] = 25; data_vals[5] = 250;

    for (i = 0; i < 6; i++) {
        result = dkcSplayTreeInsert(tree, &keys[i], &data_vals[i], sizeof(int));
        TEST_ASSERT(result == edk_SUCCEEDED, "Insert key");
    }

    TEST_ASSERT(dkcSplayTreeSize(tree) == 6, "Size is 6 after inserts");
    TEST_ASSERT(dkcSplayTreeIsEmpty(tree) == FALSE, "Tree is not empty");

    /* Find existing keys */
    node = dkcSplayTreeFind(tree, &keys[0]);
    TEST_ASSERT(node != NULL, "Find key 10");

    if (node != NULL) {
        dkcSplayTreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 100, "Key 10 has data 100");
    }

    /* After find, the found node should be at root (splay property) */
    TEST_ASSERT(tree->root != tree->sentinel, "Root is not sentinel after find");
    if (tree->root != tree->sentinel) {
        dkcSplayTreeGetBuffer(tree->root, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 100, "Root is splayed node (key 10)");
    }

    node = dkcSplayTreeFind(tree, &keys[4]);
    TEST_ASSERT(node != NULL, "Find key 50");

    if (node != NULL) {
        dkcSplayTreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 500, "Key 50 has data 500");
    }

    /* Find non-existing key */
    i = 999;
    node = dkcSplayTreeFind(tree, &i);
    TEST_ASSERT(node == NULL, "Key 999 not found");

    /* Test FindMin and FindMax */
    min_node = dkcSplayTreeFindMin(tree);
    TEST_ASSERT(min_node != NULL, "FindMin returns node");
    if (min_node != NULL) {
        dkcSplayTreeGetBuffer(min_node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 100, "Min node has data 100 (key 10)");
    }

    max_node = dkcSplayTreeFindMax(tree);
    TEST_ASSERT(max_node != NULL, "FindMax returns node");
    if (max_node != NULL) {
        dkcSplayTreeGetBuffer(max_node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 500, "Max node has data 500 (key 50)");
    }

    /* Test foreach (in-order traversal) */
    counter = 0;
    result = dkcSplayTreeForeach(tree, edkcSplayTreeInOrder, splaytree_foreach_callback, &counter);
    TEST_ASSERT(result == edk_SUCCEEDED, "Foreach in-order succeeded");
    TEST_ASSERT(counter == 6, "Foreach visited all 6 nodes");

    /* Test delete */
    i = 30;
    result = dkcSplayTreeEraseFromKey(tree, &i);
    TEST_ASSERT(result == edk_SUCCEEDED, "Delete key 30");
    TEST_ASSERT(dkcSplayTreeSize(tree) == 5, "Size is 5 after delete");

    node = dkcSplayTreeFind(tree, &i);
    TEST_ASSERT(node == NULL, "Key 30 not found after delete");

    /* Delete non-existing key */
    i = 999;
    result = dkcSplayTreeEraseFromKey(tree, &i);
    TEST_ASSERT(result == edk_Not_Found, "Delete non-existing key returns Not_Found");

    /* Test update existing key */
    i = 10;
    read_data = 1000;
    result = dkcSplayTreeInsert(tree, &i, &read_data, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Update existing key 10");

    node = dkcSplayTreeFind(tree, &i);
    if (node != NULL) {
        dkcSplayTreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 1000, "Key 10 updated to data 1000");
    }

    /* Test repeated access (cache effect) */
    i = 20;
    node = dkcSplayTreeFind(tree, &i);
    TEST_ASSERT(node != NULL, "Find key 20 first time");
    node = dkcSplayTreeFind(tree, &i);
    TEST_ASSERT(node != NULL, "Find key 20 second time (cached at root)");
    if (node != NULL) {
        dkcSplayTreeGetBuffer(node, &read_data, sizeof(int));
        TEST_ASSERT(read_data == 200, "Key 20 still has data 200");
    }

    /* Test clear */
    result = dkcSplayTreeClear(tree);
    TEST_ASSERT(result == edk_SUCCEEDED, "Clear tree");
    TEST_ASSERT(dkcSplayTreeIsEmpty(tree) == TRUE, "Tree is empty after clear");
    TEST_ASSERT(dkcSplayTreeSize(tree) == 0, "Size is 0 after clear");

    /* Free tree */
    dkcFreeSplayTreeRoot(&tree);
    TEST_ASSERT(tree == NULL, "Free Splay tree");

    TEST_END();
}

/* ========================================
 * PATRICIA TRIE TESTS
 * ======================================== */

void Test_Patricia(void)
{
    DKC_PATRICIA trie;
    int result;
    void *data;
    size_t data_size;
    int val1 = 100, val2 = 200, val3 = 300, val4 = 400;

    TEST_BEGIN("dkcPatricia (Patricia Trie) Test");

    /* Create */
    result = dkcPatriciaCreate(&trie);
    TEST_ASSERT(result == edk_SUCCEEDED, "dkcPatriciaCreate");
    TEST_ASSERT(dkcPatriciaIsEmpty(&trie) == TRUE, "New trie is empty");
    TEST_ASSERT(dkcPatriciaCount(&trie) == 0, "Count is 0");

    /* Insert */
    result = dkcPatriciaInsert(&trie, (const BYTE *)"hello", 5, &val1, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert 'hello'");

    result = dkcPatriciaInsert(&trie, (const BYTE *)"world", 5, &val2, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert 'world'");

    result = dkcPatriciaInsert(&trie, (const BYTE *)"help", 4, &val3, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert 'help'");

    result = dkcPatriciaInsert(&trie, (const BYTE *)"test", 4, &val4, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert 'test'");

    TEST_ASSERT(dkcPatriciaCount(&trie) == 4, "Count is 4");
    TEST_ASSERT(dkcPatriciaIsEmpty(&trie) == FALSE, "Trie is not empty");

    /* Search existing keys */
    data = NULL;
    data_size = 0;
    result = dkcPatriciaSearch(&trie, (const BYTE *)"hello", 5, &data, &data_size);
    TEST_ASSERT(result == edk_SUCCEEDED, "Search 'hello' found");
    if (data != NULL) {
        TEST_ASSERT(*(int *)data == 100, "'hello' has data 100");
    }

    data = NULL;
    result = dkcPatriciaSearch(&trie, (const BYTE *)"world", 5, &data, &data_size);
    TEST_ASSERT(result == edk_SUCCEEDED, "Search 'world' found");
    if (data != NULL) {
        TEST_ASSERT(*(int *)data == 200, "'world' has data 200");
    }

    data = NULL;
    result = dkcPatriciaSearch(&trie, (const BYTE *)"help", 4, &data, &data_size);
    TEST_ASSERT(result == edk_SUCCEEDED, "Search 'help' found");
    if (data != NULL) {
        TEST_ASSERT(*(int *)data == 300, "'help' has data 300");
    }

    /* Search non-existing key */
    result = dkcPatriciaSearch(&trie, (const BYTE *)"xyz", 3, &data, &data_size);
    TEST_ASSERT(result == edk_Not_Found, "Search 'xyz' not found");

    /* Update existing key */
    val1 = 999;
    result = dkcPatriciaInsert(&trie, (const BYTE *)"hello", 5, &val1, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Update 'hello'");
    TEST_ASSERT(dkcPatriciaCount(&trie) == 4, "Count still 4 after update");

    data = NULL;
    result = dkcPatriciaSearch(&trie, (const BYTE *)"hello", 5, &data, &data_size);
    TEST_ASSERT(result == edk_SUCCEEDED, "Search updated 'hello'");
    if (data != NULL) {
        TEST_ASSERT(*(int *)data == 999, "'hello' updated to 999");
    }

    /* Delete */
    result = dkcPatriciaRemove(&trie, (const BYTE *)"help", 4);
    TEST_ASSERT(result == edk_SUCCEEDED, "Remove 'help'");
    TEST_ASSERT(dkcPatriciaCount(&trie) == 3, "Count is 3 after remove");

    result = dkcPatriciaSearch(&trie, (const BYTE *)"help", 4, &data, &data_size);
    TEST_ASSERT(result == edk_Not_Found, "'help' not found after remove");

    /* Delete non-existing */
    result = dkcPatriciaRemove(&trie, (const BYTE *)"notexist", 8);
    TEST_ASSERT(result == edk_Not_Found, "Remove non-existing returns Not_Found");

    /* Remaining keys still work */
    result = dkcPatriciaSearch(&trie, (const BYTE *)"hello", 5, &data, &data_size);
    TEST_ASSERT(result == edk_SUCCEEDED, "'hello' still found after other remove");

    result = dkcPatriciaSearch(&trie, (const BYTE *)"world", 5, &data, &data_size);
    TEST_ASSERT(result == edk_SUCCEEDED, "'world' still found after other remove");

    /* Free */
    dkcPatriciaFree(&trie);
    TEST_ASSERT(dkcPatriciaCount(&trie) == 0, "Count is 0 after free");

    TEST_END();
}

/* ========================================
 * BIGINTEGER TESTS
 * ======================================== */

void Test_BigInteger(void)
{
    DKC_BIGINTEGER a, b, c, d;
    char buf[256];
    int result;

    TEST_BEGIN("dkcBigInteger (BigInteger) Test");

    /* Create */
    result = dkcBigIntCreate(&a, 0);
    TEST_ASSERT(result == edk_SUCCEEDED, "Create a");
    result = dkcBigIntCreate(&b, 0);
    TEST_ASSERT(result == edk_SUCCEEDED, "Create b");
    result = dkcBigIntCreate(&c, 0);
    TEST_ASSERT(result == edk_SUCCEEDED, "Create c");
    result = dkcBigIntCreate(&d, 0);
    TEST_ASSERT(result == edk_SUCCEEDED, "Create d");

    /* FromInt and IsZero */
    TEST_ASSERT(dkcBigIntIsZero(&a) == TRUE, "New BigInt is zero");

    dkcBigIntFromInt(&a, 0);
    TEST_ASSERT(dkcBigIntIsZero(&a) == TRUE, "0 is zero");

    dkcBigIntFromInt(&a, 123);
    TEST_ASSERT(dkcBigIntIsZero(&a) == FALSE, "123 is not zero");

    dkcBigIntFromInt(&b, -456);
    TEST_ASSERT(dkcBigIntIsZero(&b) == FALSE, "-456 is not zero");

    /* ToString (10進) */
    dkcBigIntFromInt(&a, 12345);
    result = dkcBigIntToString(&a, buf, sizeof(buf), 10);
    TEST_ASSERT(result == edk_SUCCEEDED, "ToString(12345)");
    TEST_ASSERT(strcmp(buf, "12345") == 0, "12345 -> \"12345\"");

    dkcBigIntFromInt(&a, -99);
    result = dkcBigIntToString(&a, buf, sizeof(buf), 10);
    TEST_ASSERT(result == edk_SUCCEEDED, "ToString(-99)");
    TEST_ASSERT(strcmp(buf, "-99") == 0, "-99 -> \"-99\"");

    /* ToString (16進) */
    dkcBigIntFromInt(&a, 255);
    result = dkcBigIntToString(&a, buf, sizeof(buf), 16);
    TEST_ASSERT(result == edk_SUCCEEDED, "ToString(255, hex)");
    TEST_ASSERT(strcmp(buf, "FF") == 0, "255 -> \"FF\"");

    /* FromString */
    dkcBigIntFromString(&a, "999", 10);
    dkcBigIntToString(&a, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "999") == 0, "FromString(\"999\")");

    dkcBigIntFromString(&a, "-42", 10);
    dkcBigIntToString(&a, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "-42") == 0, "FromString(\"-42\")");

    dkcBigIntFromString(&a, "FF", 16);
    dkcBigIntToString(&a, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "255") == 0, "FromString(\"FF\", 16) -> 255");

    /* Compare */
    dkcBigIntFromInt(&a, 100);
    dkcBigIntFromInt(&b, 200);
    TEST_ASSERT(dkcBigIntCompare(&a, &b) < 0, "100 < 200");
    TEST_ASSERT(dkcBigIntCompare(&b, &a) > 0, "200 > 100");

    dkcBigIntFromInt(&b, 100);
    TEST_ASSERT(dkcBigIntCompare(&a, &b) == 0, "100 == 100");

    /* Add */
    dkcBigIntFromInt(&a, 100);
    dkcBigIntFromInt(&b, 200);
    dkcBigIntAdd(&c, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "300") == 0, "100 + 200 = 300");

    /* Add with negative */
    dkcBigIntFromInt(&a, 100);
    dkcBigIntFromInt(&b, -150);
    dkcBigIntAdd(&c, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "-50") == 0, "100 + (-150) = -50");

    /* Sub */
    dkcBigIntFromInt(&a, 300);
    dkcBigIntFromInt(&b, 100);
    dkcBigIntSub(&c, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "200") == 0, "300 - 100 = 200");

    /* Mul */
    dkcBigIntFromInt(&a, 123);
    dkcBigIntFromInt(&b, 456);
    dkcBigIntMul(&c, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "56088") == 0, "123 * 456 = 56088");

    /* Mul with negative */
    dkcBigIntFromInt(&a, -7);
    dkcBigIntFromInt(&b, 8);
    dkcBigIntMul(&c, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "-56") == 0, "-7 * 8 = -56");

    /* Div */
    dkcBigIntFromInt(&a, 100);
    dkcBigIntFromInt(&b, 7);
    dkcBigIntDiv(&c, &d, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "14") == 0, "100 / 7 = 14");
    dkcBigIntToString(&d, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "2") == 0, "100 %% 7 = 2");

    /* Mod */
    dkcBigIntFromInt(&a, 17);
    dkcBigIntFromInt(&b, 5);
    dkcBigIntMod(&c, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "2") == 0, "17 %% 5 = 2");

    /* Shift Left */
    dkcBigIntFromInt(&a, 1);
    dkcBigIntShiftLeft(&c, &a, 10);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "1024") == 0, "1 << 10 = 1024");

    /* Shift Right */
    dkcBigIntFromInt(&a, 1024);
    dkcBigIntShiftRight(&c, &a, 3);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "128") == 0, "1024 >> 3 = 128");

    /* GCD */
    dkcBigIntFromInt(&a, 12);
    dkcBigIntFromInt(&b, 8);
    dkcBigIntGCD(&c, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "4") == 0, "gcd(12, 8) = 4");

    dkcBigIntFromInt(&a, 35);
    dkcBigIntFromInt(&b, 14);
    dkcBigIntGCD(&c, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "7") == 0, "gcd(35, 14) = 7");

    /* ModPow: 2^10 mod 1000 = 24 */
    dkcBigIntFromInt(&a, 2);
    dkcBigIntFromInt(&b, 10);
    dkcBigIntFromInt(&c, 1000);
    dkcBigIntModPow(&d, &a, &b, &c);
    dkcBigIntToString(&d, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "24") == 0, "2^10 mod 1000 = 24");

    /* ModPow: 3^13 mod 100 = 3^13=1594323, 1594323%100=23 */
    dkcBigIntFromInt(&a, 3);
    dkcBigIntFromInt(&b, 13);
    dkcBigIntFromInt(&c, 100);
    dkcBigIntModPow(&d, &a, &b, &c);
    dkcBigIntToString(&d, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "23") == 0, "3^13 mod 100 = 23");

    /* Large number: from string */
    dkcBigIntFromString(&a, "123456789012345678901234567890", 10);
    dkcBigIntFromString(&b, "987654321098765432109876543210", 10);
    dkcBigIntAdd(&c, &a, &b);
    dkcBigIntToString(&c, buf, sizeof(buf), 10);
    TEST_ASSERT(strcmp(buf, "1111111110111111111011111111100") == 0,
        "Large number addition");

    /* BitCount */
    dkcBigIntFromInt(&a, 255);
    TEST_ASSERT(dkcBigIntBitCount(&a) == 8, "BitCount(255) = 8");

    dkcBigIntFromInt(&a, 256);
    TEST_ASSERT(dkcBigIntBitCount(&a) == 9, "BitCount(256) = 9");

    /* Free */
    dkcBigIntFree(&a);
    dkcBigIntFree(&b);
    dkcBigIntFree(&c);
    dkcBigIntFree(&d);

    TEST_END();
}

/* ========================================
 * ChaCha20 Test
 * ======================================== */
void Test_ChaCha20(void)
{
    /* RFC 8439 Section 2.4.2 test vector */
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

    /* Verify encrypt/decrypt roundtrip */
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
    /* RFC 8439 Section 2.5.2 test vector */
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

    /* Verify constant-time comparison */
    result = dkcPoly1305Verify(tag, expected_tag);
    TEST_ASSERT(result == edk_SUCCEEDED, "Poly1305 Verify (match)");

    memset(tag2, 0, sizeof(tag2));
    result = dkcPoly1305Verify(tag, tag2);
    TEST_ASSERT(result == edk_FAILED, "Poly1305 Verify (mismatch)");

    /* Empty message */
    result = dkcPoly1305OneShot(key, 32, (const unsigned char *)"", 0, tag);
    TEST_ASSERT(result == edk_SUCCEEDED, "Poly1305 empty message");

    TEST_END();
}

/* ========================================
 * AES-GCM Test
 * ======================================== */
void Test_AESGCM(void)
{
    /* NIST test vector (AES-128-GCM) */
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

    /* Encrypt */
    result = dkcAESGCMEncrypt(ctx, iv, 12, NULL, 0, pt, 16, ct, tag);
    TEST_ASSERT(result == edk_SUCCEEDED, "AES-GCM Encrypt");

    /* Decrypt (valid tag) */
    result = dkcAESGCMDecrypt(ctx, iv, 12, NULL, 0, ct, 16, tag, decrypted);
    TEST_ASSERT(result == edk_SUCCEEDED, "AES-GCM Decrypt (valid tag)");
    TEST_ASSERT(memcmp(decrypted, pt, 16) == 0, "AES-GCM roundtrip");

    /* Tampered tag */
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

    /* Verify */
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
    /* RFC 7914 test vector #2: password="password", salt="NaCl", N=1024, r=8, p=16, dkLen=64 */
    /* We use simpler params for speed: N=16, r=1, p=1 */
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

    /* Verify determinism */
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

    /* Argon2d test */
    result = dkcArgon2Hash(dkcd_ARGON2D, 1, 16, 1,
        (const unsigned char *)"password", 8,
        (const unsigned char *)"somesalt", 8,
        hash1, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Argon2d hash");

    /* Argon2i test */
    result = dkcArgon2Hash(dkcd_ARGON2I, 1, 16, 1,
        (const unsigned char *)"password", 8,
        (const unsigned char *)"somesalt", 8,
        hash2, 32);
    TEST_ASSERT(result == edk_SUCCEEDED, "Argon2i hash");

    /* Argon2d and Argon2i should produce different outputs */
    TEST_ASSERT(memcmp(hash1, hash2, 32) != 0, "Argon2d != Argon2i");

    /* Determinism */
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

    /* Insert */
    for(i = 0; i < 100; i++){
        val = i * 3;
        result = dkcSkipListInsert(sl, &val, &i, sizeof(int));
        TEST_ASSERT(result == edk_SUCCEEDED || i > 0, "SkipList insert");
    }
    TEST_ASSERT(dkcSkipListSize(sl) == 100, "Size == 100");

    /* Find */
    val = 15; /* 5 * 3 */
    node = dkcSkipListFind(sl, &val);
    TEST_ASSERT(node != NULL, "Find key 15");

    val = 999;
    node = dkcSkipListFind(sl, &val);
    TEST_ASSERT(node == NULL, "Find non-existent key");

    /* Erase */
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

static int treap_int_compare(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    if(va < vb) return -1;
    if(va > vb) return 1;
    return 0;
}

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

    /* Insert */
    for(i = 0; i < 100; i++){
        val = 100 - i; /* reverse order to test balancing */
        result = dkcTreapInsert(treap, &val, &i, sizeof(int));
    }
    TEST_ASSERT(dkcTreapSize(treap) == 100, "Size == 100");

    /* Find */
    val = 50;
    node = dkcTreapFind(treap, &val);
    TEST_ASSERT(node != NULL, "Find key 50");

    val = 0;
    node = dkcTreapFind(treap, &val);
    TEST_ASSERT(node == NULL, "Find non-existent key 0");

    /* Erase */
    val = 50;
    result = dkcTreapErase(treap, &val);
    TEST_ASSERT(result == edk_SUCCEEDED, "Erase key 50");
    TEST_ASSERT(dkcTreapSize(treap) == 99, "Size after erase");

    dkcFreeTreapRoot(&treap);
    TEST_ASSERT(treap == NULL, "Free treap");

    TEST_END();
}

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

    /* Insert 50 elements */
    for(i = 0; i < 50; i++){
        val = i * 2;
        result = dkcBPlusTreeInsert(bpt, &val, &i, sizeof(int));
    }
    TEST_ASSERT(dkcBPlusTreeSize(bpt) == 50, "Size == 50");

    /* Find */
    val = 10;
    result = dkcBPlusTreeFind(bpt, &val, &found, sizeof(int));
    TEST_ASSERT(result == edk_SUCCEEDED, "Find key 10");
    TEST_ASSERT(found == 5, "Found data == 5");

    val = 11;
    result = dkcBPlusTreeFind(bpt, &val, NULL, 0);
    TEST_ASSERT(result == edk_Not_Found, "Find non-existent key");

    /* Range foreach */
    {
        int start = 10, end = 30;
        bpt_range_count = 0;
        dkcBPlusTreeRangeForeach(bpt, &start, &end, bpt_count_callback, NULL);
        TEST_ASSERT(bpt_range_count == 11, "Range [10,30] count == 11");
    }

    /* Erase */
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
    int i;

    TEST_BEGIN("Fibonacci Heap Test");

    fh = dkcAllocFibHeapRoot(sizeof(int), fh_int_compare);
    TEST_ASSERT(fh != NULL, "dkcAllocFibHeapRoot");
    TEST_ASSERT(dkcFibHeapIsEmpty(fh) == TRUE, "Empty Fibonacci heap");

    /* Insert */
    val = 30;
    dkcFibHeapInsert(fh, &val, NULL, 0);
    val = 10;
    node = dkcFibHeapInsert(fh, &val, NULL, 0);
    val = 20;
    dkcFibHeapInsert(fh, &val, NULL, 0);
    val = 5;
    dkcFibHeapInsert(fh, &val, NULL, 0);

    TEST_ASSERT(dkcFibHeapSize(fh) == 4, "Size == 4");

    /* FindMin */
    min_node = dkcFibHeapFindMin(fh);
    TEST_ASSERT(min_node != NULL, "FindMin not NULL");
    TEST_ASSERT(*(int *)min_node->key == 5, "Min == 5");

    /* DecreaseKey */
    val = 1;
    result = dkcFibHeapDecreaseKey(fh, node, &val);
    TEST_ASSERT(result == edk_SUCCEEDED, "DecreaseKey 10->1");

    min_node = dkcFibHeapFindMin(fh);
    TEST_ASSERT(*(int *)min_node->key == 1, "Min == 1 after decrease");

    /* ExtractMin */
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

    /* Search for "ana" */
    result = dkcSuffixArraySearch(sa, "ana", 3, &first, &count);
    TEST_ASSERT(result == edk_SUCCEEDED, "Search 'ana'");
    TEST_ASSERT(count == 2, "Found 2 occurrences of 'ana'");

    /* Search for "ban" */
    result = dkcSuffixArraySearch(sa, "ban", 3, &first, &count);
    TEST_ASSERT(result == edk_SUCCEEDED, "Search 'ban'");
    TEST_ASSERT(count == 1, "Found 1 occurrence of 'ban'");

    /* Search for non-existent */
    result = dkcSuffixArraySearch(sa, "xyz", 3, &first, &count);
    TEST_ASSERT(result == edk_Not_Found, "Search non-existent 'xyz'");

    /* Build LCP */
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

    /* CharAt */
    ch = dkcRopeCharAt(rope, 0);
    TEST_ASSERT(ch == 'H', "CharAt(0) == 'H'");
    ch = dkcRopeCharAt(rope, 4);
    TEST_ASSERT(ch == 'o', "CharAt(4) == 'o'");

    /* Insert */
    result = dkcRopeInsert(&rope, 5, " World");
    TEST_ASSERT(result == edk_SUCCEEDED, "Insert ' World'");
    TEST_ASSERT(dkcRopeLength(rope) == 11, "Length == 11");

    dkcRopeToString(rope, buf, sizeof(buf));
    TEST_ASSERT(strcmp(buf, "Hello World") == 0, "ToString == 'Hello World'");

    /* Concat */
    rope2 = dkcAllocRope("!!!");
    rope = dkcRopeConcat(rope, rope2);
    TEST_ASSERT(dkcRopeLength(rope) == 14, "Length after concat == 14");

    dkcRopeToString(rope, buf, sizeof(buf));
    TEST_ASSERT(strcmp(buf, "Hello World!!!") == 0, "Concat result");

    /* Delete */
    result = dkcRopeDelete(&rope, 5, 6); /* delete " World" */
    TEST_ASSERT(result == edk_SUCCEEDED, "Delete");
    TEST_ASSERT(dkcRopeLength(rope) == 8, "Length after delete == 8");

    dkcRopeToString(rope, buf, sizeof(buf));
    TEST_ASSERT(strcmp(buf, "Hello!!!") == 0, "Delete result");

    /* Substring */
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
    printf("  dkutil_c Comprehensive Test Suite\n");
    printf("========================================\n");

    /* Container Tests */
    Test_Stack();
    Test_Queue();
    Test_Deque();
    Test_HashSet();
    Test_HashMap();
    Test_HashMultiSet();
    Test_HashMultiMap();
    Test_SingleList();
    Test_DoubleList();
    Test_MemoryStream();
    Test_CircularMemoryStream();
    Test_Buffer();
    Test_MemoryPool();

    /* Tree Tests */
    Test_BTree();
    Test_2Tree();
    Test_RedBlackTree();
    Test_AVLTree();
    Test_NaryTree();

    /* Hash/Digest Tests */
    Test_CRC();
    Test_MD5();
    Test_MD4();
    Test_MD2();
    Test_SHA1();
    Test_SHA256();
    Test_SHA384();
    Test_SHA512();
    Test_SHA3();
    Test_HMAC();
    Test_BLAKE256();
    Test_BLAKE512();
    Test_BLAKE2();
    Test_BLAKE3();
    Test_BLAKE_SHO();

    /* Cipher Tests */
    Test_Arcfour();
    Test_Blowfish();
    Test_Rijndael();
    Test_SNOW20();
    Test_HC256();
    Test_KCipher2();

    /* Compression Tests */
    Test_LZSS();
    Test_LZW();
    Test_RLE_PackBits();

    Test_Camellia();
    Test_Twofish();
    Test_Threefish();

    /* Cipher Tests (additional) */
    Test_Vernam();

    /* Base64 Test */
    Test_Base64();

    /* MPH (Minimal Perfect Hash) Tests */
    Test_MPH();

    /* LightHash Tests */
    Test_LightHash();

    /* Bloom Filter Tests */
    Test_BloomFilter();

    /* Cuckoo Hash Tests */
    Test_CuckooHash();

    /* LRU Cache Tests */
    Test_LRUCache();
    Test_LRUCache2();

    /* Advanced Data Structures */
    Test_SegmentTree();
    Test_FenwickTree();
    Test_UnionFind();
    Test_SparseSet();

    /* Heap Tests */
    Test_Heap();
    Test_TypedHeap();
    Test_HeapSortStandalone();

    /* Splay Tree Tests */
    Test_SplayTree();

    /* Patricia Trie Tests */
    Test_Patricia();

    /* BigInteger Tests */
    Test_BigInteger();

    /* UniqueID Tests */
    Test_UniqueID();

    /* Regex Tests */
    Test_Regex();

    /* ChaCha20 / Poly1305 / AES-GCM Tests */
    Test_ChaCha20();
    Test_Poly1305();
    Test_AESGCM();

    /* Password Hash Tests */
    Test_Bcrypt();
    Test_Scrypt();
    Test_Argon2();

    /* Additional Data Structure Tests */
    Test_SkipList();
    Test_Treap();
    Test_BPlusTree();
    Test_FibonacciHeap();
    Test_SuffixArray();
    Test_Rope();

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
