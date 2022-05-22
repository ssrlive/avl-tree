#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "bst-tree.h"

struct bst_tree;

/* self-balancing binary search tree */
struct bst_node {
    ElemType key;
    struct bst_node *left;
    struct bst_node *right;
    struct bst_node* parent;
    struct bst_tree *tree;
};

struct bst_tree {
    struct bst_node *root;
    struct bst_node nil_guard;
};

#define bst_nil_guard(tree) &(tree)->nil_guard

int bst_node_is_nil(struct bst_node* node) {
    if (node == NULL) {
        assert(0);
        return 1;
    }
    return ((node) == bst_nil_guard(((node)->tree)));
}

ElemType bst_node_get_key(struct bst_node* node) {
    return node->key;
}

void _bst_node_init(struct bst_node* node, struct bst_tree* tree) {
    node->left = bst_nil_guard(tree);
    node->right = bst_nil_guard(tree);
    node->parent = bst_nil_guard(tree);
    node->tree = tree;
}

struct bst_tree * bst_tree_create(void){
    struct bst_tree *tree = (struct bst_tree*) calloc(1, sizeof(*tree));
    if (tree) {
        _bst_node_init(&tree->nil_guard, tree);
        tree->root = bst_nil_guard(tree);
    }
    return tree;
}

bool bst_tree_is_empty(struct bst_tree* tree) {
    assert(tree);
    return tree->root == bst_nil_guard(tree);
}

struct bst_node* bst_node_create(struct bst_tree* tree, ElemType e) {
    struct bst_node* node = (struct bst_node*)calloc(1, sizeof(*node));
    if (node) {
        _bst_node_init(node, tree);
        node->key = e;
    }
    return node;
}

int _node_height(struct bst_node* node) {
    int h_left, h_right;

    if (bst_node_is_nil(node)) {
        return 0;
    }
    h_left = bst_node_is_nil(node->left) ? 0 : _node_height(node->left);
    h_right = bst_node_is_nil(node->right) ? 0 : _node_height(node->right);

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

    return MAX(h_left, h_right) + 1;
}

int bst_node_balance_factor(struct bst_node* node) {
    return _node_height(node->right) - _node_height(node->left);
}

void _inorder_tree_walk(struct bst_node* x, node_walk_cb cb, void* p) {
    struct bst_tree* tree = x->tree;
    if (x != bst_nil_guard(tree)) {
        _inorder_tree_walk(x->left, cb, p);
        if (cb) {
            cb(x, p);
        }
        _inorder_tree_walk(x->right, cb, p);
    }
}

void bst_inorder_walk(struct bst_tree* tree, node_walk_cb cb, void* p) {
    _inorder_tree_walk(tree->root, cb, p);
}

struct bst_node* _tree_search(struct bst_node*x, ElemType k) {
    struct bst_tree* tree = x->tree;
    if (x == bst_nil_guard(tree) || k == x->key) {
        return x;
    }
    if (k < x->key) {
        return _tree_search(x->left, k);
    }
    else {
        return _tree_search(x->right, k);
    }
}

struct bst_node* tree_search(struct bst_tree* tree, int k) {
    return _tree_search(tree->root, k);
}

