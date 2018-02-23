#pragma once
#include "charString.h"
#include "docNode.h"

typedef int wordID;		// word id type

/*
	@brief	 word node for inverse document
*/
typedef struct WordNode {
	wordID id;			// word unique id
	CharString word;	// word string
	int occur_count;	// word occur count
	DocList* doc_list;	// document list

	WordNode* next;		// for hash table inverse document

public:
	WordNode(wordID id = 0, CharString word = "");
	~WordNode();

	void add(DocID docID);
} WordNode;