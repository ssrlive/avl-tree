#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__ 1

#include <stddef.h>

struct avl_tree;
struct avl_node;

int avl_node_is_nil(struct avl_node* node);

const void* avl_node_get_key(const struct avl_node* node);
int avl_node_balance_factor(struct avl_node* node);

typedef int (*avl_compare)(const void*, const void*);
typedef void (*avl_destroy)(void*);

struct avl_tree* avl_tree_create(avl_compare cmp, avl_destroy destroy);
int avl_tree_is_empty(struct avl_tree* tree);
int avl_insert(struct avl_tree* tree, void* data, size_t size);

const struct avl_node* avl_find_node(const struct avl_tree* tree,
                                     const void* key);

typedef void (*node_walk_cb)(struct avl_node* x, void* p);
void avl_inorder_walk(struct avl_tree* tree, node_walk_cb cb, void* p);

int avl_delete_node(struct avl_tree* tree, const void* key);
void avl_destroy_tree(struct avl_tree* tree);

#endif /* __AVL_TREE_H__ */
