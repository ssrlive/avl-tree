#include "avl-tree.h"
#include <assert.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct avl_tree;

/* self-balancing binary search tree */
struct avl_node {
    void* key;
    struct avl_node* left;
    struct avl_node* right;
    struct avl_node* parent;
    struct avl_tree* tree;
};

struct avl_tree {
    struct avl_node* root;
    struct avl_node nil_guard;
    avl_compare cmp;
    avl_destroy destroy;
};

#define avl_nil_guard(tree) &(tree)->nil_guard

int avl_node_is_nil(struct avl_node* node)
{
    if (node == NULL) {
        assert(0);
        return 1;
    }
    return ((node) == avl_nil_guard(((node)->tree)));
}

const void* avl_node_get_key(const struct avl_node* node)
{
    return node->key;
}

void _avl_node_init(struct avl_node* node, struct avl_tree* tree)
{
    node->left = avl_nil_guard(tree);
    node->right = avl_nil_guard(tree);
    node->parent = avl_nil_guard(tree);
    node->tree = tree;
}

struct avl_tree* avl_tree_create(avl_compare cmp, avl_destroy destroy)
{
    struct avl_tree* tree = (struct avl_tree*)calloc(1, sizeof(*tree));
    if (tree) {
        _avl_node_init(&tree->nil_guard, tree);
        tree->root = avl_nil_guard(tree);
        assert(cmp);
        tree->cmp = cmp;
        tree->destroy = destroy;
    }
    return tree;
}

int avl_tree_is_empty(struct avl_tree* tree)
{
    assert(tree);
    return tree->root == avl_nil_guard(tree);
}

static struct avl_node* avl_node_create(struct avl_tree* tree, void* data,
                                        size_t size)
{
    struct avl_node* node = (struct avl_node*)calloc(1, sizeof(*node));
    if (node) {
        _avl_node_init(node, tree);
        node->key = calloc(size, sizeof(uint8_t));
        assert(node->key);
        memcpy(node->key, data, size);
    }
    return node;
}

int _node_height(struct avl_node* node)
{
    int h_left, h_right;

    if (avl_node_is_nil(node)) {
        return 0;
    }
    h_left = avl_node_is_nil(node->left) ? 0 : _node_height(node->left);
    h_right = avl_node_is_nil(node->right) ? 0 : _node_height(node->right);

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

    return MAX(h_left, h_right) + 1;
}

int avl_node_balance_factor(struct avl_node* node)
{
    return _node_height(node->right) - _node_height(node->left);
}

void _inorder_tree_walk(struct avl_node* x, node_walk_cb cb, void* p)
{
    struct avl_tree* tree = x->tree;
    if (x != avl_nil_guard(tree)) {
        _inorder_tree_walk(x->left, cb, p);
        if (cb) {
            cb(x, p);
        }
        _inorder_tree_walk(x->right, cb, p);
    }
}

void avl_inorder_walk(struct avl_tree* tree, node_walk_cb cb, void* p)
{
    _inorder_tree_walk(tree->root, cb, p);
}

const struct avl_node* _iterative_tree_search(const struct avl_node* x,
                                              const void* key)
{
    const struct avl_tree* tree = x->tree;
    while (x != avl_nil_guard(tree)) {
        int cmp = tree->cmp(key, x->key);
        if (cmp == 0) {
            break;
        }
        x = (cmp < 0) ? x->left : x->right;
    }
    return x;
}

const struct avl_node* avl_find_node(const struct avl_tree* tree,
                                     const void* key)
{
    return _iterative_tree_search(tree->root, key);
}

struct avl_node* _tree_minimum(struct avl_node* x)
{
    struct avl_tree* tree = x->tree;
    while (x->left != avl_nil_guard(tree)) {
        x = x->left;
    }
    return x;
}

/*
struct avl_node* tree_minimum(struct avl_tree* tree) {
    return _tree_minimum(tree->root);
}
*/

struct avl_node* _tree_maximum(struct avl_node* x)
{
    struct avl_tree* tree = x->tree;
    while (x->right != avl_nil_guard(tree)) {
        x = x->right;
    }
    return x;
}

/*
struct avl_node* tree_maximum(struct avl_tree* tree) {
    return _tree_maximum(tree->root);
}
*/

