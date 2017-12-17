#pragma once
#include <list>
#include "charString.h"
#include "hash.h"

/*
	@brief	hashMap node. key: CharString, value: int
*/
typedef struct MNode {
public:
	CharString key;
	int value;

public:
	MNode() : key(""), value(0) {}
	MNode(CharString k, int v) : key(k), value(v) {}
} MNode;

typedef struct Map {
private:
	std::list<MNode*> data[MAX_BUCKET_NUM];

public:
	Map();
	~Map();
	bool insert(CharString key, int value);

	// array subscript operator overloading
	int operator[](CharString key);

private:
	MNode* get(CharString key);
	int hash(CharString key);
} Map;


//FIXME: delete
/*
@brief	hashMap integer node. key: int, value: int
*/
typedef struct IntMNode {
public:
	int key;
	int value;

public:
	IntMNode() : key(0), value(0) {}
	IntMNode(int k, int v) : key(k), value(v) {}
} IntMNode;

typedef struct IntMap {
private:
	std::list<IntMNode*> data[MAX_BUCKET_NUM];

public:
	IntMap();
	~IntMap();
	bool insert(int key, int value);

	// array subscript operator overloading
	int operator[](int key);

private:
	IntMNode* get(int key);
	int hash(int key);
} IntMap;
