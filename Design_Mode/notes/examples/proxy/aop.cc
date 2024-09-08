#include <iostream>
#include <memory>
#include <functional>

struct Aspect
{
using Func = std::function<void()>;

Aspect(const Func& func) 
	: m_func(func)
	{}


template<typename AP>	// Aspect Point
void invoke(AP&& ap) {
	ap.before();
	m_func();
	ap.after();
}

template<typename Head, typename...Tail>	// composit aspects
void invoke(Head&& head, Tail&&...tail) {
	head.before();
	if constexpr(sizeof...(tail) > 0) {
		invoke(tail...);
	}
	head.after();
}

private:
	std::decay_t<Func> m_func;	// 切面
};

template<typename...AP, typename Func>
void invokeHelp(const Func& func) {
	Aspect aspect(func);
	aspect.invoke(AP()...);
}


struct TestA
{
	void before() {
		std::cout << "TestA::before..." << std::endl;
	}

	void after() {
		std::cout << "TestA::after..." << std::endl;
	}
};

struct TestB
{
	void before() {
		std::cout << "TestB::before..." << std::endl;
	}

	void after() {
		std::cout << "TestB::after..." << std::endl;
	}
};

void test() {
	std::cout << "这是切面代码段...." << std::endl;
}


int main() 
{
	invokeHelp<TestA, TestB>(&test);
}

