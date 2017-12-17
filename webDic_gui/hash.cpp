#include "hash.h"
#include <fstream>
#include <iostream>
#include <cstring>

HashTable::HashTable()
{
	for (int i = 0; i < MAX_BUCKET_NUM; i++)
		this->m_htable[i] = nullptr;
}

bool HashTable::isExist(CharString key)
{
	int hash_key = hash(key);

	if (this->m_htable[hash_key] == nullptr) return false;

	CSNode* n = this->m_htable[hash_key];
	while (n) {
        if (strcmp(n->data.getStr(), key.getStr()) == 0)
			return true;

		n = n->next;
	}

	return false;
}

void HashTable::append(CharString key)
{
	int hash_key = hash(key);

	CSNode *n = new CSNode();
	n->data = key;
	n->next = nullptr;

	if (this->m_htable[hash_key] == nullptr)
		this->m_htable[hash_key] = n;
	else {
		n->next = m_htable[hash_key];
		m_htable[hash_key] = n;
	}
}


const int MAX_FILE_NAME_LENGTH = 48;
void HashTable::appendFrom(CharString file_name)
{
	char buffer[MAX_FILE_NAME_LENGTH] = { 0 };
	std::ifstream readFile(file_name.getStr());
	try {
		if (readFile.is_open() == true) {
			std::cout << "read file: " << file_name << "\n";
			while (readFile.getline(buffer, sizeof(char) * MAX_FILE_NAME_LENGTH))
				this->append(buffer);

			std::cout << "success append words to hash table from " << file_name << "\n";
			readFile.close();
			std::cout << "close file: " << file_name << "\n";
		}
	}
	catch (bool) {
		std::cout << "fail to read file " << file_name << "\n";
	}
}

int HashTable::hash(CharString key) {
	int hash_key = key.getStr()[0] % MAX_BUCKET_NUM;
	if (hash_key < 0) hash_key = (-1 * hash_key) % MAX_BUCKET_NUM;
	return hash_key;
}

HashDoc::HashDoc()
{
	for (int i = 0; i < MAX_BUCKET_NUM; i++)
		this->m_htable[i] = nullptr;
}

HashDoc::~HashDoc()
{
}

bool HashDoc::isExist(int wordID)
{
	int hash_key = hash(wordID);

	if (this->m_htable[hash_key] == nullptr) return false;

	WordNode* n = this->m_htable[hash_key];
	while (n) {
		if (n->id == wordID)
			return true;

		n = n->next;
	}

	return false;
}

bool HashDoc::insert(int wordID, WordNode * node)
{
	if (isExist(wordID)) return false;

	int hash_key = hash(wordID);

	if (this->m_htable[hash_key] == nullptr) // bucket is empty
		this->m_htable[hash_key] = node;
	else {
		node->next = m_htable[hash_key];
		m_htable[hash_key] = node;
	}
}

WordNode * HashDoc::search(int wordID)
{
	int hash_key = hash(wordID);

	if (this->m_htable[hash_key] == nullptr) return nullptr;

	WordNode* n = this->m_htable[hash_key];
	while (n) {
		if (n->id == wordID)
			return n;

		n = n->next;
	}

	return nullptr;
}

int HashDoc::hash(int wordID)
{
	return wordID % MAX_BUCKET_NUM;
}
