通过继承，擦去数据类型信息。基类不包含模版参数，派生类包含模版参数，模版参数是赋值的类型，所以派生类中包含数据类型信息，创建对象时，将派生类指针赋值给基类指针，通过多态的隐式转换擦除数据类型信息，从而可以在基类中保存任何类型的数据信息；还原：向下类型转换得到派生类，获得数据类型信息，若转换失败，抛出异常。type_index（11引入）是一个包装类，内部分装了type_info对象，可以用在关联容器，无序容器中的索引；
```cpp
/*type_index示例*/
#include <iostream>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
 
struct A
{
    virtual ~A() {}
};
 
struct B : A {};
struct C : A {};
 
int main()
{
    std::unordered_map<std::type_index, std::string> type_names;
 
    type_names[std::type_index(typeid(int))] = "int";
    type_names[std::type_index(typeid(double))] = "double";
    type_names[std::type_index(typeid(A))] = "A";
    type_names[std::type_index(typeid(B))] = "B";
    type_names[std::type_index(typeid(C))] = "C";
 
    int i;
    double d;
    A a;
 
    // note that we're storing pointer to type A
    std::unique_ptr<A> b(new B);
    std::unique_ptr<A> c(new C);
 
    std::cout << "i is " << type_names[std::type_index(typeid(i))] << '\n';
    std::cout << "d is " << type_names[std::type_index(typeid(d))] << '\n';
    std::cout << "a is " << type_names[std::type_index(typeid(a))] << '\n';
    std::cout << "*b is " << type_names[std::type_index(typeid(*b))] << '\n';
    std::cout << "*c is " << type_names[std::type_index(typeid(*c))] << '\n';
}
```
```cpp
#include <memory>
#include <typeindex>
#include <exception>
#include <iostream>

struct Any
{
public:
    Any() : m_typeIndex(std::type_index(typeid(void))) {}
    
    Any(const Any& other) 
    : m_ptr(other.clone())
    , m_typeIndex(other.typeIndex)
    {}

    Any(Any&& other)
    : m_ptr(std::move(other.m_ptr))
    , m_typeIndex(other.typeIndex)
    {}

    template<typename T, typename RT = std::decay_t<T>, typename = std::enable_if_t<!std::is_same_v<RT, Any>, RT>>
    Any(T&& value)
    : m_ptr(new Derived<RT>(std::forward<T>(value)))
    , m_typeIndex(std::type_index(typeid(RT)))
    {}

    bool isNull() const {
        return m_ptr != nullptr;
    }

    template<typename T>
    bool isType() const {
        return m_typeIndex == std::type_index(typeid(T));
    }

    template<typename T>
    T& getValue() {
        if(!isType<T>) {
            std::cout << "cast type " << typeid(T).name() << " to "<< m_typeIndex.name() << " failed" <<  std::endl;
            throw std::logic_error("error cast");
        }
        auto derived = dynamic_cast<Derived<T>*>(m_ptr.get());
        return derived->m_value;
    }

    Any& operator=(cosnt Any& other) {
        if(m_ptr == other.m_ptr) {
            return *this;
        }
        m_ptr = other.clone();
        m_typeIndex = other.m_typeIndex;
        return *this;
    }

    Any& operator=(Any&& other) {
        if(m_ptr == other.m_ptr) {
            return *this;
        }
        m_ptr = std::move(other.m_ptr);
        m_typeIndex = other.typeIndex;
        return *this;
    }

private:
    /**
     * @brief 基类只是数据类型的占位符
     */
    struct Base;
    using basePtr = std::unique_ptr<Base>;

    basePtr m_ptr;
    std::type_index m_typeIndex;

    struct Base {
        virtual ~Base() {}
        virtual basePtr clone() const = 0;
    };

    /**
     * @brief T是实际的类型参数
     */
    template<typename T>
    struct Derived : Base
    {
        T m_value;
        template<typename U>
        Derived(U&& value) : m_value(std::forward<U>(value)) {}

        basePtr clone() const {
            return basePtr(new Derived<T>(m_value));
        }
    };

    basePtr clone() const {
        if(m_ptr != nullptr) {
            return m_ptr->clone();
        }
        return nullptr;
    }
};
```