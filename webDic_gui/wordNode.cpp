#include "wordNode.h"

WordNode::WordNode(wordID id, CharString word)
{
	this->id = id;
	this->word = word;
	this->occur_count = 0;
	this->doc_list = new DocList();
	this->next = nullptr;
}

WordNode::~WordNode() {}

/*
	@brief	add doc node to word node (or edit exist doc node)
*/
void WordNode::add(DocID docID)
{
	this->occur_count += 1;
	this->doc_list->add(docID);
}