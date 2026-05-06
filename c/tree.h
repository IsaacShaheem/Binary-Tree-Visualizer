#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

typedef struct BSTTree BSTTree;
typedef struct AVLTree AVLTree;

BSTTree *bst_create(void);
bool bst_insert(BSTTree *tree, int value);
bool bst_search(const BSTTree *tree, int value);
bool bst_delete(BSTTree *tree, int value);
char *bst_export_json(const BSTTree *tree);
void bst_destroy(BSTTree *tree);

AVLTree *avl_create(void);
bool avl_insert(AVLTree *tree, int value);
bool avl_search(const AVLTree *tree, int value);
bool avl_delete(AVLTree *tree, int value);
const char *avl_last_rotation(const AVLTree *tree);
char *avl_export_json(const AVLTree *tree);
void avl_destroy(AVLTree *tree);

void tree_free_json(char *json);

#endif
