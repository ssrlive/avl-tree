#ifndef __BST_TREE_H__
#define __BST_TREE_H__ 1

struct bst_tree;
struct bst_node;

int bst_node_is_nil(struct bst_node*node);

const void* bst_node_get_key(const struct bst_node* node);
int bst_node_balance_factor(struct bst_node* node);

typedef int (*bst_compare)(const void*, const void*);
typedef void (*bst_destroy)(void*);

struct bst_tree* bst_tree_create(bst_compare cmp, bst_destroy destroy);
int bst_insert(struct bst_tree* tree, void* data, size_t size);

const struct bst_node* bst_find_node(const struct bst_tree* tree, const void* key);

typedef void(*node_walk_cb)(struct bst_node* x, void* p);
void bst_inorder_walk(struct bst_tree* tree, node_walk_cb cb, void* p);

int bst_delete_node(struct bst_tree* tree, void* key);
void bst_destroy_tree(struct bst_tree* tree);

#endif /* __BST_TREE_H__ */
