#ifndef BST_H
#define BST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct BSTNode{
    int key;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

BSTNode* create_BSTNode_BST(int key);
BSTNode *insert_BSTNode_BST(BSTNode *root, int value);
bool search_BSTNode_BST(BSTNode *root, int value);
bool delete_BSTNode_BST(BSTNode *root, int value);
bool delete_Leaf_BSTNode_BST(BSTNode* root, BSTNode* parent, BSTNode* matchedBSTNode);
bool delete_BSTNode_One_Child_BST(BSTNode* root, BSTNode* parent, BSTNode* matchedBSTNode, BSTNode* child);
bool delete_BSTNode_Two_Child_BST(BSTNode* root, BSTNode* parent, BSTNode* matchedBSTNode);
void exportBSTJSON(BSTNode* root, FILE* out);

#endif
