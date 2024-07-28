### cpp编程注意事项

* 重载模版函数，确保在调用该函数之前，所有的重载进行了声明。

* 学习std::ref()和std::cref()的使用

* 模版函数inline：inline的意义仅仅暗示一个函数的定义可以在一个翻译单元多次出现，编译器应当展开函数而不是进行函数调用。现代编译器会做的更好，所以不一定非得显式的inline。

* 模版特化可以用在对于特定类型进行处理，例如，模版传参使用值传递，对于string进行特化使用引用传递

* 模版推导指南的一个用法：如果模版参数是引用传递并且传递的是C-String或者String Literal，由于不会进行decay操作，如果不想使用值传递让他decay，可以使用推导指南：Stack(const char*) -> Stack<std::string>

* 根据传递的参数类型，实现不同的函数的写法：
```cpp
template<auto Val, typename T = decltype(Val)>
T foo();
```

* 浮点数和类类型不能作为非类型模版参数，类类型包括了std::string，而且非类型模版参数通常是常量，包括枚举值

* 变参模版打印
```cpp
void print() {

}

template<typename head, typename...Args>
void print(head h, Args...args)
{
	std::cout << h <<std::endl;
	print(args...);
}

template<typename T>
void print2(T t)
{
	std::cout << t <<std::endl;
}

template<typename T, typename...Args>
void print2(T t, Args...args)
{
	print2(t);
	print2(args...);
}

//**********************使用17 折叠表达式
template<typename T>
class AddSpace
{
private:
    const T& ref;

public:
    AddSpace(const T& cref) : ref(cref) {}

    template<typename S>
    friend std::ostream& operator<<(std::ostream& os, const AddSpace<S>& s) {
        os << s.ref << " ";
    }
};

template<typename...Args>
void print17(Args...args) {
    std::cout << ... << AddSpace<Args>(args) << std::endl;
}

```