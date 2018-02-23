#pragma once
#include <iostream>
#include <string>

typedef struct CharString {
private:
	char* str;
	int length;

public:
	CharString(char* str = 0);
	CharString(std::string str);
	CharString(CharString* charStr);
	char* getStr();
	int getLen();

public:
	int indexOf(char* str);
	int indexOf(CharString* charStr);
	char* substring(int start, int end);
	void substring(int start, int end, CharString* dest);
	void concat(char* str);
	void concat(char c);
	void concat(CharString* charStr);
	void assign(char* str);
	void assign(CharString* charStr);

	CharString operator = (char* str);
} CharString;

std::ostream& operator<<(std::ostream& output, CharString& charStr);