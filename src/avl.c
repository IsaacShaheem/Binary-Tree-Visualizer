#include "avl.h"

void update_height_AVL(AVLNode* AVLNode)
{
    int leftHeight = -1;
    if(AVLNode->left != NULL){
        leftHeight = AVLNode->left->height;
    }
    int rightHeight = -1;
    if(AVLNode->right != NULL){
        rightHeight = AVLNode->right->height;
    }

    int max;
    if(leftHeight > rightHeight){
        max = leftHeight;
    }else{
        max = rightHeight;
    }

    AVLNode->height = max + 1;

}

int get_balance_AVL(AVLNode* AVLNode)
{

    int leftHeight = -1;
    if(AVLNode->left != NULL){
        leftHeight = AVLNode->left->height;
    }
    int rightHeight = -1;
    if(AVLNode->right != NULL){
        rightHeight = AVLNode->right->height;
    }

    return leftHeight - rightHeight;

}

AVLNode* rotate_right_AVL(AVLNode* pivot)
{
    AVLNode* parent = pivot->parent;
    AVLNode* pivotLeftChild = pivot->left;
    AVLNode* pivotLeftRightChild = pivot->left->right;

    if(parent != NULL){
        if(parent->left == pivot){
            parent->left = pivotLeftChild;
        }else{
            parent->right = pivotLeftChild;
        }
    }

    pivotLeftChild->right = pivot;
    pivot->left = pivotLeftRightChild;

    pivotLeftChild->parent = pivot->parent;
    pivot->parent = pivotLeftChild;

    if(pivotLeftRightChild != NULL){
        pivotLeftRightChild->parent = pivot;
    }

    update_height_AVL(pivot);
    update_height_AVL(pivotLeftChild);

    return pivotLeftChild;
}

AVLNode* rotate_left_AVL(AVLNode* pivot)
{
    AVLNode* parent = pivot->parent;
    AVLNode* pivotRightChild = pivot->right;
    AVLNode* pivotRightLeftChild = pivot->right->left;

    if(parent != NULL){
        if(parent->left == pivot){
            parent->left = pivotRightChild;
        }else{
            parent->right = pivotRightChild;
        }
    }

    pivotRightChild->left = pivot;
    pivot->right = pivotRightLeftChild;

    pivotRightChild->parent = pivot->parent;
    pivot->parent = pivotRightChild;

    if(pivotRightLeftChild != NULL){
        pivotRightLeftChild->parent = pivot;
    }

    update_height_AVL(pivot);
    update_height_AVL(pivotRightChild);

    return pivotRightChild;

}

AVLNode *create_AVLNode_AVL(int value)
{
    AVLNode *n = malloc(sizeof(AVLNode));

    if (!n)
        return NULL;

    n->key = value;
    n->left = NULL;
    n->right = NULL;
    n->height = 0;
    n->parent = NULL;
    return n;
}

bool search_AVLNode_AVL(AVLNode *root, int value){

    AVLNode *currentAVLNode = root;

    while (currentAVLNode != NULL)
    {
        if (currentAVLNode->key == value)
        {
            return true;
        }
        else if (value < currentAVLNode->key)
        {
            currentAVLNode = currentAVLNode->left;
        }
        else
        {
            currentAVLNode = currentAVLNode->right;
        }
    }

    if (currentAVLNode == NULL)
    {
        return false;
    }

    return false;
}

AVLNode* insert_AVLNode_AVL(AVLNode* node, int key){

    if(node == NULL){
        return create_AVLNode_AVL(key);
    }

    if(key < node->key){
        node->left = insert_AVLNode_AVL(node->left,key);
        node->left->parent = node;
    }else if(key > node->key){
        node->right = insert_AVLNode_AVL(node->right,key);
        node->right->parent = node;

    }else{
        return node;
    }

    update_height_AVL(node);

    int balance = get_balance_AVL(node);

    if(balance > 1 && key < node->left->key){
        return rotate_right_AVL(node);
    }

    if(balance < -1 && key > node->right->key){
        return rotate_left_AVL(node);
    }

    if(balance > 1 && key > node->left->key){
        node->left = rotate_left_AVL(node->left);
        node->left->parent = node;
        return rotate_right_AVL(node);
    }

    if(balance < -1 && key < node->right->key){
        node->right = rotate_right_AVL(node->right);
        node->right->parent = node;
        return rotate_left_AVL(node);
    }

    return node;

}

void export_AVL_JSON(AVLNode* node, FILE* out){

    if(node == NULL){
        fprintf(out,"null");
        return;
    }

    int leftHeight, rightHeight, balance;

    if(node->left != NULL){
        leftHeight = node->left->height;
    }else{
        leftHeight = 0;
    }

    if(node->right != NULL){
        rightHeight = node->right->height;
    }else{
        rightHeight = 0;
    }
    
    balance = leftHeight - rightHeight;

    fprintf(out,"{");

    fprintf(out,"\"key\": %d,",node->key);

    fprintf(out,"\"left\": ");
    export_AVL_JSON(node->left,out);

    fprintf(out,",\"right\": ");
    export_AVL_JSON(node->right,out);

    fprintf(out,",\"height\": %d",node->height);

    fprintf(out,",\"balance\": %d",balance);

    fprintf(out,"}");

}
