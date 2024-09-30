#include <iostream>
#include <vector>
#include <numeric>

template<unsigned int p, unsigned int d>
struct DoIsPrime
{
	static constexpr bool value = (p % d != 0) && DoIsPrime<p, d - 1>::value;
};

template<unsigned int p>
struct DoIsPrime<p, 2>
{
	static constexpr bool value = (p / 2 != 0);
};

template<unsigned int p>
struct IsPrime{
	static constexpr bool value = DoIsPrime<p, p / 2>::value;
};

template<>
struct IsPrime<0> { static constexpr bool value = false; };
template<>
struct IsPrime<1> { static constexpr bool value = false; };
template<>
struct IsPrime<2> { static constexpr bool value = true; };
template<>
struct IsPrime<3> { static constexpr bool value = true; };


// 使用constexpr函数实现上述功能
constexpr bool doIsPrime(unsigned p, unsigned d) {
	return d != 2 ? (p % d != 0) && doIsPrime(p, d - 1) : (p % 2 != 0);
}

constexpr bool isPrime(unsigned p) {
	return p < 4 ?  p >= 2 : doIsPrime(p, p / 2);
}

template<unsigned int  T>
constexpr bool is_prime_v = IsPrime<T>::value;

int main()
{
	std::vector<int> data(100);
	
	std::iota(data.begin(), data.end(), 1);

//	for(auto d : data) {
//		if constexpr (IsPrime<d>::value) {
//			std::cout << d << std::endl;
//		}
//	}


	//std::cout <<std::boolalpha << "13 is prime:" << is_prime_v<1332> << std::endl;
	//std::cout << std::boolalpha << "15 is prime:" << is_prime_v<15452> << std::endl;

	
	std::cout <<std::boolalpha << "13 is prime:" << isPrime(1332) << std::endl;	
	std::cout <<std::boolalpha << "13 is prime:" << isPrime(1332) << std::endl;
}
