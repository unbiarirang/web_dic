#pragma once
#include "wordNode.h"

/*
	@brief	 AVL node for AVL tree. key is wordID
*/
typedef struct AVLNode {
public:
	int key;		// wordID
	int balance;	// height difference (right child height - left child height)
	AVLNode *left, *right, *parent;

	WordNode* data;	// word node data

public:
	AVLNode(int key, AVLNode* parent, WordNode* data);
	~AVLNode();

	AVLNode* search(int key);
} AVLNode;

/*
	@brief	 AVL tree for inverse document. key is wordID
*/
typedef struct AVLTree {
private:
	AVLNode *root;

public:
	AVLTree();
	~AVLTree();
	bool insert(int key, WordNode* data);
	WordNode* search(int key);
	bool edit(int key, WordNode* data);
	void remove(int key);

private:
	AVLNode* rotateLL(AVLNode *node);
	AVLNode* rotateRR(AVLNode *node);
	AVLNode* rotateLR(AVLNode *node);
	AVLNode* rotateRL(AVLNode *node);

	void adjust(AVLNode *node);		// rebalance tree
	int getHeight(AVLNode *node);	// get tree height
	void setBalance(AVLNode *node);	// get tree balance
} AVLTree;