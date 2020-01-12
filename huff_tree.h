/*
Name:Jiangxue Han
File: huff_tree.h
Desc:
This is the header file of simulation of the huff_tree_node "class"
with the struct definition and signatures of functions
 */
#include<stdio.h>
#include<stdlib.h>

typedef struct huff_tree_node {
  int ch;
  int freq;
  struct huff_tree_node* left;
  struct huff_tree_node* right;
} huff_tree_node;


huff_tree_node* createNode(int myChar, int myFreq);
void heapInsert(huff_tree_node* heap[], int size, huff_tree_node* dta);
huff_tree_node* heapPoll(huff_tree_node* heap[], int heapSize);

