//
//  window.c
//  New Reliable UDP
//
//  Created by Simone Minasola on 29/10/15.
//  Copyright (c) 2015 Simone Minasola. All rights reser

#include "window.h"


/*
 * return height of a node
 */
int height(struct tree_node *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

/*
 * return the bigger value between a and b
 */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/*
 * create a new node and return its pointer
 */
struct tree_node *new_node(struct packet *pkt) {
    //tree_node *new_node = (tree_node *)calloc(1, sizeof(tree_node));
    struct tree_node *new_node = malloc(sizeof(struct tree_node));
    if (new_node == NULL) {
        perror("Errore in malloc...scrivere meglio\n");
        exit(EXIT_FAILURE);
    }
    
    
    new_node->pkt = pkt;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->height = 0;
    
    return new_node;
}

/*
 * right rotate subtree_node rooted with y
 */
struct tree_node *right_rotate(struct tree_node *y) {
    struct tree_node *x = y->left;
    struct tree_node *z = x->right;
    
    /* rotation */
    x->right = y;
    y->left = z;
    
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    
    /* return new root */
    return x;
}

/*
 * left rotate subtree_node rooted with x
 */
struct tree_node *left_rotate(struct tree_node *x) {
    struct tree_node *y = x->right;
    struct tree_node *z = y->left;
    
    /* rotation */
    y->left = x;
    x->right = z;
    
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    
    /* return new root */
    return y;
}

/*
 * get the difference between height of left subtree_node and height of right subtree_node
 */
int get_balance(struct tree_node *node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

/*
 * insert a node, caculate the balance of the node and rotate a tree_node to make it balanced
 */
struct tree_node *node_insert(struct tree_node *node, struct packet *pkt) {
    int balance;
    
    /* if empty tree */
    if (node == NULL)
        return (new_node(pkt));
    
    /* if not empty tree */
    if (pkt->seq < node->pkt->seq)
        node->left = node_insert(node->left, pkt);
    else
        node->right = node_insert(node->right, pkt);
    
    node->height = max(height(node->left), height(node->right)) + 1;
    balance = get_balance(node);
    
    /* Left Left */
    if (balance > 1 && pkt->seq < (node->left)->pkt->seq)
        return right_rotate(node);
    /* Left Right */
    if (balance > 1 && pkt->seq > (node->left)->pkt->seq) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    /* Right Right */
    if (balance < -1 && pkt->seq > (node->right)->pkt->seq)
        return left_rotate(node);
    /* Right Left */
    if (balance < -1 && pkt->seq < (node->right)->pkt->seq) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }
    
    return node;
}


struct tree_node *find_min(struct tree_node *root) {
    struct tree_node *min;
    
    min = root;
    while (min->left != NULL)
        min = min->left;
    
    return min;
}


struct tree_node* find_max(struct tree_node *root) {
    struct tree_node *max;
    
    max = root;
    if(max != NULL)
        while(max->right != NULL)
            max = max->right;
    
    return max;
}

struct tree_node* find_node(struct tree_node *root, long long int seq) {
    if(root == NULL)
        return NULL;
    if(seq < root->pkt->seq)
        return find_node(root->left, seq);
    else if(seq > root->pkt->seq)
        return find_node(root->right, seq);
    else
        return root;
}

struct tree_node *node_delete(struct tree_node *node, long long int seq) {
    int balance;
    struct tree_node *temp = NULL;
    
    ////////////////Creare una funzione a parte
    if (node == NULL)
        return node;
    if (seq < node->pkt->seq)
        node->left = node_delete(node->left, seq);
    else if (seq > node->pkt->seq)
        node->right = node_delete(node->right, seq);
    ///////////////
    
    
    else if (seq == node->pkt->seq) {
        /* no child */
        if (node->left == NULL && node->right == NULL) {
            temp = node;
            node = NULL;
            free (temp->pkt);
            free (temp);
        }
        /* one child */
        else if (node->left == NULL || node->right == NULL) {
            temp = node->left ? node->left : node->right;
            *node = *temp;
            free (temp->pkt);
            free (temp);
        }
        /* two children */
        else if (node->left && node->right) {
            temp = find_min(node->right);
            node->pkt = temp->pkt;
            node->right = node_delete(node->right, temp->pkt->seq);
        }
    }
    
    if (node == NULL)
        return node;
    node->height = max(height(node->left), height(node->right)) + 1;
    balance = get_balance(node);
    
    /* Left Left */
    if (balance > 1 && get_balance(node->left) >= 0)
        return right_rotate(node);
    /* Left Right */
    if (balance > 1 && get_balance(node->left) < 0) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    if (balance < -1 && get_balance(node->right) <= 0)
        return left_rotate(node);
    if (balance < -1 && get_balance(node->right) > 0) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }
    
    return node;
}

void dispose_tree(struct tree_node *root) {
    if(root != NULL)
    {
        dispose_tree(root->left);
        dispose_tree(root->right);
        free(root);
    }
}

/*
 * print items of a tree_node
 */
void print_tree_node(struct tree_node *node) {
    if (node) {
        print_tree_node(node->left);
        printf("[seq: %lld] [size: %zu] [type: %d]\n", node->pkt->seq, node->pkt->dimension, node->pkt->type);
        print_tree_node(node->right);
    }
}

struct tree_node *find_min_acked(struct tree_node *root) {
    struct tree_node *node;
    node = find_min(root);
    if (node->pkt->acked == 1)
        return node;
    else
        return NULL;
}