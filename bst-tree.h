#ifndef __BST_TREE_H__
#define __BST_TREE_H__ 1

struct bst_tree;
struct bst_node;

typedef int ElemType;

int bst_node_is_nil(struct bst_node*node);

ElemType bst_node_get_key(struct bst_node*node);
int bst_node_balance_factor(struct bst_node* node);
struct bst_tree* bst_tree_create(void);
int bst_insert(struct bst_tree* tree, ElemType e);

struct bst_node* bst_find_node(struct bst_tree* tree, ElemType k);

typedef void(*node_walk_cb)(struct bst_node* x, void* p);
void bst_inorder_walk(struct bst_tree* tree, node_walk_cb cb, void* p);

int bst_delete_node(struct bst_tree* tree, ElemType key);
void bst_destroy_tree(struct bst_tree* tree);

#endif /* __BST_TREE_H__ */
