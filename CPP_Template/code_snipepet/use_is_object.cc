#include <type_traits>
#include <iostream>
#include <iomanip>

#define ISOBJECT(...) \ 
	std::cout << std::boolalpha << std::left << std::setw(9) << #__VA_ARGS__ \ 
		<< (std::is_object_v<__VA_ARGS__> ? " is object\n" : "is not object\n")


int main()
{
	class cls{};

	ISOBJECT(void);
	ISOBJECT(int);
	ISOBJECT(int&);
	ISOBJECT(int*);
	ISOBJECT(int*&);
	ISOBJECT(cls);
	ISOBJECT(cls&);
	ISOBJECT(cls*);
	ISOBJECT(int());
	ISOBJECT(int(*)());
	ISOBJECT(int(&)());

	return 0;
}
