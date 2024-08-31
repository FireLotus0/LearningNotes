#include <iostream>

class Derived;


template<typename T>
class Base{
public:
	void func() {
		auto dp = static_cast<T*>(this);
		if(dp == nullptr) {
			std::cout << "cast failed" <<std::endl;
			return;
		}
		dp->dfunc();
	}
};

class Derived : public Base<Derived>
{
public:
	void dfunc() {
		std::cout << "call dfunc() " << std::endl;
	} 
};


class Test{
public:
	Test() { std::cout << "default construct called!" << std::endl; }

	Test(const Test&) { std::cout << "copy construct called!" << std::endl; }

	Test& operator=(const Test&) { std::cout << "copy assignment called!" << std::endl; }

	Test(Test&&) noexcept { std::cout << "move construct called" << std::endl; }

	Test& operator=(Test&&) noexcept { std::cout << "move construct called!" << std::endl; }

};


const Test& testFunc() {
	Test test;
	return test;
}

int main()
{
//	Derived derived;
//	derived.func();

	//auto t1 = testFunc();
	auto& t2 = testFunc();

}
