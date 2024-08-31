#include <iostream>
#include <type_traits>

enum class E{e1, e2};

int main()
{
	// 类型别名
	using int_two = std::integral_constant<int, 2>;
	using int_four = std::integral_constant<int, 4>;

	static_assert(!std::is_same_v<int_two, int_four>);
	std::cout << "int_two * 2 = " << int_two::value * 2 << std::endl;
	std::cout << "int_four + 1 = " << int_four() + 1 << std::endl;

	using c1 = std::integral_constant<E, E::e1>;
	using c2 = std::integral_constant<E, E::e2>;

	static_assert(c1::value == E::e1);

	return 0;
}
