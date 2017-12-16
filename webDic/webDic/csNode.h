#pragma once
#include "charString.h"

typedef struct CSNode {
	CSNode* next;
	CharString data;

public:
	CSNode();
	CSNode(char* str);
	CSNode(CharString* charString);
} CSNode;

typedef struct CSList {
private:
	CSNode* head;
	CSNode* tail;
	int size;

public:
	CSList();
	~CSList();
	
	// get private variables
	int getSize();
	CSNode* getHead();

	void add(CharString* charString);
	void add(char* str);
	void remove(int pos);
	int search(CharString* charString);
	int search(char* str);
} CSList;