#include "csNode.h"

CSList::CSList()
{
	this->head = nullptr;
	this->tail = nullptr;
	this->size = 0;
}

CSList::~CSList() {}

int CSList::getSize()
{
	return this->size;
}

CSNode * CSList::getHead()
{
	return this->head;
}

void CSList::add(CharString* charString)
{
	if (this->head == nullptr) {
		this->head = new CSNode();
		this->tail = this->head;
		this->size++;
		return;
	}

	CSNode* node = new CSNode(charString);
	this->tail->next = node;
	this->tail = node;
	this->size++;
}

void CSList::add(char * str)
{
    CharString tempStr = str;
    this->add(&tempStr);
}

void CSList::remove(int pos)
{
	if (pos < 1 || pos > this->size) return;

	if (this->size == 1) {
		delete this->head;
		this->head = nullptr;
		this->tail = nullptr;
		this->size--;
		return;
	}

	CSNode* prev_node = this->head;
	CSNode* node = this->head->next;
	int pos_now = 2;
	while (true) {
		if (pos_now == pos) break;
		node = node->next;
		prev_node = prev_node->next;
		pos_now++;
	}

	prev_node->next = node->next;
	delete node;
	this->size--;
}

int CSList::search(CharString* charString)
{
	CSNode* node = this->head;
	int pos = 1;
	while (true) {
		if (pos > this->size) break;
		if (node->data.getStr() == charString->getStr()) break;
		node = node->next;
		pos++;
	}

	if (pos > this->size) return 0; // not found

	return pos;
}

int CSList::search(char * str)
{
    CharString tempStr = str;
    return this->search(&tempStr);
}

CSNode::CSNode()
{
	this->next = nullptr;
	this->data = "";
}

CSNode::CSNode(char * str)
{
	this->next = nullptr;
	this->data = str;
}

CSNode::CSNode(CharString* charString)
{
	this->next = nullptr;
	this->data = *charString;
}
