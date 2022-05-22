#include "bst-tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

void node_walk_cb1(struct bst_node* x, void* p) {
    printf("%d ", bst_node_get_key(x));
    (void)p;
}

void node_walk_cb2(struct bst_node* x, void* p) {
    printf("%d ", bst_node_balance_factor(x));
    (void)p;
}

void test_bst2(void);

int main() {
    struct bst_tree* tree = bst_tree_create();
    size_t i;
    ElemType nArr[] = { 1, 23, 45, 34, 98, 9, 4, 35, 23, 5, 10, 15 };
    for (i = 0; i < sizeof(nArr) / sizeof(nArr[0]); ++i) {
        bst_insert(tree, nArr[i]);
    }

    bst_inorder_walk(tree, node_walk_cb1, NULL);
    printf("==== end ====\n");

    bst_inorder_walk(tree, node_walk_cb2, NULL);
    printf("\n\n");

    bst_delete_node(tree, 35);

    bst_inorder_walk(tree, node_walk_cb1, NULL);
    printf("==== end ====\n");

    bst_inorder_walk(tree, node_walk_cb2, NULL);
    printf("\n\n");

    bst_delete_node(tree, 9);

    bst_inorder_walk(tree, node_walk_cb1, NULL);
    printf("==== end ====\n");

    bst_inorder_walk(tree, node_walk_cb2, NULL);
    printf("\n\n");

    bst_destroy_tree(tree);

    test_bst2();

    return 0;
}

void test_bst2(void)
{
    struct bst_node *node;
    int i;
    struct bst_tree *t = bst_tree_create();

    srand((unsigned int)time(NULL));

    for (i = 0; i < 5000; i++) {
        int y, x = rand() % 10000;
        if (0 != bst_insert(t, x)) {
            continue;
        }
        node = bst_find_node(t, x);
        y = bst_node_get_key(node);
        assert(y == x);
    }

    bst_inorder_walk(t, node_walk_cb2, NULL);
    printf("\n\n");

    for (i = 0; i < 10000; i++) {
        int x = rand() % 10000;
        bst_delete_node(t, x);
    }

    bst_inorder_walk(t, node_walk_cb2, NULL);
    printf("\n\n");

    bst_destroy_tree(t);
}
