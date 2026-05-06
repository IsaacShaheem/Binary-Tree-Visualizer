#include "tree.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void assert_json_contains(char *json, const char *expected)
{
    assert(json != NULL);
    assert(strstr(json, expected) != NULL);
    tree_free_json(json);
}

static void test_bst_search_and_delete(void)
{
    BSTTree *tree = bst_create();
    assert(tree != NULL);

    assert(bst_insert(tree, 8));
    assert(bst_insert(tree, 4));
    assert(bst_insert(tree, 12));
    assert(bst_insert(tree, 10));
    assert(bst_insert(tree, 14));
    assert(!bst_insert(tree, 10));

    assert(bst_search(tree, 10));
    assert(!bst_search(tree, 99));

    assert(bst_delete(tree, 12));
    assert(!bst_search(tree, 12));
    assert(bst_search(tree, 10));
    assert(bst_search(tree, 14));
    assert(!bst_delete(tree, 12));

    assert(bst_delete(tree, 8));
    assert(!bst_search(tree, 8));
    assert_json_contains(bst_export_json(tree), "\"key\":10");

    bst_destroy(tree);
}

static void test_avl_rotation(int a, int b, int c)
{
    AVLTree *tree = avl_create();
    char *json;

    assert(tree != NULL);
    assert(avl_insert(tree, a));
    assert(avl_insert(tree, b));
    assert(avl_insert(tree, c));

    json = avl_export_json(tree);
    assert(json != NULL);
    assert(strncmp(json, "{\"key\":20", 9) == 0);
    assert(strstr(json, "\"balance\":0") != NULL);
    tree_free_json(json);

    avl_destroy(tree);
}

static void test_avl_edge_cases(void)
{
    AVLTree *tree = avl_create();
    assert(tree != NULL);

    assert(avl_insert(tree, -10));
    assert(avl_insert(tree, -20));
    assert(!avl_insert(tree, -10));
    assert(avl_search(tree, -20));
    assert(!avl_search(tree, 5));

    avl_destroy(tree);
}

static void test_avl_delete_basic_cases(void)
{
    AVLTree *tree = avl_create();
    assert(tree != NULL);

    assert(avl_insert(tree, 20));
    assert(avl_insert(tree, 10));
    assert(avl_insert(tree, 30));
    assert(avl_insert(tree, 25));
    assert(avl_insert(tree, 40));

    assert(avl_delete(tree, 25));
    assert(!avl_search(tree, 25));

    assert(avl_delete(tree, 30));
    assert(!avl_search(tree, 30));
    assert(avl_search(tree, 40));

    assert(avl_delete(tree, 20));
    assert(!avl_search(tree, 20));
    assert(!avl_delete(tree, 999));

    avl_destroy(tree);
}

static void test_avl_delete_rebalances(void)
{
    AVLTree *tree = avl_create();
    char *json;
    assert(tree != NULL);

    assert(avl_insert(tree, 30));
    assert(avl_insert(tree, 20));
    assert(avl_insert(tree, 40));
    assert(avl_insert(tree, 10));
    assert(avl_insert(tree, 25));

    assert(avl_delete(tree, 40));
    json = avl_export_json(tree);
    assert(json != NULL);
    assert(strncmp(json, "{\"key\":20", 9) == 0);
    assert(strstr(json, "\"balance\":0") != NULL);
    tree_free_json(json);

    avl_destroy(tree);
}

int main(void)
{
    test_bst_search_and_delete();
    test_avl_rotation(30, 20, 10);
    test_avl_rotation(10, 20, 30);
    test_avl_rotation(30, 10, 20);
    test_avl_rotation(10, 30, 20);
    test_avl_edge_cases();
    test_avl_delete_basic_cases();
    test_avl_delete_rebalances();

    puts("C tree tests passed");
    return 0;
}
