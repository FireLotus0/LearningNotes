### c++11学习笔记

* 基于范围的for循环中，冒号后面的表达式只会调用一次。

* 范围for循环工作方式：  
    * 普通数组：起始迭代器执行数组起始地址，终止迭代器等于起始地址+size
    * 类对象：调用成员函数，bengin和end
    * 调用全局的begin和end
因此，自定义类对象，需要实现begin和end函数。  

* 自定义迭代器：实现三个函数：++，!=，*  
```cpp
template<typename T>
class iterator
{
public:
    using value_type = T;
    using size_type = size_t;

private:
    size_type curPos;
    value_type value;
    size_type setp;

public:
    iterator(size_type curPos, value_type beginVal, size_type step );

    Iterator& operator++();

    bool operator!=(const iterator other) const;

    value_type operator*() const;
};

template<typename T>
iterator<T>::ietrator(size_type curPos, value_type value, size_type step)
    : curPos{curPos}
    , value{value}
    , step(syep)
    {}

template<typename T>
iterator<T>& operator++() {
    curPos += step;
    return *this;
}

template<typename T>
bool operator!=(const iterator<T>& other) const{
    return curPos != other.curPos;
}

template<typename T>
value_type operator*() const {
    return value;
} 

```

* 使用std::bind，组合函数成新的函数，简化标准库中bind1st，bind2nd（bind1st，bind2nd用于将二元算子转换成一元算子）:
```cpp
//查找大于10的元素个数
int count = std::count_if(cont.begin(), cont.end(), std::bind1st(less<int>, 10));
//查找小于10的元素个数
int counts = std：：count_if(cont.begin(), cont.end(), std::bind2nd(less<int>, 10));

/*使用std::bind进行简化：传入的函数对象参数固定为两个，容器遍历过程中传入的参数使用占位符*/
using std::placeholders::_1;
int count = std::count_if(cont.begin(). cont.end(), std::bind(less<int>, 10, _1));  //大于10
```  
```cpp
//使用bind组合函数
using std::placeholders::_1;
auto great10 = std::bind(great<int>(), _1, 10); //大于10
auto less20 = std::bind(less<int>(), _1, 20); //小于20
auto func = std::bind(std::logical_and<bool>(), great10, less20);
int count  = std::count_if(cont.begin(), cont.end(), func);
```

* auto使用注意是事项：不能用于std::initializeer_list<>的推导。在函数中返回局部变量的{}是不正确的。

* lambda是从11引入的。 [cap] opt -> ret { body }; 

* mutable的作用：在lambda表达式中，按照c++标准，lambda的operator()默认是const，所以即使按值捕获外部变量，也不能修改，需要加上mutable的函数选项，取消默认的const。被mutable修饰，不管有没有参数都要写()。

* lambda表达式转换成函数指针：当且仅当捕获列表为空。 
* tuple：泛化的pair，使用tuple代替3个以上成员的结构体就不合适了
```cpp
//tie返回一个tuple，并且其中元素是左值引用
auto tp = std::tie(1, "hello", 2)；//std::tuple(int&,string&, int&)
//取tuple中的值：get方法，tie方法
int num = tp.get<0>;
int x,z;
std::stirng str;
std::tie(x, str, z);

//使用tie解包，可以使用std::ignore占位
//创建右值引用的tuple，forward_as_tuple
std::map<int, std::string> mp;
mp.emplace(std::forward_as_tuple(10, std::string("hello")));

//std::tuple_cat
auto t = std::tuple_cat(tp, std::make_pair(10, 10.1), std::tie(10));
```

* 万能的函数包装器：
```cpp
template<typename F. typename...Args>
inline auto FuncWrapper(F&& func, Args&&...args) -> decltype(func(std::forward<Args>(args)...)) 
{
    return func(std::forward<Args>(args)...);
}
```

* 11新增的无序容器：unordered_map，unordered_multimap, unordered_set, unordered_multiset。map和set内部是红黑树，插入元素时会自动进行排序，无序容器内部是哈希表。无序容器对于自定义的key，需要提供哈希函数和比较函数
```cpp
struct MyKey{
    std::string name;
    std::string passwd;
};

struct KeyHash
{
    std::size_t operator()(const MyKey& key) const {
        return std::hash<std::string>(key.name) ^ (std::hash<std::string>(key,passwd) << 1);
    }
};

struct keyEqual
{
    bool operator()(const MyKey& key1, const MyKey& key2) const {
        return key1.name  == key2.name &&
            key1.passwd == key2.passwd;
    }
};

int main() 
{
    std::unordered_map<MyKey, std::size_t, KeyHash, KeyEqual> userMp;
}
```

