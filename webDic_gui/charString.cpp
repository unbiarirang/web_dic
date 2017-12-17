#include "charString.h"
#include <iostream>

int _index_KMP(char* S, char* T, int pos);
void _get_next(char* T, int* next);

CharString::CharString(char * str)
{
	this->str = nullptr;
	this->length = 0;

	if (str) this->assign(str);
}

CharString::CharString(std::string str)
{
	this->assign(&str[0]);
}

CharString::CharString(CharString * charStr)
{
	this->str = nullptr;
	this->length = 0;

	if (charStr) this->assign(charStr->getStr());
}

char * CharString::getStr()
{
	return this->str;
}

int CharString::getLen()
{
	return this->length;
}

int CharString::indexOf(char * str)
{
	int pos = _index_KMP(this->str, str, 1);
	return pos - 1;
}

int CharString::indexOf(CharString* charStr)
{
	return this->indexOf(charStr->getStr());
}

char * CharString::substring(int start, int end) // index
{
	if (start < 0 || end > this->length - 1 || this->length == 0) return nullptr;

	int len = end - start + 1;
	char* new_str = new char[len + 1]();
	int i = 0;
	while (true) {
		if (i == len) break;

		new_str[i] = this->str[start + i];
		i++;
	}

	return new_str;
}

void CharString::substring(int start, int end, CharString* dest) // index
{
	if (start < 0 || end > this->length - 1) return;

	int len = end - start + 1;
	char* new_str = new char[len + 1]();
	int i = 0;
	while (true) {
		if (i == len) break;

		new_str[i] = this->str[start + i];
		i++;
	}

	dest->assign(new_str);
}

void CharString::concat(char * str)
{
	int len = 0;
	while (true) {
		if (str[len] == 0) break;
		len++;
	}

	int new_len = this->length + len;
	int temp_len = this->length;
	char* temp_str = this->str;
	this->str = new char[new_len + 1]();
	this->length = new_len;

	int i = 0;
	while (true) {
		if (i == temp_len) break;

		this->str[i] = temp_str[i];
		i++;
	}
	int j = 0;
	while (true) {
		if (i == new_len) break;

		this->str[i] = str[j];
		i++; j++;
	}
}

void CharString::concat(char c)
{
	this->concat(&c);
}

void CharString::concat(CharString* charStr)
{
	this->concat(charStr->getStr());
}

void CharString::assign(char * str)
{
	int len = 0;
	while (true) {
		if (str != nullptr && str[len] == 0) break;
		len++;
	}
	this->length = len;
	this->str = new char[len + 1]();

	int i = 0;
	while (true) {
		if (i == len) break;
		this->str[i] = str[i];
		i++;
	}
}

void CharString::assign(CharString* charStr)
{
	this->assign(charStr->getStr());
}

CharString CharString::operator=(char * str)
{
	this->assign(str);
	return this;
}

std::ostream& operator<<(std::ostream& output, CharString& charStr) {
	output << charStr.getStr();
	return output;
}

int _index_KMP(char* S, char* T, int pos) {
	//  1≤pos≤StrLength(S)
	int i = pos;
	int j = 1;

	int n, m;
	int len = 0;
	while (true) {
		if (S[len] == 0) break;
		len++;
	}
	n = len;

	len = 0;
	while (true) {
		if (T[len] == 0) break;
		len++;
	}
	m = len;

	int* next = new int[m+1];
	_get_next(T, next);

	while (i <= n && j <= m) {
		if (j == 0 || S[i-1] == T[j-1]) { ++i;  ++j; }
		else  j = next[j];
	}
	if (j > m)  return  i - m;
	else return 0;
}

void _get_next(char* T, int* next) {
	int j = 1;
	next[1] = 0;
	int k = 0;
	int m;
	int len = 0;
	while (true) {
		if (T[len] == 0) break;
		len++;
	}
	m = len;

	while (j < m) {
		if (k == 0 || T[j] == T[k])
		{
			++j;  ++k; next[j] = k;
		}
		else  k = next[k];
	}
}