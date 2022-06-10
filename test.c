#include "avl-tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

int int_ptr_compare(const void* lhs, const void* rhs) {
    int left = *(int*)lhs;
    int right = *(int*)rhs;

    if (left < right) {
        return -1;
    }
    else if (left == right) {
        return 0;
    }
    else {
        return 1;
    }
}

void node_walk_cb1(struct avl_node* x, void* p) {
    printf("%d ", *((int*)avl_node_get_key(x)));
    (void)p;
}

void node_walk_cb2(struct avl_node* x, void* p) {
    printf("%d ", avl_node_balance_factor(x));
    (void)p;
}

void test_bst2(void);
void test_avl_string(void);
void test_avl_string2(void);

int main() {
    struct avl_tree* tree = avl_tree_create(int_ptr_compare, NULL);
    size_t i;
    int nArr[] = { 1, 23, 45, 34, 98, 9, 4, 35, 23, 5, 10, 15 };
    int v;
    for (i = 0; i < sizeof(nArr) / sizeof(nArr[0]); ++i) {
        avl_insert(tree, &nArr[i], sizeof(int));
    }

    avl_inorder_walk(tree, node_walk_cb1, NULL);
    printf("==== end ====\n");

    avl_inorder_walk(tree, node_walk_cb2, NULL);
    printf("\n\n");

    v = 35;
    avl_delete_node(tree, &v);

    avl_inorder_walk(tree, node_walk_cb1, NULL);
    printf("==== end ====\n");

    avl_inorder_walk(tree, node_walk_cb2, NULL);
    printf("\n\n");

    v = 9;
    avl_delete_node(tree, &v);

    avl_inorder_walk(tree, node_walk_cb1, NULL);
    printf("==== end ====\n");

    avl_inorder_walk(tree, node_walk_cb2, NULL);
    printf("\n\n");

    avl_destroy_tree(tree);

    test_bst2();
    test_avl_string();
    test_avl_string2();

    return 0;
}

void test_bst2(void)
{
    const struct avl_node *node;
    int i;
    struct avl_tree *t = avl_tree_create(int_ptr_compare, NULL);

    srand((unsigned int)time(NULL));

    for (i = 0; i < 5000; i++) {
        int y, x = rand() % 10000;
        if (0 != avl_insert(t, &x, sizeof(x))) {
            continue;
        }
        node = avl_find_node(t, &x);
        y = *(int*)avl_node_get_key(node);
        assert(y == x);
    }

    avl_inorder_walk(t, node_walk_cb2, NULL);
    printf("\n\n");

    for (i = 0; i < 10000; i++) {
        int x = rand() % 10000;
        avl_delete_node(t, &x);
    }

    avl_inorder_walk(t, node_walk_cb2, NULL);
    printf("\n\n");

    avl_destroy_tree(t);
}

int string_ptr_compare(const void* lhs, const void* rhs) {
    char *left = *(char**)lhs;
    char *right = *(char**)rhs;
    return strcmp(left, right);
}

void string_ptr_destroy(void*p) {
    char* _p = *(char**)p;
    free(_p);
}

void node_walk_cb3(struct avl_node* node, void* p) {
    char *str = *(char**)avl_node_get_key(node);
    printf("%s \n", str);
    (void)p;
}

char* strArr[] = {
    "abcd1234",
    "good_idea",
    "just",
    "ding",
};

void test_avl_string(void) {
    const struct avl_node* node;
    size_t i;
    struct avl_tree* t = avl_tree_create(string_ptr_compare, string_ptr_destroy);
    for (i = 0; i < sizeof(strArr)/sizeof(strArr[0]); i++) {
        char* y;
        char* p = strdup(strArr[i]);
        if (0 != avl_insert(t, &p, sizeof(p))) {
            continue;
        }
        node = avl_find_node(t, &strArr[i]);
        y = *(char**)avl_node_get_key(node);
        assert(strcmp(y, strArr[i]) == 0);
    }

    printf("\n==== test_avl_string ====\n");
    avl_inorder_walk(t, node_walk_cb3, NULL);

    avl_destroy_tree(t);
}

int string_ptr_compare2(const void* lhs, const void* rhs) {
    return strcmp((char*)lhs, (char*)rhs);
}

void node_walk_cb4(struct avl_node* node, void* p) {
    char* str = (char*)avl_node_get_key(node);
    printf("%s \n", str);
    (void)p;
}

void test_avl_string2(void) {
    const struct avl_node* node;
    size_t i;
    struct avl_tree* t = avl_tree_create(string_ptr_compare2, NULL);
    for (i = 0; i < sizeof(strArr) / sizeof(strArr[0]); i++) {
        char* y;
        char* p = strArr[i];
        if (0 != avl_insert(t, p, strlen(p)+1)) {
            continue;
        }
        node = avl_find_node(t, strArr[i]);
        y = (char*)avl_node_get_key(node);
        assert(strcmp(y, strArr[i]) == 0);
    }

    printf("\n==== test_avl_string2 ====\n");
    avl_inorder_walk(t, node_walk_cb4, NULL);

    avl_destroy_tree(t);
}
