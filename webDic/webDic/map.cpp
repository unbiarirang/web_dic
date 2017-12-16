#include <iostream>
#include "map.h"

Map::Map() {}

Map::~Map() {
	for (int i = 0; i < MAX_BUCKET_NUM; ++i) {
		std::list<MNode*>& val = data[i];
		for (std::list<MNode*>::iterator it = val.begin(); it != val.end(); it++) {
			MNode* n = *it;
			delete n;
		}
	}
}

/*
	@brief	get element from hashMap
*/
MNode* Map::get(CharString key) {
	int hash_key = hash(key);
	std::list<MNode*>::iterator it = data[hash_key].begin();

	while (it != data[hash_key].end()) {
		MNode ** d = &(*it);
		if (strcmp((*d)->key.getStr(), key.getStr()) == 0)
			return *d;

		it++;
	}
	return nullptr;
}

/*
	@brief	insert element into hashMap
	@return insert success or failure
*/
bool Map::insert(CharString key, int value) {
	int hash_key = hash(key);
	MNode* node = this->get(key);

	if (node != nullptr) // key already exist
		return false;

	data[hash_key].push_front(new MNode(key, value));
	return true;
}

/*
	@brief	get value of key
	@return value
*/
int Map::operator[](CharString key)
{
	if (get(key) == nullptr) return 0; // key not exist

	return get(key)->value;
}

/*
	@brief	hash funciton
	@return hash key
*/
int Map::hash(CharString key)
{
	int hash_key = key.getStr()[0] % MAX_BUCKET_NUM;
	if (hash_key < 0) hash_key = (-1 * hash_key) % MAX_BUCKET_NUM;
	return hash_key;
}

IntMap::IntMap() {}

IntMap::~IntMap()
{
	for (int i = 0; i < MAX_BUCKET_NUM; ++i) {
		std::list<IntMNode*>& val = data[i];
		for (std::list<IntMNode*>::iterator it = val.begin(); it != val.end(); it++) {
			IntMNode* n = *it;
			delete n;
		}
	}
}

/*
	@brief	insert element into hashMap. if key exist edit the value by adding new value
	@return insert success or failure
*/
bool IntMap::insert(int key, int value)
{
	int hash_key = hash(key);
	IntMNode* node = this->get(key);

	if (node != nullptr) { // key already exist
		node->value += value;
		return false;
	}

	data[hash_key].push_front(new IntMNode(key, value));
	return true;
}

/*
	@brief	get value of key
	@return value
*/
int IntMap::operator[](int key)
{
	if (get(key) == nullptr) return 0; // key not exist

	return get(key)->value;
}

/*
	@brief	get element from hashMap
*/
IntMNode * IntMap::get(int key)
{
	int hash_key = hash(key);
	std::list<IntMNode*>::iterator it = data[hash_key].begin();

	while (it != data[hash_key].end()) {
		IntMNode ** d = &(*it);
		if ((*d)->key == key)
			return *d;

		it++;
	}
	return nullptr;
}

/*
	@brief	hash funciton
	@return hash key
*/
int IntMap::hash(int key)
{
	int hash_key = key % MAX_BUCKET_NUM;
	if (hash_key < 0) hash_key = (-1 * hash_key) % MAX_BUCKET_NUM;
	return hash_key;
}