* 编译期常量：
```cpp
template<typename T>
staruct GetSize {
    static const int size = 1;
    enum{ val = 1 };
};
//使用类型萃取:GetSize::value
template<typename>
struct GetSize : std::integral_constant<int, 1>
{};

//integral_constanct的实现
template<typename T, T v>
struct integral_constant
{
    static const T value = v;
    using value_type = T;
    using type = integral_constanct<T, v>;
    operator value_type() { return value; }
};

//true_type和false_type
using true_type =  integral_constanct<bool, true>;
using false_type = integral_constanct<bool, false>;
```

* 常用类型萃取：
![alt text](./images/01.png)   
![alt text](./images/02.png)   
![alt text](./images/03.png)    
![alt text](./images/04.png)    
![alt text](./images/05.png)   

* std::result_of:使用decltype和declval，decay组合也可以
```cpp
template<typename T, typename...Args>
class result_of<F(Args...)>;
``` 

* 关于缓冲区：总是声明一个char[N]并不对，要考虑内存对齐的问题。使用内存对齐的缓冲区。alignof(T)获取T的内存对齐大小。
```cpp
template<std::size_t Len, std::size_t Align = /*default-alignment*/>
struct aligned_storage{};
```

* 是shared_ptr支持数组：
```cpp
template<typename T>
std::shared_ptr<T> makeSharedArray(std::size_t N)
{
    return std::shared_ptr<T>(new T[N], std::default_delete<T[]>);
    /*
    return std::shared_ptr<T>(new T[N], [](T* ptr) { delete []ptr; });
    */
}
```

* shared_ptr使用注意事项：
    * 不要用同一个原始指针初始化多个shared_ptr对象
    * 不要在传递参数时，创建shared_ptr对象 func(std::shared_ptr<int>(new int), f())；因为参数入栈顺序不确定，有可能先调用了new int，然后调用f()，但是在f()中抛出异常，而shared_ptr对象没有构造完成，不会调用析构函数，导致new int内存泄露。
    * shared_from_this返回this指针，但是不要将this指针作为shared_ptr返回，可能导致二次释放
    ```cpp
    struct A{
        shared_ptr<A> getSelf() {
            return shared_ptr<A>(this);
        }
    };

    int main() {
        shared_ptr<A> p1(new A);
        shared_ptr<A> p2(p1->getSelf());
        return 0;
    }
    /*正确从shared_ptr返回this指针的做法*/
    struct B : std::enable_shared_from_this<B>
    {
        std::shared_ptr<T> getSelf() {
            return shared_from_this();  //街垒的成员函数
        }
    };
    int main() {
        std::shared_ptr<B> p1 (new B);
        std::shared_ptr<B> p2 = p1->getSelf();
        return 0;
    }
    ```
    * 经典问题：循环引用
    ```cpp
    struct A;
    struct B;

    struct A{
        std::shared_ptr<B> aB;
        ~A() { std::cout << "A is delete; " << std::endl; }
    };

    struct B{
        std::shared_ptr<A> bA;
        ~B() { std::cout << "B is delete; " << std::endl; }
    };

    void test()
    {
        std::shared_ptr<A> pA(new A);
        std::shared_ptr<B> pB(new B);
        pA->aB = pB;
        pB->bA  = pA;
    }
    ```

* unique_ptr只能移动赋值，不能拷贝赋值
```cpp
/*实现make_unique类似make_shared*/
//普通指针
template<typename T, typename...Args>
inline std::enable_if_t<!std::is_array_v<T>, std::unique_ptr<T>> make_unique(Args&&...args) 
{
    return unique_ptr<T>(new T(std::foeward<Args>(args)...));
}
//动态数组
template<typename T>
inline std::enable_if_t<std::is_array_v<T> && extent<T>::value == 0, std::unique_ptr<T>> make_unique(std::size_t size)
{
    using U = std::remove_extent_t<T>;
    return std::unqiue_ptr<T>(new U[size]());
}

//过滤掉定长数组
template<typename T, typename...Args>
std::enable_if_t<extent<T>::value != 0, void>  make_unique(Args&&...)= delete;

```
* unique_ptr可以指向一个数组，而shared_ptr不可以
```cpp
std::unqiue_ptr<int[]> p(new int[10]); //ok
std::shared_ptr<int[]> p2(new int[10]); //error
```
* unique_ptr指定删除器和shared_ptr不一样，前者需要明确删除器类型
```cpp
std::unique_ptr<int[], void(*)(int*)> p(new int[10], [](int *p){ delete []p; });    //不能捕获变量，否则lambda不能转换成函数指针

std::unique_ptr<int[], std::function<void()>> p(new int[10], [&](int* s){ delete []s; });   //要捕获变量，使用function包装
```

