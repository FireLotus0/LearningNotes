### C++类型萃取

#### injected-class-name:在类定义中定义的类型名，可以在类的作用域中被解释为类类型，通常用于模版编程，在模版编程中可以避免歧义，其可以自动解释为当前模版类实例的完整类型
```cpp
template<typename T>
class Test
{
	using type = Test;	// de这里的Test自动解释为Test<T>
};

```

#### 类型萃取的基类：std::integral_constant。定义了一个static constat 成员变量
```cpp
template<typename T, T v>
struct integral_constant{

	using value_type = T;

	static constexpr  T value = v;
	
	using type = integral_constant;

	constexpr operator value_type() const noexcept { return value;}

	constexpr value_type operator()() const noexcept { return value; }
};

//c++17
template<bool val>
using bool_constant = std::integral_constant<bool,val>;


using true_type = std::integral_constant<bool, true>;
using false_type = std::integral_constant<bool, false>;

```

#### std::is_same  
```cpp
template<typename T, typename U>
struct is_same<T, U> : std::false_type{};

template<typename T>
struct is_same<T, T> : std::true_type{};
```

#### nullptr_t: 是空指针常量nullptr的类型，本身并不是一个指针类型，也不是指向成员的指针类型。nullptr_t的纯右值是***空指针常量***， sizeof(nullpttr_t) == sizeof(void*);

#### std::is_null_pointer
```cpp
template<typename T>
struct is_null_pointer : std::is_same<std::nullptr_t, std::remove_cv_t<T>> {};
```


#### std::is_array:
```cpp

template<typename T>
struct is_array : std::false_type {};


template<typename T>
struct is_array<T[]> : std::true_type {};

template<typename T, std::size_t N>
struct is_array<T[N]> : std::true_type {};

```


#### std::rank
```cpp
template<typename T>
struct rank : std::integral_constant<std::size_t, 0>;

template<typename T>
struct rank<T[]> : std::integral_constant<std::size_t, rank<T>::value + 1>;

template<typename T, std::size_t N>
struct rank<T[N]> : std::integral_constant<std::size_t, rank<T>::value + 1>;

```

#### std::is_class
```cpp
namespace Detail
{
	template<typename T>
	std::integral_constant<bool, !std::is_union<T>::value> test(int T::*);

	template<typename>
	std::false_type test(...)
}

template<typename T>
struct is_class : decltype(Detail::test<T>(nullptr));

```

#### std::is_function
```cpp
template<typename T>
struct is_function : std::integral_constant<bool, !std::is_const<const T>::value && !std::is_refernce<T>::value>{};
```

#### std::is_reference
```cpp
template<typename T>
struct is_reference : std::false_type{};

template<typename T>
struct is_reference<T&> : std::true_type{};

template<typename T>
struct is_reference<T&&> : std::true_type{};

```

####  std::is_object
```cpp
template<typename T>
struct is_object : std::integral_constant<bool,
				std::is_scalar<T>::value ||
				std::is_union<T>::value ||
				std::is_class<T>::value ||
				std::is_array<T>::value>{};

```


#### c++中的标量类型：C++内存模型中的每个单独内存位置，包括语言特性（如虚拟表指针）使用的隐藏内存位置，都具有标量类型（或是非零长度的相邻位字段序列）。表达式求值、线程间同步和依赖关系排序中的副作用排序都是根据单个标量对象定义的。

#### is_polymorphic:
```cpp
namespace Detail
{
	template<typename T>
	std::true_type detect_is_polymorphic(decltype(dynamic_cast<const volatile void*>(static_cast<T*>(nullptr))));

	template<typename T>
	std::false_type detect_is_polymorphic(...);
}

tempalte<typename T>
struct is_polymorphic : decltype(Detail::detect_is_polymorphic<T>(nullptr)){};

```










































