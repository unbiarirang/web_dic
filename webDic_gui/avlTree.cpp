#include "avlTree.h"
#include <algorithm>
#include <iostream>

AVLNode::AVLNode(int _key, AVLNode* _parent, WordNode* _data) : key(_key), balance(0), parent(_parent),
left(nullptr), right(nullptr), data(_data) {}

AVLNode::~AVLNode() {
	delete left;
	delete right;
}

/*
	@brief	search AVL node 
*/
AVLNode * AVLNode::search(int key)
{
	if (this == nullptr) return nullptr;
	if (this->key == key)
		return this;

	if (key < this->key)
		return this->left->search(key);
	else
		return this->right->search(key);
}

AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
	delete root;
}

/*
	@brief	insert element into AVL tree
	@return	insertion success or failure
*/
bool AVLTree::insert(int key, WordNode* data) {
	if (root == nullptr) {
		root = new AVLNode(key, nullptr, data);
		return true;
	}

	AVLNode *node = root;
	AVLNode	*parent;

	while (true) {
		// the key is already exist
		if (node->key == key)
			return false;

		parent = node;

		// decide whether to go left or go right 
		bool is_left = node->key > key;
		node = is_left ? node->left : node->right;

		if (node == nullptr) {
			if (is_left)
				parent->left = new AVLNode(key, parent, data);
			else
				parent->right = new AVLNode(key, parent, data);

			adjust(parent);
			break;
		}
	}

	return true;
}

WordNode * AVLTree::search(int key)
{
	if (this == nullptr) return nullptr;
	if (this->root == nullptr) return nullptr;

	AVLNode* node = this->root->search(key);

	if (node == nullptr) return nullptr; // key not found

	return node->data;
}

/*
	@brief	edit element info
	@return	edit success or failure
*/
bool AVLTree::edit(int key, WordNode * data)
{
	if (root == nullptr) return false;	// key not found

	AVLNode *node = root;
	AVLNode	*parent;

	while (true) {
		// the key is exist
		if (node->key == key) {
			node->data = data;	// replace old data with new data
			return true;
		}

		parent = node;

		// decide whether to go left or go right 
		bool is_left = node->key > key;
		node = is_left ? node->left : node->right;

		if (node == nullptr) break;	// key not found
	}

	return false;
}

/*
	@brief	remove element from AVL tree
*/
void AVLTree::remove(int key) {
	if (root == nullptr)
		return;

	AVLNode
		*n = root,
		*parent = root,
		*delNode = nullptr,	// node to be removed
		*child = root;

	while (child != nullptr) {
		parent = n;
		n = child;
		child = key >= n->key ? n->right : n->left;
		if (key == n->key)
			delNode = n;
	}

	if (delNode == nullptr)	return; // the key not exist

	delNode->key = n->key;
	delNode->data = n->data;
	child = n->left != nullptr ? n->left : n->right;

	if (root->key == key)
		root = child;
	else {
		if (parent->left == n)
			parent->left = child;
		else
			parent->right = child;

		adjust(parent);
	}
}

/*
	@brief	Left-Left rotation
*/
AVLNode* AVLTree::rotateLL(AVLNode *node) {
	AVLNode *r_node = node->right;	// root node after rotation
	r_node->parent = node->parent;
	node->right = r_node->left;

	if (node->right != nullptr)
		node->right->parent = node;

	r_node->left = node;
	node->parent = r_node;

	if (r_node->parent != nullptr) {
		if (r_node->parent->right == node)
			r_node->parent->right = r_node;
		else
			r_node->parent->left = r_node;
	}

	setBalance(node);
	setBalance(r_node);
	return r_node;
}

/*
	@brief	Right-Right rotation
*/
AVLNode* AVLTree::rotateRR(AVLNode *node) {
	AVLNode *r_node = node->left;	// root node after rotation
	r_node->parent = node->parent;
	node->left = r_node->right;

	if (node->left != nullptr)
		node->left->parent = node;

	r_node->right = node;
	node->parent = r_node;

	if (r_node->parent != nullptr) {
		if (r_node->parent->right == node)
			r_node->parent->right = r_node;
		else
			r_node->parent->left = r_node;
	}

	setBalance(node);
	setBalance(r_node);
	return r_node;
}

/*
	@brief	Left-Right rotation
*/
AVLNode* AVLTree::rotateLR(AVLNode *node) {
	node->left = rotateLL(node->left);
	return rotateRR(node);
}

/*
	@brief	Right-Left Rotation
*/
AVLNode* AVLTree::rotateRL(AVLNode *node) {
	node->right = rotateRR(node->right);
	return rotateLL(node);
}

/*
	@brief balancing AVL tree
*/
void AVLTree::adjust(AVLNode *node) {
	setBalance(node);

	if (node->balance == -2) {
		if (getHeight(node->left->left) >= getHeight(node->left->right))
			node = rotateRR(node);
		else
			node = rotateLR(node);
	}
	else if (node->balance == 2) {
		if (getHeight(node->right->right) >= getHeight(node->right->left))
			node = rotateLL(node);
		else
			node = rotateRL(node);
	}

	if (node->parent != nullptr)
		adjust(node->parent);
	else
		this->root = node;
}

/*
	@brief	get height of AVL tree
	@return height
*/
int AVLTree::getHeight(AVLNode *node) {
	if (node == nullptr)
		return -1;

	return 1 + std::max(getHeight(node->left), getHeight(node->right));
}

/*
	@brief	set balance of AVL tree
*/
void AVLTree::setBalance(AVLNode *node) {
	node->balance = getHeight(node->right) - getHeight(node->left);
}