* weak_ptr是用来监视shared_ptr的，不会使引用计数加1，他不管理shared_ptr内部的指针，主要是监视shared_ptr的生命周期。weak_ptr没有重载*和->运算符，不能操作资源，纯粹作为一个旁观者查看shared_ptr中的资源是否还存在，同时，可以用来返回this指针和解决循环引用的问题。  
* shared_from_this()的原理：std::enable_shareed_from_this类中有一个weak_ptr,调用shared_from_this()时，会调用内部这个weak_ptr的lock方法，将所观测的shared_ptr返回。

* timed_mutex的基本用法:
```cpp
std::timed_mutex tmMutex;

void test() {
    /*设置超时时间*/
    std::chrono::milliseconds timeout(100);
    while(true)
    {
        if(tmMutex.try_lock_for(timeout)) {
            //do something
            tmMutex.unlock();
        } else {
            
        }
    }
}
```

* 条件变量：std::condition_variable（和std::unique_lock一起使用），std::condition_variable_any（和任意mutex一起使用，灵活但是性能比unique_lock低）

* 数值类型和字符串的转换：std::to_string(value)

* 字符串转换成整形和浮点型：
    * atoi：字符串转换为int
    * atol：字符串转long
    * atoll: 字符串转long long
    * atof: 字符串转浮点型

* ***using BaseClass::Function***用于解决派生类隐藏基类同名函数的问题：using声明表示使用基类的同名函数，using BaseClass::BaseClass表示使用基类的构造函数，而不用一一在派生类中定义相应的构造函数，这种方法不会初始化派生类中新定义的数据成员。

* 原始字面量在处理例如文件路径时不需要进行转义。语法：R"xxx(raw string)xxx"

* ***堆内存的对齐方式，使用malloc分配堆内存，也应当考虑堆内存对齐。malloc一般使用当前平台默认的最大对齐数进行对齐，例如MSVC32位下是8字节，64位下是16字节。当我们自定义的内存对齐超过这个范围，则不能直接使用malloc，需要使用_aligned_malloc，在Linux下要使用memalign***


* future用于获异步操作的结果，具有三种状态：
    * Deferred: 异步操作还未开始
    * Ready：异步操作已经完成
    * Timeout：异步操作超时
```cpp
std::future_status status;
do{
    status = future.wait_for(std::chrono::seconds(1));
    if(status == std::future_status::defferred) {

    } else if( status == std::future_status::timeout) {

    } else if(status == std::future_status::ready) {

    }

}while(status != std::future_status::ready);
```

* 异步操作：
    * std::future：用于保存异步操作的执行结果，或者用于获取另一个线程中的数据。不可复制，不能放入容器中，需要使用std::shared_future
    * std::promise：用于包装future，通常用法是在外面创建好promise，获取其future，使用std::ref传递给线程函数，线程函数调用set赋值,外面通过get获取结果  
    * std::packaged_task：将创建线程时，传递线程函数和promise的操作封装起来，只需传递packaged_task给线程即可  
    * std::async：将创建线程这一步骤也封装起来  
* std::async的执行策略：
    * std::launch::async：调用std::async时就创建线程
    * std::launch::deferred：延迟创建，调用async时不创建线程，调用future的get或者wait方法的时候创建线程      

* duration:
```cpp
/*第一个参数是时钟数类型，第二个参数是一个默认模版参数，表示时钟周期*/
template<class Rep, class Period = std::ratio<1, 1>>
class duration;

/*Num代表分子，Demon代表分母。ration<1>代表1s，ratio<60>代表60秒一分钟....*/
template<std::intmax_t Num, std::intmax_t Demon = 1>
class ratio;

/*标准库*/
typedef std::duration<Rep, std::ratio<3600, 1>> hours;
typedef std::duration<Rep, std::ratio<60, 1>> minutes;
//还有seconds, milliseconds, nanoseconds

/*chrono库中获取时钟周期间隔count，时间间隔之间可以计算*/
std::chrono::minutes t1(10);
std::chrono::seconds t2(60);
std::chrono::seconds t3 = t1 - t2;
std::cout << t3.count() << "seconds" << std::endl;

/*计算当前时间距离1970年1月1日有多少天*/
void test() 
{
    using namespace std::chrono;
    using DayType = duration<int, std::ratio<60 * 60 * 24>>>;

    time_point<system_clock, DayType> today = time_point_cast<DayType>(system_clock::now());

    std::cout << today.time_since_epoch().count() << "days";
}
```