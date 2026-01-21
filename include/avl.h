#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define LEFT 1
#define RIGHT 2

typedef struct AVLNode{
    int key;
    struct AVLNode* left;
    struct AVLNode* right;
    struct AVLNode* parent;
    int height;
} AVLNode;

AVLNode* insert_AVLNode_AVL(AVLNode* root, int key);
bool search_AVLNode_AVL(AVLNode *root, int value);
AVLNode* create_AVLNode_AVL(int value);
AVLNode* rebalance_Tree_AVL(AVLNode* root, AVLNode* AVLNode);
AVLNode* rotate_left_AVL(AVLNode* pivot);
AVLNode* rotate_right_AVL(AVLNode* pivot);
bool replace_child(AVLNode* parent, AVLNode* currentChild, AVLNode* newChild);
bool set_child_AVL(AVLNode* parent,int whichChild,AVLNode* child);
int get_balance_AVL(AVLNode* AVLNode);
void update_height_AVL(AVLNode* AVLNode);
void export_AVL_JSON(AVLNode* node, FILE* out);

#endif
