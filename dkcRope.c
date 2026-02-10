/*! @file
	@brief Rope 実装
	@author d.Kingyo
	@note Boehm et al. (1995) に基づく実装。
*/

#include "dkcRope.h"
#include <string.h>

/* ====================================================================
 * 内部関数
 * ==================================================================== */

static DKC_ROPE_NODE *rope_create_leaf(const char *str, size_t len)
{
	DKC_ROPE_NODE *node;

	node = (DKC_ROPE_NODE *)dkcAllocateFill(sizeof(DKC_ROPE_NODE), 0);
	if(node == NULL) return NULL;

	node->str = (char *)dkcAllocateFast(len + 1);
	if(node->str == NULL){
		dkcFree((void **)&node);
		return NULL;
	}
	memcpy(node->str, str, len);
	node->str[len] = '\0';

	node->weight = len;
	node->length = len;
	node->left = NULL;
	node->right = NULL;

	return node;
}

static DKC_ROPE_NODE *rope_create_internal(DKC_ROPE_NODE *left, DKC_ROPE_NODE *right)
{
	DKC_ROPE_NODE *node;

	node = (DKC_ROPE_NODE *)dkcAllocateFill(sizeof(DKC_ROPE_NODE), 0);
	if(node == NULL) return NULL;

	node->str = NULL;
	node->left = left;
	node->right = right;
	node->weight = (left != NULL) ? left->length : 0;
	node->length = node->weight + ((right != NULL) ? right->length : 0);

	return node;
}

static void rope_free_recursive(DKC_ROPE_NODE *node)
{
	if(node == NULL) return;
	rope_free_recursive(node->left);
	rope_free_recursive(node->right);
	if(node->str) dkcFree((void **)&node->str);
	dkcFree((void **)&node);
}

/* Build a rope from string, splitting into leaf chunks */
static DKC_ROPE_NODE *rope_build(const char *str, size_t len)
{
	DKC_ROPE_NODE *left;
	DKC_ROPE_NODE *right;
	size_t mid;

	if(len == 0) return NULL;

	if(len <= dkcd_ROPE_LEAF_MAX){
		return rope_create_leaf(str, len);
	}

	mid = len / 2;
	left = rope_build(str, mid);
	right = rope_build(str + mid, len - mid);

	if(left == NULL && right == NULL) return NULL;
	if(left == NULL) return right;
	if(right == NULL) return left;

	return rope_create_internal(left, right);
}

/* Collect characters into buffer */
static void rope_collect(DKC_ROPE_NODE *node, char *buf, size_t *pos, size_t bufsize)
{
	if(node == NULL) return;

	if(node->str != NULL){
		/* Leaf */
		size_t copylen = node->length;
		if(*pos + copylen > bufsize) copylen = bufsize - *pos;
		memcpy(buf + *pos, node->str, copylen);
		*pos += copylen;
	}else{
		rope_collect(node->left, buf, pos, bufsize);
		rope_collect(node->right, buf, pos, bufsize);
	}
}

