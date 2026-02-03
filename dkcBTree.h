/*!
@file dkcBTree.h
@brief B-Tree (multi-way balanced search tree)
@note
B-Tree is a self-balancing tree data structure that maintains sorted data
and allows searches, sequential access, insertions, and deletions in
logarithmic time. Each node can contain multiple keys and children.
The minimum degree (t) is user-configurable:
- Each node has at most 2t-1 keys
- Each non-root node has at least t-1 keys
- Each non-leaf node has at most 2t children
*/
#ifndef DKUTIL_C_BTREE_H
#define DKUTIL_C_BTREE_H

#include "dkcOSIndependent.h"

/*! B-Tree node */
typedef struct dkc_BTreeNode{
	int num_keys;                       /*!< current number of keys */
	int is_leaf;                        /*!< TRUE if leaf node */
	void **keys;                        /*!< key pointer array [max 2t-1] */
	void **data;                        /*!< data pointer array [max 2t-1] */
	size_t *data_sizes;                 /*!< data size array [max 2t-1] */
	struct dkc_BTreeNode **children;    /*!< child pointer array [max 2t] */
}DKC_BTREE_NODE;

/*! B-Tree root */
typedef struct dkc_BTreeRoot{
	int min_degree;                     /*!< minimum degree t */
	size_t key_size;                    /*!< key size in bytes */
	size_t now_num;                     /*!< current total key count */
	size_t max_num;                     /*!< max key count (0=unlimited) */
	DKC_COMPARE_TYPE compare;           /*!< comparison function */
	DKC_BTREE_NODE *root;               /*!< root node */
}DKC_BTREE_ROOT;

/*! B-Tree search result */
typedef struct dkc_BTreeSearchResult{
	DKC_BTREE_NODE *node;               /*!< found node (NULL if not found) */
	int index;                          /*!< index within node */
}DKC_BTREE_SEARCH_RESULT;

/*!
Callback function type for B-Tree foreach traversal.
@param key[in] key pointer
@param data[in] data pointer
@param data_size[in] data size in bytes
@param user[in] user-defined context pointer
@return FALSE to stop traversal
*/
typedef BOOL (WINAPI *DKC_BTREE_FOREACH_CALLBACK)(
	const void *key,void *data,size_t data_size,void *user);

/*!
@param key_size[in] size of key in bytes
@param min_degree[in] minimum degree t (must be >= 2)
@param compare[in] comparison function (qsort style)
@param max_num[in] maximum number of keys (0=unlimited)
@return DKC_BTREE_ROOT pointer (NULL on failure)
@note Call dkcFreeBTreeRoot() to release.
*/
DKC_EXTERN DKC_BTREE_ROOT * WINAPI dkcAllocBTreeRoot(
	size_t key_size,int min_degree,
	DKC_COMPARE_TYPE compare,size_t max_num);

/*!
@param ptr[in] pointer to pointer allocated by dkcAllocBTreeRoot
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcFreeBTreeRoot(DKC_BTREE_ROOT **ptr);

/*!
@param ptr[in] B-Tree root
@param key[in] key pointer
@param data[in] data pointer (NULL if no data)
@param data_size[in] data size in bytes (0 if no data)
@return edk_SUCCEEDED on success, edk_FAILED if duplicate key exists
*/
DKC_EXTERN int WINAPI dkcBTreeInsert(DKC_BTREE_ROOT *ptr,
	const void *key,const void *data,size_t data_size);

/*!
@param ptr[in] B-Tree root
@param key[in] key to delete
@return edk_SUCCEEDED on success, edk_Not_Found if key not found
*/
DKC_EXTERN int WINAPI dkcBTreeErase(DKC_BTREE_ROOT *ptr,const void *key);

/*!
@param ptr[in] B-Tree root
@param key[in] key to search for
@return search result (node=NULL if not found)
*/
DKC_EXTERN DKC_BTREE_SEARCH_RESULT WINAPI dkcBTreeFind(
	DKC_BTREE_ROOT *ptr,const void *key);

/*!
@param ptr[in] B-Tree root
@param key[in] key to search for
@param data[out] output buffer
@param size[in] output buffer size
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcBTreeGetBuffer(DKC_BTREE_ROOT *ptr,
	const void *key,void *data,size_t size);

/*!
@param ptr[in] B-Tree root
@param key[in] key to search for
@param data[in] new data to set
@param size[in] new data size
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcBTreeSetBuffer(DKC_BTREE_ROOT *ptr,
	const void *key,const void *data,size_t size);

/*!
In-order traversal of all keys.
@param ptr[in] B-Tree root
@param callback[in] callback function called for each key
@param user[in] user-defined context pointer
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcBTreeForeach(DKC_BTREE_ROOT *ptr,
	DKC_BTREE_FOREACH_CALLBACK callback,void *user);

/*!
@param ptr[in] B-Tree root
@return pointer to the minimum key (NULL if empty)
*/
DKC_EXTERN void * WINAPI dkcBTreeFindMin(DKC_BTREE_ROOT *ptr);

/*!
@param ptr[in] B-Tree root
@return pointer to the maximum key (NULL if empty)
*/
DKC_EXTERN void * WINAPI dkcBTreeFindMax(DKC_BTREE_ROOT *ptr);

/*!
Find smallest key strictly greater than given key.
@param ptr[in] B-Tree root
@param key[in] reference key
@return pointer to found key (NULL if not found)
*/
DKC_EXTERN void * WINAPI dkcBTreeFindMinimalGreater(
	DKC_BTREE_ROOT *ptr,const void *key);

/*!
Find largest key strictly less than given key.
@param ptr[in] B-Tree root
@param key[in] reference key
@return pointer to found key (NULL if not found)
*/
DKC_EXTERN void * WINAPI dkcBTreeFindMaximumLess(
	DKC_BTREE_ROOT *ptr,const void *key);

/*!
@param ptr[in] B-Tree root
@return current number of keys
*/
DKC_EXTERN size_t WINAPI dkcBTreeSize(DKC_BTREE_ROOT *ptr);

/*!
@param ptr[in] B-Tree root
@return TRUE if tree is empty
*/
DKC_EXTERN BOOL WINAPI dkcBTreeIsEmpty(DKC_BTREE_ROOT *ptr);


#if !defined( DKUTIL_C_BTREE_C ) && defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcBTree.c"
#endif

#endif /*end of include once*/
