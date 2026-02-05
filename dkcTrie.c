/*! @file
	@brief Trie（トライ木）実装
	@author d.Kingyo, threeBrane
	@note
	配列ベースのTrieノード実装

	各ノードはalphabet_size個の子ポインタ配列を持つ。
	メモリ効率よりも検索速度を優先した実装。
*/

#define DKUTIL_C_TRIE_C

#include "dkcTrie.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 内部関数
 * ==================================================================== */

/*!
	@brief ノードを作成
*/
static DKC_TRIE_NODE* trie_create_node(size_t alphabet_size)
{
	DKC_TRIE_NODE *node;

	node = (DKC_TRIE_NODE*)malloc(sizeof(DKC_TRIE_NODE));
	if (!node) return NULL;

	node->children = (DKC_TRIE_NODE**)calloc(alphabet_size, sizeof(DKC_TRIE_NODE*));
	if (!node->children) {
		free(node);
		return NULL;
	}

	node->value = NULL;
	node->alphabet_size = alphabet_size;
	node->is_end = FALSE;
	node->prefix_count = 0;

	return node;
}

/*!
	@brief ノードを再帰的に解放
*/
static void trie_free_node(DKC_TRIE_NODE *node)
{
	size_t i;

	if (!node) return;

	if (node->children) {
		for (i = 0; i < node->alphabet_size; i++) {
			if (node->children[i]) {
				trie_free_node(node->children[i]);
			}
		}
		free(node->children);
	}

	free(node);
}

/*!
	@brief 文字をインデックスに変換
*/
static size_t trie_char_to_index(unsigned char c)
{
	return (size_t)c;
}

/*!
	@brief 接頭辞に対応するノードを取得
*/
static DKC_TRIE_NODE* trie_find_node(const DKC_TRIE *trie, const char *key)
{
	DKC_TRIE_NODE *node;
	const unsigned char *p;
	size_t idx;

	if (!trie || !trie->root || !key) return NULL;

	node = trie->root;
	for (p = (const unsigned char*)key; *p; p++) {
		idx = trie_char_to_index(*p);
		if (idx >= trie->alphabet_size || !node->children[idx]) {
			return NULL;
		}
		node = node->children[idx];
	}

	return node;
}

/* 列挙用コンテキスト */
typedef struct {
	DKC_TRIE_CALLBACK callback;
	void *user_data;
	char *buffer;
	size_t buffer_size;
	size_t count;
	BOOL cancelled;
} TRIE_ENUM_CONTEXT;

/*!
	@brief サブツリーを再帰的に列挙
*/
static void trie_enum_recursive(DKC_TRIE_NODE *node, TRIE_ENUM_CONTEXT *ctx, size_t depth)
{
	size_t i;

	if (!node || ctx->cancelled) return;
	if (depth >= ctx->buffer_size - 1) return;  /* バッファオーバーフロー防止 */

	/* 終端ノードならコールバック */
	if (node->is_end) {
		ctx->buffer[depth] = '\0';
		if (!ctx->callback(ctx->buffer, node->value, ctx->user_data)) {
			ctx->cancelled = TRUE;
			return;
		}
		ctx->count++;
	}

	/* 子ノードを再帰的に処理 */
	for (i = 0; i < node->alphabet_size && !ctx->cancelled; i++) {
		if (node->children[i]) {
			ctx->buffer[depth] = (char)i;
			trie_enum_recursive(node->children[i], ctx, depth + 1);
		}
	}
}

/* ====================================================================
 * 基本関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcTrieCreate(DKC_TRIE *trie, size_t alphabet_size)
{
	if (!trie) return edk_FAILED;

	if (alphabet_size == 0) {
		alphabet_size = dkcd_TRIE_ALPHABET_SIZE;
	}

	memset(trie, 0, sizeof(DKC_TRIE));
	trie->alphabet_size = alphabet_size;

	trie->root = trie_create_node(alphabet_size);
	if (!trie->root) return edk_FAILED;

	trie->node_count = 1;

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcTrieFree(DKC_TRIE *trie)
{
	if (!trie) return;

	if (trie->root) {
		trie_free_node(trie->root);
		trie->root = NULL;
	}

	trie->count = 0;
	trie->node_count = 0;
}

DKC_EXTERN int WINAPI dkcTrieInsert(DKC_TRIE *trie, const char *key, void *value)
{
	DKC_TRIE_NODE *node;
	const unsigned char *p;
	size_t idx;

	if (!trie || !trie->root || !key) return edk_FAILED;

	node = trie->root;

	for (p = (const unsigned char*)key; *p; p++) {
		idx = trie_char_to_index(*p);
		if (idx >= trie->alphabet_size) return edk_FAILED;

		node->prefix_count++;

		if (!node->children[idx]) {
			node->children[idx] = trie_create_node(trie->alphabet_size);
			if (!node->children[idx]) return edk_FAILED;
			trie->node_count++;
		}

		node = node->children[idx];
	}

	/* 新規追加の場合のみカウントアップ */
	if (!node->is_end) {
		node->is_end = TRUE;
		node->prefix_count++;
		trie->count++;
	}

	node->value = value;

	return edk_SUCCEEDED;
}

DKC_EXTERN void* WINAPI dkcTrieSearch(const DKC_TRIE *trie, const char *key)
{
	DKC_TRIE_NODE *node;

	node = trie_find_node(trie, key);
	if (!node || !node->is_end) return NULL;

	return node->value;
}

DKC_EXTERN BOOL WINAPI dkcTrieContains(const DKC_TRIE *trie, const char *key)
{
	DKC_TRIE_NODE *node;

	node = trie_find_node(trie, key);
	return (node && node->is_end);
}

