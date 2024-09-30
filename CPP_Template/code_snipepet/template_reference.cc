#include <iostream>
#include <type_traits>

template<typename T>
void isReference(T t) {
	std::cout << std::boolalpha << std::is_reference_v<T> << std::endl;
}


template<typename T>
void test(T&& t) {
	 t = 30;
}

template<typename T>
class S
{
	T s;
public:
	S() = default;

	S(T t) : s(t){}
};

int main()
{
	int p = 0;
	int& q = p;

	isReference(p);
	isReference(q);
	isReference<int&>(p);
	isReference<int&>(q);

	std::cout << "t = " << p << std::endl;
	test(p);
	std::cout << "t = " << p << std::endl;


	S<int> tt;
	
}
