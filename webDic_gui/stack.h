#pragma once
const int STACK_MAX_SIZE = 1024;

typedef struct Stack {
private:
	char* s;
	int size;

public:
	Stack(int max_size = STACK_MAX_SIZE);
	int getSize();

	void push(char c);
	char top();
	void pop();
	bool empty();
	void clear();
} Stack;

