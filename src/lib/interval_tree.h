#pragma once
#include "common.h"

// (Closed) Interval tree node
// Nodes are ordered by domain
typedef struct interval_node_s interval_node_t;

// Recursively search an interval tree starting at a given node
// @param[in] node Root node
// @param[in] key Number value used to match an interval
// @returns Pointer to the matching interval node, or NULL if not found
interval_node_t* interval_tree_search(interval_node_t* node, u32 key);

// Creates a node and inserts it into an interval tree. Overlapping intervals are rejected. 
// @param[in] node Root node
// @param[in] begin The minimum bound for this closed interval
// @param[in] end The maximum bound for this closed interval
// @param[in] data Data the node is associated with
// @returns Pointer to the new node, or NULL if interval overlap was attempted
interval_node_t* interval_tree_insert(interval_node_t* node, u32 begin, u32 end, void* data);

// Frees all nodes within an interval tree
// @param[in] root The root (or subroot) of the interval tree to destroy
void interval_tree_free(interval_node_t* root);

// Tests if the given key is within the interval on a discrete node
// @param[in] key
// @returns True on collision
b8 interval_node_test(interval_node_t* node, u32 key);

// @returns The data associated with this interval tree node
void* interval_node_get_data(interval_node_t* node);