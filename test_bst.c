#include <stdio.h>
#include <assert.h>

#include "bst.c"
#include "bst.h"

int main() {
  Node *a, *b,*e;
  a = addNode(NULL, 5);
   addNode(a,10);
  addNode(a,15);
  b=addNode(a,15);
  e=addNode(a,20);
  
  assert(e!=NULL);
  displaySubtree(a);
  int c = numberLeaves(a);
  printf("叶子数:%d\n",c);
  assert(c == 1);

  int d = nodeDepth(a,b);
  printf("深度为%d\n",d);
 

  // a = removeSubtree(a, 15);
  a=removeNode(a, 5);
  printf("%d\n", a->value);
   displaySubtree(a);

  
  return 0;
}