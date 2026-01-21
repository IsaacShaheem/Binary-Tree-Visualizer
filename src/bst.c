#include "bst.h"

BSTNode *create_BSTNode_BST(int value)
{

    BSTNode *n = malloc(sizeof(BSTNode));

    if (!n)
        return NULL;

    n->key = value;
    n->left = NULL;
    n->right = NULL;

    return n;
}

BSTNode *insert_BSTNode_BST(BSTNode *root, int value)
{

    BSTNode *newBSTNode = create_BSTNode_BST(value);
    BSTNode *currentBSTNode;

    if (root == NULL)
    {
        root = newBSTNode;
        return root;
    }

    else
    {

        currentBSTNode = root;

        while (currentBSTNode != NULL)
        {

            if (newBSTNode->key < currentBSTNode->key)
            {

                if (currentBSTNode->left == NULL)
                {
                    currentBSTNode->left = newBSTNode;
                    currentBSTNode = NULL;
                }
                else
                {
                    currentBSTNode = currentBSTNode->left;
                }
            }
            else
            {

                if (currentBSTNode->right == NULL)
                {
                    currentBSTNode->right = newBSTNode;
                    currentBSTNode = NULL;
                }
                else
                {
                    currentBSTNode = currentBSTNode->right;
                }
            }
        }
    }

    return root;
}

bool search_BSTNode_BST(BSTNode *root, int value)
{

    BSTNode *currentBSTNode = NULL;

    while (currentBSTNode != NULL)
    {
        if (currentBSTNode->key == value)
        {
            return true;
        }
        else if (value < currentBSTNode->key)
        {
            currentBSTNode = currentBSTNode->left;
        }
        else
        {
            currentBSTNode = currentBSTNode->right;
        }
    }

    if (currentBSTNode == NULL)
    {
        return false;
    }

    return true;
}

bool delete_BSTNode_BST(BSTNode *root, int value)
{

    BSTNode *parent = NULL;
    BSTNode *currenBSTNode = root;
    bool success;

    while (currenBSTNode != NULL)
    {

        if (currenBSTNode->key == value)
        {
            if (currenBSTNode->left == NULL && currenBSTNode->right == NULL)
            {
                success = delete_Leaf_BSTNode_BST(root,parent,currenBSTNode);
                return success;
            }
            else if (currenBSTNode->right == NULL)
            {
                success = delete_BSTNode_One_Child_BST(root,parent,currenBSTNode,currenBSTNode->left);
                return success;
            }
            else if (currenBSTNode->left == NULL)
            {
                success = delete_BSTNode_One_Child_BST(root,parent,currenBSTNode,currenBSTNode->right);
                return success;
            }
            else
            {
                success = delete_BSTNode_Two_Child_BST(root,parent,currenBSTNode);
                return success;
            }
        }
        else if (currenBSTNode->key < value)
        {
            parent = currenBSTNode;
            currenBSTNode = currenBSTNode->left;
        }
        else
        {
            parent = currenBSTNode;
            currenBSTNode = currenBSTNode->right;
        }
    }
    return true ;
}

bool delete_Leaf_BSTNode_BST(BSTNode* root, BSTNode* parent, BSTNode* matchedBSTNode)
{

    if(parent == NULL){
        root = NULL;
    }else if(parent->left == matchedBSTNode){
        parent->left = NULL;
    }else{
        parent->right = NULL;
    }

    return true;
}

bool delete_BSTNode_One_Child_BST(BSTNode* root, BSTNode* parent, BSTNode* matchedBSTNode, BSTNode* child)
{

    if(parent == NULL){
        root = child;
    }else if(parent->left == matchedBSTNode){
        parent->left = child;
    }else{
        parent->right = child;
    }

    return true;
}

bool delete_BSTNode_Two_Child_BST(BSTNode* root, BSTNode* parent, BSTNode* matchedBSTNode)
{

    int temp = matchedBSTNode->key;
    BSTNode* successor = matchedBSTNode->right;
    BSTNode* successorParent = NULL;

    while(successor->left != NULL){
        successorParent = successor;
        successor = successor->left;
    }

    matchedBSTNode->key = successor->key;
    successor->key = temp;

    bool removed = delete_Leaf_BSTNode_BST(root,successorParent,successor);

    return removed;
}

void exportBSTJSON(BSTNode* node, FILE* out)
{
    if(node == NULL){
        fprintf(out,"null");
        return;
    }

    fprintf(out,"{");

    fprintf(out,"\"key\": %d,",node->key);

    fprintf(out,"\"left\": ");
    exportBSTJSON(node->left,out);

    fprintf(out,",\"right\": ");
    exportBSTJSON(node->right,out);

    fprintf(out,"}");

}
