#pragma once
#include "interval_tree.h"

#include <assert.h>

struct interval_node_s {
    u32 begin,
        end;
    void* data;
    interval_node_t* left;
    interval_node_t* right;
};

interval_node_t* interval_tree_search(interval_node_t* node, u32 key) {
    // End of tree case
    if (node == NULL)
        return NULL;

    // Key matches this node's interval
    if (key >= node->begin && key <= node->end)
        return node;

    // Check left (less than) or right (greater than)
    if (node->left && key < node->begin)
        return interval_tree_search(node->left, key);
    else if (node->right)
        return interval_tree_search(node->right, key);

    // No match in this node or children
    return NULL;
}

interval_node_t* interval_tree_insert(interval_node_t* node, u32 begin, u32 end, void* data) {
    assert(end > begin);

    // Empty tree 
    if (node == NULL) {
        node = (interval_node_t*)calloc(1, sizeof(interval_node_t));
        node->begin = begin;
        node->end = end;
        node->data = data;
        return node;
    }

    // Find a position for the new node

    interval_node_t* parent = NULL;

    while (node != NULL) {
        // Immediately reject any (closed) interval overlap (De Morgan's law)
        if (end >= node->begin && begin <= node->end)
            return NULL;

        // Assign `parent` to the currently selected node before `node` becomes the next node
        parent = node;

        // Check and assign next node
        if (end < node->begin)
            node = node->left;
        else
            node = node->right;
    }

    // Create the new node
    node = (interval_node_t*)calloc(1, sizeof(interval_node_t));
    node->begin = begin;
    node->end = end;
    node->data = data;

    // Assign the new node's position to its parent
    if (end < parent->begin)
        parent->left = node;
    else 
        parent->right = node;

    return node;
} 

void interval_tree_free(interval_node_t* root) {
    if (root == NULL)
        return;

    interval_tree_free(root->left);
    interval_tree_free(root->right);

    free(root);
}

b8 interval_node_test(interval_node_t* node, u32 key) {
    return (key >= node->begin && key <= node->end);
}

void* interval_node_get_data(interval_node_t* node) {
    return node->data;
}