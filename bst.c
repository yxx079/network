#include "bst.h"
#include <stdio.h>
#include<stdbool.h>
#include<assert.h>
#include<stdlib.h>
typedef struct _Node {
int value;
Node *left;
 Node *right;
}Node;
Node * findParentNode(Node * root, int value) {
  assert(root != NULL);
  assert(value != root->value);

  Node * next = value > root->value ? root->left : root->right;

  if (next == NULL || next->value == value)
    return root;
  else
    return findParentNode(next, value);
    }
Node *malloc_node(int value){
    Node * node = (Node *) malloc(sizeof(Node));
  node->value = value;
  node->left = node->right = NULL;
  return node;
}

Node * addNode(Node * root, int value){
  if (root == NULL)
    return malloc_node(value);
    
  if (value == root->value)
    return NULL;
    
 Node *current;
 Node **link;
 link=&root;
 while((current=*link)!=NULL ){
if(value>current->value){
link=&current->left;
}
else if(value<current->value){

  link =&current->right;
}
else{
return NULL;}
}
current = malloc_node(value);
*link=current;
return current;

}


bool is_ordered(Node * root) {
  if (root == NULL)
    return true;
  if (root->left && root->left->value < root->value)
    return false;
  if (root->right && root->right->value > root->value)
    return false;
  return true;
}



Node * removeNode(Node * root, int value){
   // empty tree
  if (root == NULL)
    return NULL;
  // find node with value 'value' and its parent node 
  Node * parent, * node;
  
  if (root->value == value) {
    parent = NULL;
    node = root;
  } else {
    parent = findParentNode(root, value);
    node = value >parent->value ? parent->left : parent->right;
  }
  assert(node == NULL || node->value == value);

  // value not found
  if (node == NULL)
    return root;

  // re-establish consistency
  Node * new_node;
  if (node->left == NULL) {
    // node has only right child, then make right child the new node
    new_node = node->right;
  } else {
    // otherwise make right child the rightmost leaf of the subtree rooted in the left child
    // and make the left child the new node
    Node * rightmost = new_node = node->left;
    while (rightmost->right != NULL)
      rightmost = rightmost->right;
    rightmost->right = node->right;
  }

  free(node);

  Node * new_root;
  if (parent == NULL) {
    // if deleted node was root, then return new node as root 
    new_root = new_node;

  } else {
    // otherwise glue new node with parent and return old root
    new_root = root;
    if (value > parent->value) 
      parent->left = new_node;
    else 
      parent->right = new_node;
  }

  assert(is_ordered(new_root));

  return new_root;
}


void displaySubtree(Node * N){
     if (N == NULL) return;
  displaySubtree(N->right);
  printf("%d \n", N->value);
  displaySubtree(N->left);
    }

int numberLeaves(Node * N){
  if (N == NULL)
    return 0;

  if (N->left == NULL && N->right == NULL)
    return 1;
  
  return numberLeaves(N->left) + numberLeaves(N->right);
}
void freeTreeNode(Node * root) {
  if (root == NULL)
    return;

  freeTreeNode(root->left);
  freeTreeNode(root->right);
  free(root);
}

Node * removeSubtree(Node * root, int value){

  if (root == NULL)
    return NULL;

  if (root->value == value) {
    freeTreeNode(root);
    return NULL;
  }

  Node * parent = findParentNode(root, value);
  if (value >parent->value) {
    assert(parent->left == NULL || parent->left->value == value);
   freeTreeNode(parent->left);
    parent->left = NULL;
  } else {
    assert(parent->right == NULL || parent->right->value == value);
   freeTreeNode(parent->right);
    parent->right = NULL;
  }

  return root;

}

int nodeDepth (Node * root, Node * N){
  int dist=-1;

if (root == NULL || N == NULL)
    return -1;
  if (root->value == N->value)
    return 0;

if
(dist=nodeDepth(root->left,N)>=0)
return dist+1;
if
 (dist=nodeDepth(root->right,N)>=0)
  return dist+1;

return dist;
}
