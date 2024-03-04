Variant类似于union，代表多种定义的类型，允许赋不同类型的值给他。variant的好处在于可以擦除类型信息，不同类型的值都统一成一个variant。取值时通过get<T>(v)，如果T的类型与v的类型不匹配，抛出异常。关键点：
* 需要内存对齐的缓冲区，使用std::aligned_storage
* 赋值时需要将值的类型ID记录下来，以便根据类型取值。将值保存到内部缓冲区时，需要使用placement new在缓冲区创建对象，同时还需要保证variant已定义的类型中存在变量类型。如果通过默认赋值函数，对于内部的缓冲区将导致浅拷贝，两个对象共享缓冲区的问题，所以赋值的正确操作是：从原来的缓冲区中取出对象，通过取出的对象构造新的对象。
* 通过类型取值时，判断类型是否匹配
```cpp
#include <typeindex>
#include <iostream>

/*在编译期间求整数列表中的最大值*/
template<size_t arg, size_t...args>
struct MaxValue;

/*定义编译期间常量，通过MaxValue::value获取*/
template<size_t arg>
struct MaxValue<arg> : std::integral_constant<size_t, arg>
{};

template<size_t arg1, size_t arg2, size_t...args>
struct MaxValue<arg1, arg2, args...> : std::integral_constant<size_t, arg1 >= arg2 ? MaxValue<arg1, args...>::value  : MaxValue<arg2, args...>::value >
{};

/*获取最大的内存对齐*/
template<typename...Args>
struct MaxAlign : std::integral_constant<int, MaxValue<std::alignment_of<Args>::value...>::value>
{};

/*判断是否包含某个类型*/
template<typename T, typename...List>
struct Contains;

template<typename T, typename First, typename...Rest>
struct Contains<T, First, Rest...> : std::conditional<std::is_same_t<T, First>, std::true_type, Contains<T, Rest...>>::type
{};

template<typename T>
struct Contains<T> : std::false_type
{};

template<typename T, typename...List>
struct IndexOf;

template<typename T, typename First, typename...Rest>
struct IndexOf<T, First, Rest...>
{
    enum { value = IndexOf<T, Rest...>::value + 1 };
};

template<typename T, typename...Rest>
struct IndexOf<T, T, Rest...>
{
    enum { value = 0 };
};

template<typename T>
struct IIndexOf<T>
{
    enum { value = -1 };
};

template<int index, typename...Types>
struct At;

template<int index, typename First, typename... Types>
struct At<index, First, Types...>
{
	using type = typename At<index - 1, Types...>::type;
};

template<typename T, typename... Types>
struct At<0, T, Types...>
{
	using type = T;
};

template<typename... Types>
class Variant
{
	enum
	{
		data_size = IntegerMax<sizeof(Types)...>::value,
		align_size = MaxAlign<Types...>::value
	};
	using data_t = typename std::aligned_storage<data_size, align_size>::type;
public:
	template<int index>
	using IndexType = typename At<index, Types...>::type;

	Variant(void) :type_index_(typeid(void))
	{
	}

	~Variant()
	{
		destroy(type_index_, &data_);
	}

	Variant(Variant<Types...>&& old) : type_index_(old.type_index_)
	{
		move(old.type_index_, &old.data_, &data_);
	}

	Variant(const Variant<Types...>& old) : type_index_(old.type_index_)
	{
		copy(old.type_index_, &old.data_, &data_);
	}

	Variant& operator=(const Variant& old)
	{
		copy(old.type_index_, &old.data_, &data_);
		type_index_ = old.type_index_;
		return *this;
	}

	Variant& operator=(Variant&& old)
	{
		move(old.type_index_, &old.data_, &data_);
		type_index_ = old.type_index_;
		return *this;
	}

	template <class T,
	class = typename std::enable_if<Contains<typename std::decay<T>::type, Types...>::value>::type>
		Variant(T&& value) : type_index_(typeid(void))
	{
			destroy(type_index_, &data_);
			typedef typename std::decay<T>::type U;
			new(&data_) U(std::forward<T>(value));
			type_index_ = std::type_index(typeid(U));
	}

	template<typename T>
	bool is() const
	{
		return (type_index_ == std::type_index(typeid(T)));
	}

	bool Empty() const
	{
		return type_index_ == std::type_index(typeid(void));
	}

	std::type_index type() const
	{
		return type_index_;
	}

	template<typename T>
	typename std::decay<T>::type& get()
	{
		using U = typename std::decay<T>::type;
		if (!is<U>())
		{
			std::cout << typeid(U).name() << " is not defined. " 
                << "current type is " << type_index_.name() << std::endl;
			throw std::bad_cast{};
		}

		return *(U*)(&data_);
	}

	template <typename T>
	int indexOf()
	{
		return IndexOf<T, Types...>::value;
	}

	bool operator==(const Variant& rhs) const
	{
		return type_index_ == rhs.type_index_;
	}

	bool operator<(const Variant& rhs) const
	{
		return type_index_ < rhs.type_index_;
	}

private:
	void destroy(const std::type_index& index, void *buf)
	{
        [](Types&&...){}((destroy0<Types>(index, buf), 0)...);
	}

	template<typename T>
	void destroy0(const std::type_index& id, void *data)
	{
		if (id == std::type_index(typeid(T)))
			reinterpret_cast<T*>(data)->~T();
	}

	void move(const std::type_index& old_t, void *old_v, void *new_v) 
	{
        [](Types&&...){}((move0<Types>(old_t, old_v, new_v), 0)...);
	}

	template<typename T>
	void move0(const std::type_index& old_t, void *old_v, void *new_v)
	{
		if (old_t == std::type_index(typeid(T)))
			new (new_v)T(std::move(*reinterpret_cast<T*>(old_v)));
	}

	void copy(const std::type_index& old_t, const void *old_v, void *new_v)
	{
        [](Types&&...){}((copy0<Types>(old_t, old_v, new_v), 0)...);
	}

	template<typename T>
	void copy0(const std::type_index& old_t, const void *old_v, void *new_v)
	{
		if (old_t == std::type_index(typeid(T)))
			new (new_v)T(*reinterpret_cast<const T*>(old_v));
	}

private:
	data_t data_;
	std::type_index type_index_;
};
```