struct avl_node* tree_successor(struct avl_node* x)
{
    struct avl_tree* tree = x->tree;
    struct avl_node* y;
    if (x->right != avl_nil_guard(tree)) {
        return _tree_minimum(x->right);
    }
    y = x->parent;
    while (y != avl_nil_guard(tree) && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

void left_rotate(struct avl_tree* tree, struct avl_node* y);
void right_rotate(struct avl_tree* tree, struct avl_node* y);

void _do_general_rebalance(struct avl_node* unbalanced, struct avl_node* child)
{
    int balance_factor = avl_node_balance_factor(unbalanced);
    int child_bf = avl_node_balance_factor(child);
    struct avl_tree* tree = unbalanced->tree;
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

void _tree_insert_rebalance(struct avl_tree* tree, struct avl_node* node)
{
    int index = 0;
    struct avl_node* unbalanced = node;
    for (;;) {
        int balance_factor;
        struct avl_node* child = unbalanced;
        unbalanced = unbalanced->parent;
        /* printf("==== insert backtracking steps (%d) ====\n", ++index); */
        if (avl_node_is_nil(unbalanced)) {
            break;
        }
        balance_factor = avl_node_balance_factor(unbalanced);
        if (abs(balance_factor) == 2) {
            _do_general_rebalance(unbalanced, child);
            break;
        }
    }
    (void)tree;
    (void)index;
}

void _tree_insert(struct avl_tree* tree, struct avl_node* z)
{
    struct avl_node* y = avl_nil_guard(tree);
    struct avl_node* x = tree->root;
    int cmp;
    while (x != avl_nil_guard(tree)) {
        y = x;
        if ((cmp = tree->cmp(z->key, x->key)) < 0) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == avl_nil_guard(tree)) {
        tree->root = z; /* tree was empty */
    }
    else if ((cmp = tree->cmp(z->key, y->key)) < 0) {
        y->left = z;
    }
    else {
        y->right = z;
    }

    _tree_insert_rebalance(tree, z);
}

int avl_insert(struct avl_tree* tree, void* data, size_t size)
{
    struct avl_node* node = avl_node_create(tree, data, size);
    _tree_insert(tree, node);
    return 0;
}

void transplant(struct avl_tree* tree, struct avl_node* u, struct avl_node* v)
{
    if (u->parent == avl_nil_guard(tree)) {
        tree->root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else if (u == u->parent->right) {
        u->parent->right = v;
    }
    else {
        assert(0);
    }
    if (v != avl_nil_guard(tree)) {
        v->parent = u->parent;
    }
}

void _tree_delete_rebalance(struct avl_tree* tree, struct avl_node* node)
{
    int index = 0;
    struct avl_node* unbalanced = node;
    struct avl_node* child = avl_nil_guard(tree);
    for (;;) {
        int balance_factor;
        if (avl_node_is_nil(unbalanced)) {
            break;
        }
        /* printf("==== delete backtracking steps (%d) ====\n", ++index); */
        balance_factor = avl_node_balance_factor(unbalanced);
        if (abs(balance_factor) == 2) {
            int child_bf = avl_node_balance_factor(child);
            if (child_bf != 0) {
                assert(abs(child_bf) == 1);
                _do_general_rebalance(unbalanced, child);
            }
            else {
                if (balance_factor == -2) {
                    right_rotate(tree, unbalanced);
                }
                else {
                    left_rotate(tree, unbalanced);
                }
            }
        }
        child = unbalanced;
        unbalanced = unbalanced->parent;
    }
    (void)index;
}

void _tree_delete(struct avl_tree* tree, struct avl_node* z)
{
    struct avl_node* y;
    struct avl_node* runner;
    if (z->left == avl_nil_guard(tree)) {
        runner = avl_node_is_nil(z->right) ? z->parent : z->right;
        transplant(tree, z, z->right);
    }
    else if (z->right == avl_nil_guard(tree)) {
        runner = z->left;
        transplant(tree, z, z->left);
    }
    else {
        int bf = avl_node_balance_factor(z);
        if (bf >= 0) {
            y = _tree_minimum(z->right);
            assert(avl_node_is_nil(y->left));
            if (y->parent != z) {
                runner = y->parent;
                transplant(tree, y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            else {
                runner = y;
            }
            transplant(tree, z, y);
            y->left = z->left;
            y->left->parent = y;
        }
        else {
            y = _tree_maximum(z->left);
            assert(avl_node_is_nil(y->right));
            if (y->parent != z) {
                runner = y->parent;
                transplant(tree, y, y->left);
                y->left = z->left;
                y->left->parent = y;
            }
            else {
                runner = y;
            }
            transplant(tree, z, y);
            y->right = z->right;
            y->right->parent = y;
        }
    }
    if (runner != avl_nil_guard(tree)) {
        _tree_delete_rebalance(tree, runner);
    }
}

static void _destroy_node(struct avl_node* node)
{
    struct avl_tree* tree = node->tree;
    if (tree->destroy) {
        tree->destroy(node->key);
    }
    free(node->key);
    free(node);
}

int avl_delete_node(struct avl_tree* tree, const void* key)
{
    struct avl_node* node = (struct avl_node*)avl_find_node(tree, key);
    if (node != avl_nil_guard(tree)) {
        _tree_delete(tree, node);
        _destroy_node(node);
        return 0;
    }
    return -1;
}

void _avl_destroy_node_recursive(struct avl_tree* tree, struct avl_node* node)
{
    if (node != avl_nil_guard(tree)) {
        _avl_destroy_node_recursive(tree, node->left);
        _avl_destroy_node_recursive(tree, node->right);
        _destroy_node((struct avl_node*)node);
    }
}

void avl_destroy_tree(struct avl_tree* tree)
{
    _avl_destroy_node_recursive(tree, tree->root);
    free(tree);
}

void left_rotate(struct avl_tree* tree, struct avl_node* x)
{
    struct avl_node* y = x->right; /* set y */

    /* turn y's left subtree into x's right subtree */
    x->right = y->left;

    if (y->left != avl_nil_guard(tree)) {
        y->left->parent = x;
    }
    y->parent = x->parent; /* link x's parent to y */
    if (x->parent == avl_nil_guard(tree)) {
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

void right_rotate(struct avl_tree* tree, struct avl_node* y)
{
    struct avl_node* x = y->left;
    y->left = x->right;
    if (x->right != avl_nil_guard(tree)) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == avl_nil_guard(tree)) {
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
