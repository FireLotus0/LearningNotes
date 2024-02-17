#include <type_traits>
#include <iostream>

template<typename T>
void checkValueCategory(T&& t) 
{
    if constexpr(std::is_lvalue_reference<decltype((std::forward<T>(t)))>::value) {
        std::cout << "value category is lvalue" << std::endl;
    } 
    else if constexpr(std::is_rvalue_reference<decltype((std::forward<T>(t)))>::value) {
        std::cout << "value category is xvalue" << std::endl;
    }
    else {
        std::cout << "value gategory is rvalue" << std::endl;
    }
}