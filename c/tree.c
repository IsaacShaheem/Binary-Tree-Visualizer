#include "tree.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BSTNode {
    int key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

struct BSTTree {
    BSTNode *root;
};

typedef struct AVLNode {
    int key;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

struct AVLTree {
    AVLNode *root;
    char last_rotation[3];
};

typedef struct JsonBuffer {
    char *data;
    size_t length;
    size_t capacity;
    bool failed;
} JsonBuffer;

static bool json_reserve(JsonBuffer *buffer, size_t extra)
{
    size_t required = buffer->length + extra + 1;
    char *next;
    size_t next_capacity;

    if (buffer->failed || required <= buffer->capacity) {
        return !buffer->failed;
    }

    next_capacity = buffer->capacity == 0 ? 256 : buffer->capacity;
    while (next_capacity < required) {
        next_capacity *= 2;
    }

    next = realloc(buffer->data, next_capacity);
    if (next == NULL) {
        free(buffer->data);
        buffer->data = NULL;
        buffer->length = 0;
        buffer->capacity = 0;
        buffer->failed = true;
        return false;
    }

    buffer->data = next;
    buffer->capacity = next_capacity;
    return true;
}

static void json_append(JsonBuffer *buffer, const char *format, ...)
{
    va_list args;
    va_list copy;
    int needed;

    if (buffer->failed) {
        return;
    }

    va_start(args, format);
    va_copy(copy, args);
    needed = vsnprintf(NULL, 0, format, copy);
    va_end(copy);

    if (needed < 0 || !json_reserve(buffer, (size_t)needed)) {
        buffer->failed = true;
        va_end(args);
        return;
    }

    vsnprintf(buffer->data + buffer->length, buffer->capacity - buffer->length, format, args);
    buffer->length += (size_t)needed;
    va_end(args);
}

static char *json_finish(JsonBuffer *buffer)
{
    if (buffer->failed) {
        free(buffer->data);
        return NULL;
    }

    if (!json_reserve(buffer, 0)) {
        return NULL;
    }

    buffer->data[buffer->length] = '\0';
    return buffer->data;
}

static BSTNode *bst_node_create(int value)
{
    BSTNode *node = malloc(sizeof(BSTNode));
    if (node == NULL) {
        return NULL;
    }

    node->key = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static BSTNode *bst_insert_node(BSTNode *root, int value, bool *inserted)
{
    if (root == NULL) {
        BSTNode *node = bst_node_create(value);
        *inserted = node != NULL;
        return node;
    }

    if (value < root->key) {
        root->left = bst_insert_node(root->left, value, inserted);
    } else if (value > root->key) {
        root->right = bst_insert_node(root->right, value, inserted);
    } else {
        *inserted = false;
    }

    return root;
}

static bool bst_search_node(const BSTNode *node, int value)
{
    while (node != NULL) {
        if (value == node->key) {
            return true;
        }
        node = value < node->key ? node->left : node->right;
    }

    return false;
}

static BSTNode *bst_detach_min(BSTNode *node, BSTNode **min_node)
{
    if (node->left == NULL) {
        *min_node = node;
        return node->right;
    }

    node->left = bst_detach_min(node->left, min_node);
    return node;
}

static BSTNode *bst_delete_node(BSTNode *node, int value, bool *deleted)
{
    BSTNode *replacement;
    BSTNode *successor;

    if (node == NULL) {
        *deleted = false;
        return NULL;
    }

    if (value < node->key) {
        node->left = bst_delete_node(node->left, value, deleted);
        return node;
    }

    if (value > node->key) {
        node->right = bst_delete_node(node->right, value, deleted);
        return node;
    }

    *deleted = true;

    if (node->left == NULL) {
        replacement = node->right;
        free(node);
        return replacement;
    }

    if (node->right == NULL) {
        replacement = node->left;
        free(node);
        return replacement;
    }

    node->right = bst_detach_min(node->right, &successor);
    successor->left = node->left;
    successor->right = node->right;
    free(node);
    return successor;
}

static void bst_export_node(const BSTNode *node, JsonBuffer *buffer)
{
    if (node == NULL) {
        json_append(buffer, "null");
        return;
    }

    json_append(buffer, "{\"key\":%d,\"left\":", node->key);
    bst_export_node(node->left, buffer);
    json_append(buffer, ",\"right\":");
    bst_export_node(node->right, buffer);
    json_append(buffer, "}");
}

static void bst_free_node(BSTNode *node)
{
    if (node == NULL) {
        return;
    }

    bst_free_node(node->left);
    bst_free_node(node->right);
    free(node);
}

BSTTree *bst_create(void)
{
    BSTTree *tree = malloc(sizeof(BSTTree));
    if (tree == NULL) {
        return NULL;
    }

    tree->root = NULL;
    return tree;
}

bool bst_insert(BSTTree *tree, int value)
{
    bool inserted = false;
    if (tree == NULL) {
        return false;
    }

    tree->root = bst_insert_node(tree->root, value, &inserted);
    return inserted;
}

bool bst_search(const BSTTree *tree, int value)
{
    return tree != NULL && bst_search_node(tree->root, value);
}

bool bst_delete(BSTTree *tree, int value)
{
    bool deleted = false;
    if (tree == NULL) {
        return false;
    }

    tree->root = bst_delete_node(tree->root, value, &deleted);
    return deleted;
}

char *bst_export_json(const BSTTree *tree)
{
    JsonBuffer buffer = {0};
    if (tree == NULL) {
        return NULL;
    }

    bst_export_node(tree->root, &buffer);
    return json_finish(&buffer);
}

void bst_destroy(BSTTree *tree)
{
    if (tree == NULL) {
        return;
    }

    bst_free_node(tree->root);
    free(tree);
}

static int avl_height(const AVLNode *node)
{
    return node == NULL ? -1 : node->height;
}

static int avl_max(int left, int right)
{
    return left > right ? left : right;
}

static void avl_update_height(AVLNode *node)
{
    if (node != NULL) {
        node->height = avl_max(avl_height(node->left), avl_height(node->right)) + 1;
    }
}

static int avl_balance(const AVLNode *node)
{
    return node == NULL ? 0 : avl_height(node->left) - avl_height(node->right);
}

static AVLNode *avl_node_create(int value)
{
    AVLNode *node = malloc(sizeof(AVLNode));
    if (node == NULL) {
        return NULL;
    }

    node->key = value;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static AVLNode *avl_rotate_right(AVLNode *pivot)
{
    AVLNode *new_root = pivot->left;
    AVLNode *moved_subtree = new_root->right;

    new_root->right = pivot;
    pivot->left = moved_subtree;

    avl_update_height(pivot);
    avl_update_height(new_root);
    return new_root;
}

static AVLNode *avl_rotate_left(AVLNode *pivot)
{
    AVLNode *new_root = pivot->right;
    AVLNode *moved_subtree = new_root->left;

    new_root->left = pivot;
    pivot->right = moved_subtree;

    avl_update_height(pivot);
    avl_update_height(new_root);
    return new_root;
}

static void avl_set_rotation(char rotation[3], const char *value)
{
    rotation[0] = value[0];
    rotation[1] = value[1];
    rotation[2] = '\0';
}

static AVLNode *avl_rebalance(AVLNode *node, char rotation[3])
{
    int balance;

    if (node == NULL) {
        return NULL;
    }

    avl_update_height(node);
    balance = avl_balance(node);

    if (balance > 1) {
        if (avl_balance(node->left) < 0) {
            avl_set_rotation(rotation, "LR");
            node->left = avl_rotate_left(node->left);
        } else {
            avl_set_rotation(rotation, "LL");
        }
        return avl_rotate_right(node);
    }

    if (balance < -1) {
        if (avl_balance(node->right) > 0) {
            avl_set_rotation(rotation, "RL");
            node->right = avl_rotate_right(node->right);
        } else {
            avl_set_rotation(rotation, "RR");
        }
        return avl_rotate_left(node);
    }

    return node;
}

static AVLNode *avl_insert_node(AVLNode *node, int value, bool *inserted, char rotation[3])
{
    if (node == NULL) {
        AVLNode *created = avl_node_create(value);
        *inserted = created != NULL;
        return created;
    }

    if (value < node->key) {
        node->left = avl_insert_node(node->left, value, inserted, rotation);
    } else if (value > node->key) {
        node->right = avl_insert_node(node->right, value, inserted, rotation);
    } else {
        *inserted = false;
        return node;
    }

    if (!*inserted) {
        return node;
    }

    return avl_rebalance(node, rotation);
}

static bool avl_search_node(const AVLNode *node, int value)
{
    while (node != NULL) {
        if (value == node->key) {
            return true;
        }
        node = value < node->key ? node->left : node->right;
    }

    return false;
}

static AVLNode *avl_min_node(AVLNode *node)
{
    while (node != NULL && node->left != NULL) {
        node = node->left;
    }

    return node;
}

static AVLNode *avl_delete_node(AVLNode *node, int value, bool *deleted, char rotation[3])
{
    AVLNode *replacement;
    AVLNode *successor;

    if (node == NULL) {
        *deleted = false;
        return NULL;
    }

    if (value < node->key) {
        node->left = avl_delete_node(node->left, value, deleted, rotation);
    } else if (value > node->key) {
        node->right = avl_delete_node(node->right, value, deleted, rotation);
    } else {
        *deleted = true;

        if (node->left == NULL || node->right == NULL) {
            replacement = node->left != NULL ? node->left : node->right;
            free(node);
            return replacement;
        }

        successor = avl_min_node(node->right);
        node->key = successor->key;
        node->right = avl_delete_node(node->right, successor->key, deleted, rotation);
    }

    if (!*deleted) {
        return node;
    }

    return avl_rebalance(node, rotation);
}

static void avl_export_node(const AVLNode *node, JsonBuffer *buffer)
{
    if (node == NULL) {
        json_append(buffer, "null");
        return;
    }

    json_append(
        buffer,
        "{\"key\":%d,\"height\":%d,\"balance\":%d,\"left\":",
        node->key,
        node->height,
        avl_balance(node));
    avl_export_node(node->left, buffer);
    json_append(buffer, ",\"right\":");
    avl_export_node(node->right, buffer);
    json_append(buffer, "}");
}

static void avl_free_node(AVLNode *node)
{
    if (node == NULL) {
        return;
    }

    avl_free_node(node->left);
    avl_free_node(node->right);
    free(node);
}

AVLTree *avl_create(void)
{
    AVLTree *tree = malloc(sizeof(AVLTree));
    if (tree == NULL) {
        return NULL;
    }

    tree->root = NULL;
    tree->last_rotation[0] = '\0';
    return tree;
}

bool avl_insert(AVLTree *tree, int value)
{
    bool inserted = false;
    if (tree == NULL) {
        return false;
    }

    tree->last_rotation[0] = '\0';
    tree->root = avl_insert_node(tree->root, value, &inserted, tree->last_rotation);
    return inserted;
}

bool avl_search(const AVLTree *tree, int value)
{
    return tree != NULL && avl_search_node(tree->root, value);
}

bool avl_delete(AVLTree *tree, int value)
{
    bool deleted = false;
    if (tree == NULL) {
        return false;
    }

    tree->last_rotation[0] = '\0';
    tree->root = avl_delete_node(tree->root, value, &deleted, tree->last_rotation);
    return deleted;
}

const char *avl_last_rotation(const AVLTree *tree)
{
    if (tree == NULL || tree->last_rotation[0] == '\0') {
        return "";
    }

    return tree->last_rotation;
}

char *avl_export_json(const AVLTree *tree)
{
    JsonBuffer buffer = {0};
    if (tree == NULL) {
        return NULL;
    }

    avl_export_node(tree->root, &buffer);
    return json_finish(&buffer);
}

void avl_destroy(AVLTree *tree)
{
    if (tree == NULL) {
        return;
    }

    avl_free_node(tree->root);
    free(tree);
}

void tree_free_json(char *json)
{
    free(json);
}
