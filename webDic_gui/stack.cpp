#include "stack.h"

Stack::Stack(int max_size)
{
	this->s = new char[max_size];
	this->size = 0;
}

int Stack::getSize()
{
	return this->size;
}

void Stack::push(char c)
{
	this->s[this->size] = c;
	this->size++;
}

char Stack::top()
{
	if (this->empty())
		return 0;

	return this->s[this->size - 1];
}

void Stack::pop()
{
	if (this->empty()) return;

	this->s[this->size - 1] = 0;
	this->size--;
}

bool Stack::empty()
{
	if (this->size == 0) return true;
	return false;
}

void Stack::clear()
{
	while (this->size != 0)
		this->pop();
}