/* Split rope at position pos into left (0..pos-1) and right (pos..end) */
static void rope_split(DKC_ROPE_NODE *node, size_t pos,
	DKC_ROPE_NODE **out_left, DKC_ROPE_NODE **out_right)
{
	if(node == NULL){
		*out_left = NULL;
		*out_right = NULL;
		return;
	}

	if(node->str != NULL){
		/* Leaf node */
		if(pos == 0){
			*out_left = NULL;
			*out_right = node;
		}else if(pos >= node->length){
			*out_left = node;
			*out_right = NULL;
		}else{
			*out_left = rope_create_leaf(node->str, pos);
			*out_right = rope_create_leaf(node->str + pos, node->length - pos);
			rope_free_recursive(node);
		}
		return;
	}

	/* Internal node */
	if(pos <= node->weight){
		DKC_ROPE_NODE *left_left;
		DKC_ROPE_NODE *left_right;

		rope_split(node->left, pos, &left_left, &left_right);
		*out_left = left_left;

		if(left_right != NULL && node->right != NULL){
			*out_right = rope_create_internal(left_right, node->right);
		}else if(left_right != NULL){
			*out_right = left_right;
		}else{
			*out_right = node->right;
		}

		/* Free the old internal node (not its children) */
		node->left = NULL;
		node->right = NULL;
		if(node->str) dkcFree((void **)&node->str);
		dkcFree((void **)&node);
	}else{
		DKC_ROPE_NODE *right_left;
		DKC_ROPE_NODE *right_right;

		rope_split(node->right, pos - node->weight, &right_left, &right_right);

		if(node->left != NULL && right_left != NULL){
			*out_left = rope_create_internal(node->left, right_left);
		}else if(node->left != NULL){
			*out_left = node->left;
		}else{
			*out_left = right_left;
		}
		*out_right = right_right;

		node->left = NULL;
		node->right = NULL;
		if(node->str) dkcFree((void **)&node->str);
		dkcFree((void **)&node);
	}
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN DKC_ROPE_NODE * WINAPI dkcAllocRope(const char *str)
{
	size_t len;
	if(str == NULL) return NULL;
	len = strlen(str);
	if(len == 0) return rope_create_leaf("", 0);
	return rope_build(str, len);
}

DKC_EXTERN int WINAPI dkcFreeRope(DKC_ROPE_NODE **ptr)
{
	if(ptr == NULL || *ptr == NULL) return edk_FAILED;
	rope_free_recursive(*ptr);
	*ptr = NULL;
	return edk_SUCCEEDED;
}

DKC_EXTERN DKC_ROPE_NODE * WINAPI dkcRopeConcat(
	DKC_ROPE_NODE *left, DKC_ROPE_NODE *right)
{
	if(left == NULL) return right;
	if(right == NULL) return left;
	return rope_create_internal(left, right);
}

DKC_EXTERN int WINAPI dkcRopeInsert(DKC_ROPE_NODE **ptr,
	size_t pos, const char *str)
{
	DKC_ROPE_NODE *left;
	DKC_ROPE_NODE *right;
	DKC_ROPE_NODE *mid;

	if(ptr == NULL || str == NULL) return edk_FAILED;

	mid = dkcAllocRope(str);
	if(mid == NULL) return edk_FAILED;

	if(*ptr == NULL){
		*ptr = mid;
		return edk_SUCCEEDED;
	}

	rope_split(*ptr, pos, &left, &right);
	*ptr = dkcRopeConcat(dkcRopeConcat(left, mid), right);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcRopeDelete(DKC_ROPE_NODE **ptr,
	size_t pos, size_t len)
{
	DKC_ROPE_NODE *left;
	DKC_ROPE_NODE *mid_right;
	DKC_ROPE_NODE *mid;
	DKC_ROPE_NODE *right;

	if(ptr == NULL || *ptr == NULL) return edk_FAILED;
	if(len == 0) return edk_SUCCEEDED;

	rope_split(*ptr, pos, &left, &mid_right);
	rope_split(mid_right, len, &mid, &right);
	rope_free_recursive(mid);

	*ptr = dkcRopeConcat(left, right);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcRopeCharAt(DKC_ROPE_NODE *ptr, size_t index)
{
	DKC_ROPE_NODE *node;

	if(ptr == NULL) return -1;
	if(index >= ptr->length) return -1;

	node = ptr;
	while(node != NULL){
		if(node->str != NULL){
			/* Leaf */
			if(index < node->length){
				return (unsigned char)node->str[index];
			}
			return -1;
		}

		if(index < node->weight){
			node = node->left;
		}else{
			index -= node->weight;
			node = node->right;
		}
	}

	return -1;
}

DKC_EXTERN int WINAPI dkcRopeToString(DKC_ROPE_NODE *ptr,
	char *buf, size_t bufsize)
{
	size_t pos;

	if(ptr == NULL || buf == NULL || bufsize == 0) return edk_FAILED;

	pos = 0;
	rope_collect(ptr, buf, &pos, bufsize - 1);
	buf[pos] = '\0';

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcRopeSubstring(DKC_ROPE_NODE *ptr,
	size_t pos, size_t len, char *buf, size_t bufsize)
{
	size_t i;
	size_t copy_len;
	int ch;

	if(ptr == NULL || buf == NULL || bufsize == 0) return edk_FAILED;

	copy_len = len;
	if(copy_len > bufsize - 1) copy_len = bufsize - 1;

	for(i = 0; i < copy_len; i++){
		ch = dkcRopeCharAt(ptr, pos + i);
		if(ch < 0) break;
		buf[i] = (char)ch;
	}
	buf[i] = '\0';

	return edk_SUCCEEDED;
}

DKC_EXTERN size_t WINAPI dkcRopeLength(DKC_ROPE_NODE *ptr)
{
	if(ptr == NULL) return 0;
	return ptr->length;
}
