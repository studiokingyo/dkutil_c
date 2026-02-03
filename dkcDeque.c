/*!
@file dkcDeque.c
@brief Double-ended queue (std::deque-like) implementation
@see dkcDeque.h
@note
Implemented as a circular buffer with automatic growth.
When the buffer is full, it is reallocated to double the capacity
and the elements are linearized.
*/
#define DKUTIL_C_DEQUE_C
#include "dkcDeque.h"
#include "dkcStdio.h"


/* ------------------------------------------------------------ */
/* Internal helpers                                             */
/* ------------------------------------------------------------ */

/*! Get byte offset for element at logical index i */
static DKC_INLINE size_t deque_physical_offset(const DKC_DEQUE *ptr, size_t i)
{
	size_t phys = (ptr->mStart + i) % ptr->mCapacity;
	return phys * ptr->mOffsetOf;
}

/*! Grow the buffer to at least new_cap elements, linearizing the ring */
static int deque_grow(DKC_DEQUE *ptr, size_t new_cap)
{
	BYTE *new_buf;
	size_t i;

	new_buf = (BYTE *)dkcAllocateFast(new_cap * ptr->mOffsetOf);
	if(NULL == new_buf) return edk_OutOfMemory;

	/* copy elements in order to new linear buffer */
	for(i = 0; i < ptr->mCount; i++){
		memcpy(
			new_buf + i * ptr->mOffsetOf,
			ptr->mBuffer + deque_physical_offset(ptr, i),
			ptr->mOffsetOf
		);
	}

	free(ptr->mBuffer);
	ptr->mBuffer = new_buf;
	ptr->mStart = 0;
	ptr->mCapacity = new_cap;
	return edk_SUCCEEDED;
}

static DKC_INLINE int deque_ensure_capacity(DKC_DEQUE *ptr)
{
	if(ptr->mCount < ptr->mCapacity) return edk_SUCCEEDED;

	/* double the capacity (minimum 4) */
	{
		size_t new_cap = ptr->mCapacity * 2;
		if(new_cap < 4) new_cap = 4;
		return deque_grow(ptr, new_cap);
	}
}


/* ------------------------------------------------------------ */
/* Public API                                                   */
/* ------------------------------------------------------------ */

DKC_DEQUE* WINAPI dkcAllocDeque(size_t numof__,size_t offsetof__)
{
	DKC_DEQUE *p;
	size_t size;

	if(0 == numof__ || 0 == offsetof__) return NULL;

	size = numof__ * offsetof__;

	p = (DKC_DEQUE *)dkcAllocate(sizeof(DKC_DEQUE));
	if(NULL == p) return NULL;

	p->mBuffer = (BYTE *)dkcAllocate(size);
	if(NULL == p->mBuffer){
		dkcFree((void **)&p);
		return NULL;
	}

	p->mCapacity = numof__;
	p->mOffsetOf = offsetof__;
	p->mStart = 0;
	p->mCount = 0;

	return p;
}

int WINAPI dkcFreeDeque(DKC_DEQUE **ptr)
{
	if(NULL == ptr || NULL == (*ptr)) return edk_ArgumentException;
	if((*ptr)->mBuffer){
		dkcFree((void **)&(*ptr)->mBuffer);
	}
	return dkcFree((void **)ptr);
}

int WINAPI dkcDequePushBack(DKC_DEQUE *ptr,const void *data)
{
	size_t back_offset;
	int result;

	if(NULL == ptr || NULL == data) return edk_FAILED;

	result = deque_ensure_capacity(ptr);
	if(DKUTIL_FAILED(result)) return result;

	back_offset = deque_physical_offset(ptr, ptr->mCount);
	memcpy(ptr->mBuffer + back_offset, data, ptr->mOffsetOf);
	ptr->mCount++;

	return edk_SUCCEEDED;
}

int WINAPI dkcDequePopBack(DKC_DEQUE *ptr,void *data)
{
	size_t back_offset;

	if(NULL == ptr) return edk_FAILED;
	if(0 == ptr->mCount) return edk_FAILED;

	ptr->mCount--;
	if(data != NULL){
		back_offset = deque_physical_offset(ptr, ptr->mCount);
		memcpy(data, ptr->mBuffer + back_offset, ptr->mOffsetOf);
	}

	return edk_SUCCEEDED;
}

