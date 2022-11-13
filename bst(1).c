#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "bst.h"

/*

Place for the BST functions from Exercise 1.
*/
Node *addNode(Node *root, int value)
{
	Node *newNode = malloc(sizeof(Node));
	newNode->data = value;
	newNode->left = NULL;
	newNode->right = NULL;

	Node *temp;
	Node *prev = NULL;

	temp = root;
	while (temp)
	{
		prev = temp;
		if (temp->data > newNode->data)
			temp = temp->right;
		else if (temp->data < newNode->data)
			temp = temp->left;
		else
		{
			free(newNode);
			return root;
		}
	}
	if (prev == NULL)
		root = newNode;
	else
	{
		if (prev->data > newNode->data)
			prev->right = newNode;
		else
			prev->left = newNode;
	}

	return root;
}

Node *freeSubtree(Node *N)
{
	if (N == NULL)
		return NULL;
	freeSubtree(N->left);
	freeSubtree(N->right);
	free(N);
	return N;
}

int countNodes(Node *N)
{
	int count = 0;
	if (N == NULL)
		return 0;
	count = count + countNodes(N->right);
	count = count + countNodes(N->left);
	count = count + 1;
	return count;
}

Node *removeNode(Node *root, int value)
{
	// Ensure root isn't null.
	if (root == NULL)
	{
		return NULL;
	}

	if (value > root->data)
	{ // Value is in the left sub-tree.
		root->left = removeNode(root->left, value);
	}
	else if (value < root->data)
	{ // Value is in the right sub-tree.
		root->right = removeNode(root->right, value);
	}
	else
	{ // Found the correct node with value
		// Check the three cases - no child, 1 child, 2 child...
		// No Children
		if (root->left == NULL && root->right == NULL)
		{
			free(root);
			root = NULL;
		}
		// 1 child (on the right)
		else if (root->left == NULL)
		{
			Node *temp = root; // save current node
			root = root->right;
			free(temp);
		}
		// 1 child (on the left)
		else if (root->right == NULL)
		{
			Node *temp = root; // save current node
			root = root->left;
			free(temp);
		}
		// Two children
		else
		{
			// find minimal value of right sub tree
			Node *temp = root->left;
			while (temp->right != NULL)
			{
				temp = temp->right;
			}
			root->data = temp->data;						 // duplicate the node
			root->left = removeNode(root->left, root->data); // delete the duplicate node
		}
	}
	return root; // parent node can update reference
}

void displaySubtree(Node *N)
{
	if (N == NULL)
	{
		return;
	}
	displaySubtree(N->right);
	printf("%d\n", N->data);
	displaySubtree(N->left);
}

/*
	Perform inorder traversal along the given BST and store the node values in an array.
*/

void inorder(Node *root, int *array, int *index)
{
	if (root == NULL)
		return;

	inorder(root->left, array, index);
	array[(*index)++] = root->data;
	inorder(root->right, array, index);
}

/*
	1) Get the middle element of the sorted array and make it the root of the tree.
	2) Recursively do same for the left half and right half.
		a) Get the middle of the left half and make it the left child of the root created in step 1
		b) Get the middle of the right half and make it the right child of the root created in step 1.
*/
Node *balanceTreeRec(int *array, int start, int end)
{
	if (start > end)
		return NULL;

	int mid = (start + end) / 2;
	Node *root = malloc(sizeof(Node));
	root->data = array[mid];
	root->left = balanceTreeRec(array, start, mid - 1);
	root->right = balanceTreeRec(array, mid + 1, end);

	return root;
}

/*
	This function returns the sum of all the nodes.
*/
Node *balanceTree(Node *root)
{
	// Perform inorder traversal along the given BST and store the node values in an array.
	int *array = (int *)malloc(sizeof(int) * countNodes(root));
	int index = 0;
	inorder(root, array, &index);
	// create a balanced BST from a sorted array.
	Node *root_balanced = balanceTreeRec(array, 0, index - 1);
	free(array);
	return root_balanced;
}
/*int sumNodes(Node *N)
{
  int s = 0;
  if(N == NULL)
    return 0;
  s = N->data + sumNodes(N->left) + sumNodes(N->right);
  return s;
}
float avgSubtree(Node *N){
    if (N==NULL)
        return 0;
    float sum = (float)sumNodes(N);
    float count = (float)countNodes(N);
    return sum / count;
}*/
int sumSubtree(Node *N)
{
	if (N == NULL)
		return 0;
	return sumSubtree(N->right) + sumSubtree(N->left) + N->data;
}
/*
	This function returns the average of all the nodes.
*/

float avgSubtree(Node *N)
{
	if (N == NULL)
		return 0;
	return (float)sumSubtree(N) / (float)countNodes(N);
}

/* 
	This functions converts an unbalanced BST to a balanced BST
*/

