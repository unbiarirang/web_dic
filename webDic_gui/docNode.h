#pragma once

typedef int DocID;		// document id type

/*
	@brief	document node for inverse document 
*/
typedef struct DocNode {
	DocID id;			// document unique id
	int occur_count;	// occurence count of a word in the document
	DocNode* next;

public:
	DocNode(DocID id = 0);
	~DocNode();
} DocNode;

/*
	@brief	document node list for inverse document
*/
typedef struct DocList {
private:
	DocNode* head;
	DocNode* tail;
	int size;

public:
	DocList();
	~DocList();

	// get private variables
	int getSize();
	DocNode* getHead();
	DocNode* getTail();

	void add(DocID id);
	void remove(DocID id);
	DocNode* search(DocID id);
	void edit(DocID id);
} DocList;