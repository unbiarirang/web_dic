#include "docNode.h"

DocNode::DocNode(DocID id) // 'id = 0' means no id value
{
	this->id = id;
	this->occur_count = 1;
	this->next = nullptr;
}

DocNode::~DocNode() {}

DocList::DocList() : head(nullptr), tail(nullptr), size(0) {}

DocList::~DocList() {}

int DocList::getSize()
{
	return this->size;
}

DocNode * DocList::getHead()
{
	return this->head;
}

DocNode * DocList::getTail()
{
	return this->tail;
}

/*
	@brief	add a document to document list
*/
void DocList::add(DocID id)
{
	if (id < 1) return; // not valid document node

	DocNode* node = search(id);
	// the doc node already exist
	if (node) {
		edit(id);	// edit doc node
		return;
	}

	node = new DocNode(id);

	if (this->head == nullptr) {
		this->head = node;
		this->tail = this->head;
		this->size++;
		return;
	}

	this->tail->next = node;
	this->tail = node;
	this->size++;
}

/*
	@brief	remove a document from document list
*/
void DocList::remove(DocID id)
{
	if (id < 1) return; // not valid document id

	if (this->size == 1) {
		delete this->head;
		this->head = nullptr;
		this->tail = nullptr;
		this->size--;
		return;
	}

	DocNode* prev_node = this->head;
	DocNode* node = this->head->next;
	while (true) {
		if (node == nullptr) return; // document not found
		if (node->id == id) break;
		node = node->next;
		prev_node = prev_node->next;
	}

	prev_node->next = node->next;
	delete node;
	this->size--;
}

/*
	@brief	search a document from document list
	@return document node pointer
*/
DocNode* DocList::search(DocID id)
{
	if (id < 1) return nullptr; // not valid document id

	DocNode* node = this->head;
	while (true) {
		if (node == nullptr) return nullptr; // document not found
		if (node->id == id) break;
		node = node->next;
	}

	return node;
}

/*
	@brief	edit doc node. occur_count add 1
*/
void DocList::edit(DocID id)
{
	DocNode* node = search(id);
	node->occur_count += 1;
}
