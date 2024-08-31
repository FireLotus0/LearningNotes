#include <iostream>
#include <utility>
#include <array>
#include <cstddef>
#include <tuple>

namespace Details
{
template<typename Array, std::size_t... I>
constexpr auto array_to_tuple_impl(const Array& a, std::index_sequence<I...>) {
	return std::tuple(a[I]...);
}

template<typename Ch, typename Tr, typename Tuple, std::size_t...I>
void print_tuple_impl(std::basic_ostream<Ch, Tr>& os, const Tuple& t, std::index_sequence<I...>) {
	((os << (I ? "," : "") << std::get<I>(t)), ...);
}

}

template<typename T, T...ints>
void print_sequence(int id, std::integer_sequence<T, ints...> int_seq) {
	std::cout << id << ") The sequence size is:" << int_seq.size() << std::endl;
	((std::cout << ints << " "), ...);
	std::cout << std::endl;
	
}

template<typename T, std::size_t N, typename Index = std::make_index_sequence<N>>
constexpr auto array_to_tuple(const std::array<T, N>& a) {
	return Details::array_to_tuple_impl(a, Index{});
}

template<typename Ch, typename Tr, typename...Args>
auto& operator<<(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& t) {
	os << "(";
	Details::print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
	return os << ")";
}


int main()
{
	print_sequence(1, std::integer_sequence<unsigned int, 1, 3, 1, 4, 1, 5, 9>{});
	print_sequence(2, std::make_integer_sequence<int, 12>{});
	print_sequence(3, std::make_index_sequence<10>{});
	print_sequence(4, std::index_sequence_for<std::ios, float, signed>{});

	constexpr std::array<int, 4> array{1, 2, 3, 4};
	auto tuple1 = array_to_tuple(array);
	static_assert(std::is_same_v<decltype(tuple1), std::tuple<int, int, int, int>>, "");

	std::cout << "5)" << tuple1 << std::endl;

	constexpr auto tuple2 = array_to_tuple<int, 4, std::integer_sequence<std::size_t, 0, 1,2,3>>(array);

	std::cout << "6)" << tuple2 << std::endl;



	return 0;
}
