#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>

template<typename Head, typename...Tail>
void are_scalars(Head&& head, Tail&&...tail) {
	using T = std::decay_t<decltype(head)>;

	std::cout << typeid(head).name() << (std::is_scalar_v<Head> ? " is scalar type" : "is not scalar type") << std::endl;
	if constexpr(sizeof...(tail)) {
		are_scalars(tail...);
	}
}


int main()
{
	struct S {int m;} s;
	int S::* mp = &S::m;
	enum class E{e};

	are_scalars(23, 3.22, E::e, "kkk", mp, nullptr, s);

	return 0;
}
