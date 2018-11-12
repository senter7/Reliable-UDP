//
//  window.h
//  New Reliable UDP
//
//  Created by Simone Minasola on 29/10/15.
//  Copyright (c) 2015 Simone Minasola. All rights reser

#ifndef __New_Reliable_UDP__window__
#define __New_Reliable_UDP__window__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "packet.h"

/*
 * data structure of AVL tree_node
 */
struct tree_node
{
    struct packet *pkt;
    struct tree_node *left;
    struct tree_node *right;
    int height;
};

struct tree_node *new_node(struct packet *pkt);

struct tree_node *node_insert(struct tree_node *node, struct packet *pkt);

struct tree_node *find_min(struct tree_node *root);

struct tree_node* find_max(struct tree_node *root);

struct tree_node* find_node(struct tree_node *root, long long int seq);

struct tree_node *node_delete(struct tree_node *node, long long int seq);

struct tree_node *find_min_acked(struct tree_node *root);

void dispose_tree(struct tree_node *root);

//only for debug
void print_tree_node(struct tree_node *node);

#endif /* defined(__New_Reliable_UDP__window__) */
