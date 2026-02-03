/*!
@file dkcDeque.h
@brief Double-ended queue (std::deque-like)
@note
Circular buffer based deque with dynamic resizing.
Supports O(1) push/pop at both front and back,
and O(1) random access by index.
*/
#ifndef DKUTIL_C_DEQUE_H
#define DKUTIL_C_DEQUE_H

#include "dkcOSIndependent.h"

/*!
Double-ended queue structure.
Uses a circular buffer internally.
*/
typedef struct dkc_Deque{
	BYTE *mBuffer;      /*!< ring buffer */
	size_t mCapacity;   /*!< number of elements that fit in buffer */
	size_t mOffsetOf;   /*!< element size in bytes */
	size_t mStart;      /*!< index of front element (element-based) */
	size_t mCount;      /*!< current number of elements */
}DKC_DEQUE;

/*!
@param numof__[in] initial capacity (number of elements)
@param offsetof__[in] element size in bytes
@return DKC_DEQUE pointer (NULL on failure)
*/
DKC_EXTERN DKC_DEQUE* WINAPI dkcAllocDeque(size_t numof__,size_t offsetof__);

/*!
@param ptr[in] pointer to DKC_DEQUE pointer
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcFreeDeque(DKC_DEQUE **ptr);

/*!
Push element to back of deque. Grows automatically if full.
@param ptr[in] deque
@param data[in] pointer to element data
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcDequePushBack(DKC_DEQUE *ptr,const void *data);

/*!
Pop element from back of deque.
@param ptr[in] deque
@param data[out] buffer to receive popped element (may be NULL)
@return edk_SUCCEEDED on success, edk_FAILED if empty
*/
DKC_EXTERN int WINAPI dkcDequePopBack(DKC_DEQUE *ptr,void *data);

/*!
Push element to front of deque. Grows automatically if full.
@param ptr[in] deque
@param data[in] pointer to element data
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcDequePushFront(DKC_DEQUE *ptr,const void *data);

/*!
Pop element from front of deque.
@param ptr[in] deque
@param data[out] buffer to receive popped element (may be NULL)
@return edk_SUCCEEDED on success, edk_FAILED if empty
*/
DKC_EXTERN int WINAPI dkcDequePopFront(DKC_DEQUE *ptr,void *data);

/*!
Get front element without removing it.
@param ptr[in] deque
@param data[out] buffer to receive element
@return edk_SUCCEEDED on success, edk_FAILED if empty
*/
DKC_EXTERN int WINAPI dkcDequeFront(const DKC_DEQUE *ptr,void *data);

/*!
Get back element without removing it.
@param ptr[in] deque
@param data[out] buffer to receive element
@return edk_SUCCEEDED on success, edk_FAILED if empty
*/
DKC_EXTERN int WINAPI dkcDequeBack(const DKC_DEQUE *ptr,void *data);

/*!
Random access read by index (0-based from front).
@param ptr[in] deque
@param point_of__[in] index (0 = front)
@param buffer[out] buffer to receive element
@param buffsize[in] buffer size in bytes
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcDequeGetPoint(const DKC_DEQUE *ptr,size_t point_of__,void *buffer,size_t buffsize);

/*!
Random access write by index (0-based from front).
@param ptr[in] deque
@param point_of__[in] index (0 = front)
@param data[in] pointer to element data
@param datasize[in] data size in bytes
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcDequeSetPoint(DKC_DEQUE *ptr,size_t point_of__,const void *data,size_t datasize);

/*!
Clear all elements. Capacity is unchanged.
*/
DKC_EXTERN void WINAPI dkcDequeClear(DKC_DEQUE *ptr);

/*!
@return current number of elements
*/
DKC_EXTERN size_t WINAPI dkcDequeSize(const DKC_DEQUE *ptr);

/*!
@return TRUE if deque is empty
*/
DKC_EXTERN BOOL WINAPI dkcDequeIsEmpty(const DKC_DEQUE *ptr);

/*!
@return current capacity (number of elements)
*/
DKC_EXTERN size_t WINAPI dkcDequeCapacity(const DKC_DEQUE *ptr);


#if !defined( DKUTIL_C_DEQUE_C ) && defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcDeque.c"
#endif

#endif /*end of include once*/
