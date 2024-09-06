#include <iostream>
#include <cassert>
#include <array>

template<typename T, std::size_t MaxSize>
class Stack;

template<typename T>
std::ostream& operator<<(std::ostream& os, const T& stack) {
	for(auto data : stack) 
	{
		os << data ;
	}
	return os << std::endl;
}


template<typename T, std::size_t MaxSize>
class Stack
{
	using iterator = typename std::array<T, MaxSize>::const_iterator;

public:
	Stack();

	void push(const T& elem);

	void pop();

	T top() const;

	bool empty() const;

	std::size_t size() const;

	iterator begin() const  {
		return elems.begin();
	}

	iterator end() const  {
		return elems.end();
	}
	
	friend std::ostream& operator<<<Stack<T,MaxSize>>(std::ostream& , const Stack<T, MaxSize>&);

private:
	std::array<T, MaxSize> elems;
	std::size_t numSize;
};

template<typename T, std::size_t MaxSize>
Stack<T, MaxSize>::Stack()
	: elems{0}
{}

template<typename T, std::size_t MaxSize>
void Stack<T, MaxSize>::push(const T& elem)
{
	assert(numSize < MaxSize);
	elems[numSize] = elem;
	numSize++;
}

template<typename T, std::size_t MaxSize>
void Stack<T, MaxSize>::pop(){
	assert(!empty());
	numSize--;
}

template<typename T, std::size_t MaxSize>
T Stack<T, MaxSize>::top() const {
	assert(!empty());
	return elems[numSize - 1];
}

template<typename T, std::size_t MaxSize>
std::size_t Stack<T,MaxSize>::size() const  {
	return numSize;
}

template<typename T, std::size_t MaxSize>
bool  Stack<T, MaxSize>::empty() const {
	return numSize == 0;
}

int main()
{
	Stack<int, 10> stack10;
	Stack<int, 20> stack20;

	for(int i = 0; i < 10; i++) {
		stack10.push(i);
	}
	
	std::cout << stack10;

	return 0;
}