DKC_EXTERN int WINAPI dkcTrieRemove(DKC_TRIE *trie, const char *key)
{
	DKC_TRIE_NODE *node;
	DKC_TRIE_NODE **path;
	size_t *indices;
	size_t len, i;
	const unsigned char *p;

	if (!trie || !trie->root || !key) return edk_FAILED;

	len = strlen(key);
	if (len == 0) return edk_FAILED;

	/* パスを記録 */
	path = (DKC_TRIE_NODE**)malloc(sizeof(DKC_TRIE_NODE*) * (len + 1));
	indices = (size_t*)malloc(sizeof(size_t) * len);
	if (!path || !indices) {
		if (path) free(path);
		if (indices) free(indices);
		return edk_FAILED;
	}

	/* 文字列をたどってパスを記録 */
	node = trie->root;
	path[0] = node;

	for (i = 0, p = (const unsigned char*)key; *p; i++, p++) {
		size_t idx = trie_char_to_index(*p);
		if (idx >= trie->alphabet_size || !node->children[idx]) {
			free(path);
			free(indices);
			return edk_FAILED;
		}
		indices[i] = idx;
		node = node->children[idx];
		path[i + 1] = node;
	}

	/* 終端ノードでない場合は失敗 */
	if (!node->is_end) {
		free(path);
		free(indices);
		return edk_FAILED;
	}

	/* 終端フラグを解除 */
	node->is_end = FALSE;
	node->value = NULL;
	trie->count--;

	/* prefix_countを減らしながら、不要なノードを削除 */
	for (i = len; i > 0; i--) {
		DKC_TRIE_NODE *parent = path[i - 1];
		DKC_TRIE_NODE *child = path[i];
		size_t idx = indices[i - 1];

		parent->prefix_count--;

		/* 子ノードが不要（終端でなく、子もない）なら削除 */
		if (!child->is_end && child->prefix_count == 0) {
			trie_free_node(child);
			parent->children[idx] = NULL;
			trie->node_count--;
		}
	}

	free(path);
	free(indices);

	return edk_SUCCEEDED;
}

/* ====================================================================
 * 接頭辞操作
 * ==================================================================== */

DKC_EXTERN BOOL WINAPI dkcTrieStartsWith(const DKC_TRIE *trie, const char *prefix)
{
	return (trie_find_node(trie, prefix) != NULL);
}

DKC_EXTERN size_t WINAPI dkcTrieCountPrefix(const DKC_TRIE *trie, const char *prefix)
{
	DKC_TRIE_NODE *node;

	if (!prefix || *prefix == '\0') {
		return trie ? trie->count : 0;
	}

	node = trie_find_node(trie, prefix);
	if (!node) return 0;

	return node->prefix_count;
}

DKC_EXTERN size_t WINAPI dkcTrieEnumPrefix(const DKC_TRIE *trie, const char *prefix,
	DKC_TRIE_CALLBACK callback, void *user_data)
{
	DKC_TRIE_NODE *node;
	TRIE_ENUM_CONTEXT ctx;
	char *buffer;
	size_t prefix_len;
	size_t buffer_size = 4096;

	if (!trie || !callback) return 0;

	prefix_len = prefix ? strlen(prefix) : 0;

	/* 接頭辞のノードを検索 */
	if (prefix_len > 0) {
		node = trie_find_node(trie, prefix);
		if (!node) return 0;
	} else {
		node = trie->root;
	}

	/* バッファを確保 */
	buffer = (char*)malloc(buffer_size);
	if (!buffer) return 0;

	/* 接頭辞をバッファにコピー */
	if (prefix_len > 0) {
		memcpy(buffer, prefix, prefix_len);
	}

	/* コンテキストを設定 */
	ctx.callback = callback;
	ctx.user_data = user_data;
	ctx.buffer = buffer;
	ctx.buffer_size = buffer_size;
	ctx.count = 0;
	ctx.cancelled = FALSE;

	/* 再帰的に列挙 */
	trie_enum_recursive(node, &ctx, prefix_len);

	free(buffer);

	return ctx.count;
}

/* ====================================================================
 * 情報取得
 * ==================================================================== */

DKC_EXTERN size_t WINAPI dkcTrieCount(const DKC_TRIE *trie)
{
	if (!trie) return 0;
	return trie->count;
}

DKC_EXTERN BOOL WINAPI dkcTrieIsEmpty(const DKC_TRIE *trie)
{
	if (!trie) return TRUE;
	return trie->count == 0;
}

DKC_EXTERN size_t WINAPI dkcTrieNodeCount(const DKC_TRIE *trie)
{
	if (!trie) return 0;
	return trie->node_count;
}

/* ====================================================================
 * 高度な操作
 * ==================================================================== */

DKC_EXTERN size_t WINAPI dkcTrieLongestCommonPrefix(const DKC_TRIE *trie,
	const char *key, char *result, size_t size)
{
	DKC_TRIE_NODE *node;
	const unsigned char *p;
	size_t len = 0;
	size_t idx;

	if (!trie || !trie->root || !key || !result || size == 0) return 0;

	node = trie->root;

	for (p = (const unsigned char*)key; *p && len < size - 1; p++) {
		idx = trie_char_to_index(*p);
		if (idx >= trie->alphabet_size || !node->children[idx]) {
			break;
		}
		result[len++] = *p;
		node = node->children[idx];
	}

	result[len] = '\0';
	return len;
}

DKC_EXTERN size_t WINAPI dkcTrieForEach(const DKC_TRIE *trie,
	DKC_TRIE_CALLBACK callback, void *user_data)
{
	return dkcTrieEnumPrefix(trie, "", callback, user_data);
}
