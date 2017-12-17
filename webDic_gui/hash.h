#pragma once
#include <vector>
#include <string>
#include "charString.h"
#include "csNode.h"
#include "docNode.h"
#include "wordNode.h"

const int MAX_BUCKET_NUM = 101;

/*
	@brief	CharString node hash table. create dictionary
*/
typedef struct HashTable {
	CSNode* m_htable[MAX_BUCKET_NUM];

public:
	HashTable();
	bool isExist(CharString key);
	void append(CharString key);
	void appendFrom(CharString file_name);

private:
	int hash(CharString key);
} HashTable;

/*
	@brief	Word node hash table for inverse document
*/
typedef struct HashDoc {
	WordNode* m_htable[MAX_BUCKET_NUM];

public:
	HashDoc();
	~HashDoc();
	bool isExist(int wordID);
	bool insert(int wordID, WordNode* data);
	WordNode* search(int wordID);

private:
	int hash(int wordID);
} HashDoc;