int WINAPI dkcDequePushFront(DKC_DEQUE *ptr,const void *data)
{
	size_t front_offset;
	int result;

	if(NULL == ptr || NULL == data) return edk_FAILED;

	result = deque_ensure_capacity(ptr);
	if(DKUTIL_FAILED(result)) return result;

	/* move start backwards (with wrap) */
	if(ptr->mStart == 0){
		ptr->mStart = ptr->mCapacity - 1;
	}else{
		ptr->mStart--;
	}

	front_offset = ptr->mStart * ptr->mOffsetOf;
	memcpy(ptr->mBuffer + front_offset, data, ptr->mOffsetOf);
	ptr->mCount++;

	return edk_SUCCEEDED;
}

int WINAPI dkcDequePopFront(DKC_DEQUE *ptr,void *data)
{
	size_t front_offset;

	if(NULL == ptr) return edk_FAILED;
	if(0 == ptr->mCount) return edk_FAILED;

	if(data != NULL){
		front_offset = ptr->mStart * ptr->mOffsetOf;
		memcpy(data, ptr->mBuffer + front_offset, ptr->mOffsetOf);
	}

	ptr->mStart = (ptr->mStart + 1) % ptr->mCapacity;
	ptr->mCount--;

	return edk_SUCCEEDED;
}

int WINAPI dkcDequeFront(const DKC_DEQUE *ptr,void *data)
{
	size_t front_offset;

	if(NULL == ptr || NULL == data) return edk_FAILED;
	if(0 == ptr->mCount) return edk_FAILED;

	front_offset = ptr->mStart * ptr->mOffsetOf;
	memcpy(data, ptr->mBuffer + front_offset, ptr->mOffsetOf);

	return edk_SUCCEEDED;
}

int WINAPI dkcDequeBack(const DKC_DEQUE *ptr,void *data)
{
	size_t back_offset;

	if(NULL == ptr || NULL == data) return edk_FAILED;
	if(0 == ptr->mCount) return edk_FAILED;

	back_offset = deque_physical_offset(ptr, ptr->mCount - 1);
	memcpy(data, ptr->mBuffer + back_offset, ptr->mOffsetOf);

	return edk_SUCCEEDED;
}

int WINAPI dkcDequeGetPoint(const DKC_DEQUE *ptr,size_t point_of__,
	void *buffer,size_t buffsize)
{
	size_t offset;

	if(NULL == ptr || NULL == buffer) return edk_FAILED;
	if(point_of__ >= ptr->mCount) return edk_ArgumentException;
	if(buffsize < ptr->mOffsetOf) return edk_BufferOverFlow;

	offset = deque_physical_offset(ptr, point_of__);
	memcpy(buffer, ptr->mBuffer + offset, ptr->mOffsetOf);

	return edk_SUCCEEDED;
}

int WINAPI dkcDequeSetPoint(DKC_DEQUE *ptr,size_t point_of__,
	const void *data,size_t datasize)
{
	size_t offset;

	if(NULL == ptr || NULL == data) return edk_FAILED;
	if(point_of__ >= ptr->mCount) return edk_ArgumentException;
	if(datasize < ptr->mOffsetOf) return edk_ArgumentException;

	offset = deque_physical_offset(ptr, point_of__);
	memcpy(ptr->mBuffer + offset, data, ptr->mOffsetOf);

	return edk_SUCCEEDED;
}

void WINAPI dkcDequeClear(DKC_DEQUE *ptr)
{
	if(NULL == ptr) return;
	ptr->mStart = 0;
	ptr->mCount = 0;
}

size_t WINAPI dkcDequeSize(const DKC_DEQUE *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->mCount;
}

BOOL WINAPI dkcDequeIsEmpty(const DKC_DEQUE *ptr)
{
	if(NULL == ptr) return TRUE;
	return (0 == ptr->mCount) ? TRUE : FALSE;
}

size_t WINAPI dkcDequeCapacity(const DKC_DEQUE *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->mCapacity;
}