struct bst_node* _iterative_tree_search(struct bst_node* x, ElemType k) {
    struct bst_tree* tree = x->tree;
    while (x != bst_nil_guard(tree) && k != x->key) {
        if (k < x->key) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    return x;
}

struct bst_node* bst_find_node(struct bst_tree* tree, ElemType k) {
    return _iterative_tree_search(tree->root, k);
}

struct bst_node* _tree_minimum(struct bst_node* x) {
    struct bst_tree* tree = x->tree;
    while (x->left != bst_nil_guard(tree)) {
        x = x->left;
    }
    return x;
}

/*
struct bst_node* tree_minimum(struct bst_tree* tree) {
    return _tree_minimum(tree->root);
}
*/

struct bst_node* _tree_maximum(struct bst_node* x) {
    struct bst_tree* tree = x->tree;
    while (x->right != bst_nil_guard(tree)) {
        x = x->right;
    }
    return x;
}

/*
struct bst_node* tree_maximum(struct bst_tree* tree) {
    return _tree_maximum(tree->root);
}
*/

struct bst_node* tree_successor(struct bst_node* x) {
    struct bst_tree* tree = x->tree;
    struct bst_node* y;
    if (x->right != bst_nil_guard(tree)) {
        return _tree_minimum(x->right);
    }
    y = x->parent;
    while (y != bst_nil_guard(tree) && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

void left_rotate(struct bst_tree* tree, struct bst_node* y);
void right_rotate(struct bst_tree* tree, struct bst_node* y);

void _do_general_rebalance(struct bst_node* unbalanced, struct bst_node* child) {
    int balance_factor = bst_node_balance_factor(unbalanced);
    int child_bf = bst_node_balance_factor(child);
    struct bst_tree* tree = unbalanced->tree;
    assert(tree == child->tree);
    assert(unbalanced == child->parent);
    if ((balance_factor == -2) && (child_bf == -1)) {
        /* case left - left */
        right_rotate(tree, unbalanced);
    }
    else if ((balance_factor == 2) && (child_bf == 1)) {
        /* case right - right */
        left_rotate(tree, unbalanced);
    }
    else if ((balance_factor == 2) && (child_bf == -1)) {
        /* case right - left */
        right_rotate(tree, child);
        left_rotate(tree, unbalanced);
    }
    else if ((balance_factor == -2) && (child_bf == 1)) {
        /* case left - right */
        left_rotate(tree, child);
        right_rotate(tree, unbalanced);
    }
    else {
        assert(0);
    }
}

void _tree_insert_rebalance(struct bst_tree* tree, struct bst_node*node) {
    int index = 0;
    struct bst_node* unbalanced = node;
    for (;;) {
        int balance_factor;
        struct bst_node* child = unbalanced;
        unbalanced = unbalanced->parent;
        /* printf("==== insert backtracking steps (%d) ====\n", ++index); */
        if (bst_node_is_nil(unbalanced)) {
            break;
        }
        balance_factor = bst_node_balance_factor(unbalanced);
        if (abs(balance_factor) == 2) {
            _do_general_rebalance(unbalanced, child);
            break;
        }
    }
    (void)tree;
    (void)index;
}

void _tree_insert(struct bst_tree* tree, struct bst_node* z) {
    struct bst_node* y = bst_nil_guard(tree);
    struct bst_node* x = tree->root;
    while (x != bst_nil_guard(tree)) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == bst_nil_guard(tree)) {
        tree->root = z; /* tree was empty */
    }
    else if (z->key < y->key) {
        y->left = z;
    }
    else {
        y->right = z;
    }

    _tree_insert_rebalance(tree, z);
}

int bst_insert(struct bst_tree* tree, ElemType e) {
    struct bst_node* node = bst_node_create(tree, e);
    _tree_insert(tree, node);
    return 0;
}

void transplant(struct bst_tree* tree, struct bst_node* u, struct bst_node* v) {
    if (u->parent == bst_nil_guard(tree)) {
        tree->root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    if (v != bst_nil_guard(tree)) {
        v->parent = u->parent;
    }
}

void _tree_delete_rebalance(struct bst_tree* tree, struct bst_node* node) {
    int index = 0;
    struct bst_node* unbalanced = node;
    for (;;) {
        int balance_factor;
        if (bst_node_is_nil(unbalanced)) {
            break;
        }
        /* printf("==== delete backtracking steps (%d) ====\n", ++index); */
        balance_factor = bst_node_balance_factor(unbalanced);
        if (abs(balance_factor) == 2) {
            struct bst_node* left = unbalanced->left;
            int left_bf = bst_node_balance_factor(left);
            struct bst_node* right = unbalanced->right;
            int right_bf = bst_node_balance_factor(right);
            if (left_bf != 0 || right_bf != 0) {
                if (abs(left_bf) == 1) {
                    _do_general_rebalance(unbalanced, left);
                }
                else if (abs(right_bf) == 1) {
                    _do_general_rebalance(unbalanced, right);
                }
            }
            else {
                if (balance_factor == -2) {
                    right_rotate(tree, unbalanced);
                }
                else {
                    left_rotate(tree, unbalanced);
                }
            }
            break;
        }
        unbalanced = unbalanced->parent;
    }
    (void)index;
}


void _tree_delete(struct bst_tree* tree, struct bst_node* z) {
    struct bst_node* y;
    struct bst_node* runner;
    if (z->left == bst_nil_guard(tree)) {
        runner = z->right;
        transplant(tree, z, z->right);
    }
    else if (z->right == bst_nil_guard(tree)) {
        runner = z->left;
        transplant(tree, z, z->left);
    }
    else {
        int bf = bst_node_balance_factor(z);
        if (bf >= 0) {
            y = _tree_minimum(z->right);
            runner = y->parent;
            if (y->parent != z) {
                transplant(tree, y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(tree, z, y);
            y->left = z->left;
            y->left->parent = y;
        }
        else {
            y = _tree_maximum(z->left);
            runner = y->parent;
            if (y->parent != z) {
                transplant(tree, y, y->left);
                y->left = z->left;
                y->left->parent = y;
            }
            transplant(tree, z, y);
            y->right = z->right;
            y->right->parent = y;
        }
    }
    _tree_delete_rebalance(tree, bst_node_is_nil(runner) ? z->parent : runner);
}

int bst_delete_node(struct bst_tree* tree, ElemType key) {
    struct bst_node* x = bst_find_node(tree, key);
    if (x != bst_nil_guard(tree)) {
        _tree_delete(tree, x);
        free(x);
        return 0;
    }
    return -1;
}

void _bst_destroy_node_recursive(struct bst_tree* tree, struct bst_node* node) {
    if (node != bst_nil_guard(tree)) {
        _bst_destroy_node_recursive(tree, node->left);
        _bst_destroy_node_recursive(tree, node->right);
        free(node);
    }
}

void bst_destroy_tree(struct bst_tree* tree) {
    _bst_destroy_node_recursive(tree, tree->root);
    free(tree);
}

void left_rotate(struct bst_tree* tree, struct bst_node* x) {
    struct bst_node* y = x->right; /* set y */

    /* turn y's left subtree into x's right subtree */
    x->right = y->left;

    if (y->left != bst_nil_guard(tree)) {
        y->left->parent = x;
    }
    y->parent = x->parent; /* link x's parent to y */
    if (x->parent == bst_nil_guard(tree)) {
        tree->root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x; /* put x on y's left */
    x->parent = y;
}

void right_rotate(struct bst_tree* tree, struct bst_node* y) {
    struct bst_node* x = y->left;
    y->left = x->right;
    if (x->right != bst_nil_guard(tree)) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == bst_nil_guard(tree)) {
        tree->root = x;
    }
    else if (y == y->parent->right) {
        y->parent->right = x;
    }
    else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

